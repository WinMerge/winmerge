/**
 *  @file DirScan.cpp
 *
 *  @brief Implementation of DirScan (q.v.) and helper functions
 */ 

#include "pch.h"
#include "DirScan.h"
#include <cassert>
#include <memory>
#define POCO_NO_UNWINDOWS 1
#include <Poco/Semaphore.h>
#include <Poco/Notification.h>
#include <Poco/NotificationQueue.h>
#include <Poco/Environment.h>
#include <Poco/ThreadPool.h>
#include <Poco/Thread.h>
#include <Poco/Runnable.h>
#include <Poco/Mutex.h>
#include <Poco/AutoPtr.h>
#include <Poco/Stopwatch.h>
#include <Poco/Format.h>
#include "DiffThread.h"
#include "UnicodeString.h"
#include "DiffWrapper.h"
#include "CompareStats.h"
#include "FolderCmp.h"
#include "FileFilterHelper.h"
#include "IAbortable.h"
#include "DirItem.h"
#include "DirTravel.h"
#include "paths.h"
#include "Plugins.h"
#include "MergeAppCOMClass.h"
#include "MergeApp.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"
#include "PathContext.h"
#include "DebugNew.h"

using Poco::NotificationQueue;
using Poco::Notification;
using Poco::AutoPtr;
using Poco::Thread;
using Poco::ThreadPool;
using Poco::Runnable;
using Poco::Environment;
using Poco::Stopwatch;

// Static functions (ie, functions only used locally)
static void CompareDiffItem(FolderCmp &fc, DIFFITEM &di);
static DIFFITEM *AddToList(const String &sLeftDir, const String &sRightDir, const DirItem *lent, const DirItem *rent,
	unsigned code, DiffFuncStruct *myStruct, DIFFITEM *parent);
static DIFFITEM *AddToList(const String &sDir1, const String &sDir2, const String &sDir3, const DirItem *ent1, const DirItem *ent2, const DirItem *ent3,
	unsigned code, DiffFuncStruct *myStruct, DIFFITEM *parent, int nItems = 3);
static void UpdateDiffItem(DIFFITEM &di, bool &bExists, CDiffContext *pCtxt);
static int CompareItems(NotificationQueue &queue, DiffFuncStruct *myStruct, DIFFITEM *parentdiffpos);
static unsigned GetDirCompareFlags3Way(const DIFFITEM& di);

class WorkNotification: public Poco::Notification
{
public:
	WorkNotification(DIFFITEM& di, NotificationQueue& queueResult): m_di(di), m_queueResult(queueResult) {}
	DIFFITEM& data() const { return m_di; }
	NotificationQueue& queueResult() const { return m_queueResult; }
private:
	DIFFITEM& m_di;
	NotificationQueue& m_queueResult;
};

class WorkCompletedNotification: public Poco::Notification
{
public:
	explicit WorkCompletedNotification(DIFFITEM& di): m_di(di) {}
	DIFFITEM& data() const { return m_di; }
private:
	DIFFITEM& m_di;
};

class DiffWorker: public Runnable
{
public:
	DiffWorker(NotificationQueue& queue, CDiffContext *pCtxt, int id):
	  m_queue(queue), m_pCtxt(pCtxt), m_id(id) {}

	void run()
	{
		FolderCmp fc(m_pCtxt);
		// keep the scripts alive during the Rescan
		// when we exit the thread, we delete this and release the scripts
		CAssureScriptsForThread scriptsForRescan(new MergeAppCOMClass());

		AutoPtr<Notification> pNf(m_queue.waitDequeueNotification());
		while (pNf.get() != nullptr)
		{
			WorkNotification* pWorkNf = dynamic_cast<WorkNotification*>(pNf.get());
			if (pWorkNf != nullptr) {
				m_pCtxt->m_pCompareStats->BeginCompare(&pWorkNf->data(), m_id);
				if (!m_pCtxt->ShouldAbort())
					CompareDiffItem(fc, pWorkNf->data());
				pWorkNf->queueResult().enqueueNotification(new WorkCompletedNotification(pWorkNf->data()));
			}
			pNf = m_queue.waitDequeueNotification();
		}
	}

private:
	NotificationQueue& m_queue;
	CDiffContext *m_pCtxt;
	int m_id;
};

typedef std::shared_ptr<DiffWorker> DiffWorkerPtr;

/**
 * @brief Collect file- and folder-names to list.
 * This function walks given folders and adds found subfolders and files into
 * lists. There are two modes, determined by the @p depth:
 * - in non-recursive mode we walk only given folders, and add files
 *   contained. Subfolders are added as folder items, not walked into.
 * - in recursive mode we walk all subfolders and add the files they
 *   contain into list.
 *
 * Items are tested against file filters in this function.
 * 
 * @param [in] paths Root paths of compare
 * @param [in] leftsubdir Left side subdirectory under root path
 * @param [in] bLeftUniq Is left-side folder unique folder?
 * @param [in] rightsubdir Right side subdirectory under root path
 * @param [in] bRightUniq Is right-side folder unique folder?
 * @param [in] myStruct Compare-related data, like context etc.
 * @param [in] casesensitive Is filename compare case sensitive?
 * @param [in] depth Levels of subdirectories to scan, -1 scans all
 * @param [in] parent Folder diff item to be scanned
 * @param [in] bUniques If true, walk into unique folders.
 * @return 1 normally, -1 if compare was aborted
 */
int DirScan_GetItems(const PathContext &paths, const String subdir[],
		DiffFuncStruct *myStruct,
		bool casesensitive, int depth, DIFFITEM *parent,
		bool bUniques)
{
	static const tchar_t backslash[] = _T("\\");
	int nDirs = paths.GetSize();
	CDiffContext *pCtxt = myStruct->context;
	String sDir[3];
	String subprefix[3];

	std::copy(paths.begin(), paths.end(), sDir);

	if (!subdir[0].empty())
	{
		for (int nIndex = 0; nIndex < paths.GetSize(); nIndex++)
		{
			sDir[nIndex] = paths::ConcatPath(sDir[nIndex], subdir[nIndex]);
			subprefix[nIndex] = subdir[nIndex] + backslash;
		}
	}

	DirItemArray dirs[3], aFiles[3];
	for (int nIndex = 0; nIndex < nDirs; nIndex++)
		LoadAndSortFiles(sDir[nIndex], &dirs[nIndex], &aFiles[nIndex], casesensitive);

	// Allow user to abort scanning
	if (pCtxt->ShouldAbort())
		return -1;

	// Handle directories
	// i points to current directory in left list (leftDirs)
	// j points to current directory in right list (rightDirs)

	{
		int nIndex;
		for (nIndex = 0; nIndex < nDirs; nIndex++)
			if (dirs[nIndex].size() != 0 || aFiles[nIndex].size() != 0) break;
		if (nIndex == nDirs)
			return 0;
	}

	DirItemArray::size_type i=0, j=0, k=0;
	while (true)
	{
		if (pCtxt->ShouldAbort())
			return -1;

		if (i >= dirs[0].size() && j >= dirs[1].size() && (nDirs < 3 || k >= dirs[2].size()))
			break;

		unsigned nDiffCode = DIFFCODE::DIR;
		// Comparing directories leftDirs[i].name to rightDirs[j].name
		if (i<dirs[0].size() && (j==dirs[1].size() || collstr(dirs[0][i].filename, dirs[1][j].filename, casesensitive)<0)
			&& (nDirs < 3 ||      (k==dirs[2].size() || collstr(dirs[0][i].filename, dirs[2][k].filename, casesensitive)<0) ))
		{
			nDiffCode |= DIFFCODE::FIRST;
		}
		else if (j<dirs[1].size() && (i==dirs[0].size() || collstr(dirs[1][j].filename, dirs[0][i].filename, casesensitive)<0)
			&& (nDirs < 3 ||      (k==dirs[2].size() || collstr(dirs[1][j].filename, dirs[2][k].filename, casesensitive)<0) ))
		{
			nDiffCode |= DIFFCODE::SECOND;
		}
		else if (nDirs < 3)
		{
			nDiffCode |= DIFFCODE::BOTH;
		}
		else
		{
			if (k<dirs[2].size() && (i==dirs[0].size() || collstr(dirs[2][k].filename, dirs[0][i].filename, casesensitive)<0)
				&&                     (j==dirs[1].size() || collstr(dirs[2][k].filename, dirs[1][j].filename, casesensitive)<0) )
			{
				nDiffCode |= DIFFCODE::THIRD;
			}
			else if ((i<dirs[0].size() && j<dirs[1].size() && collstr(dirs[0][i].filename, dirs[1][j].filename, casesensitive) == 0)
				&& (k==dirs[2].size() || collstr(dirs[2][k].filename, dirs[0][i].filename, casesensitive) != 0))
			{
				nDiffCode |= DIFFCODE::FIRST | DIFFCODE::SECOND;
			}
			else if ((i<dirs[0].size() && k<dirs[2].size() && collstr(dirs[0][i].filename, dirs[2][k].filename, casesensitive) == 0)
				&& (j==dirs[1].size() || collstr(dirs[1][j].filename, dirs[2][k].filename, casesensitive) != 0))
			{
				nDiffCode |= DIFFCODE::FIRST | DIFFCODE::THIRD;
			}
			else if ((j<dirs[1].size() && k<dirs[2].size() && collstr(dirs[1][j].filename, dirs[2][k].filename, casesensitive) == 0)
				&& (i==dirs[0].size() || collstr(dirs[0][i].filename, dirs[1][j].filename, casesensitive) != 0))
			{
				nDiffCode |= DIFFCODE::SECOND | DIFFCODE::THIRD;
			}
			else
			{
				nDiffCode |= DIFFCODE::ALL;
			}
		}

		String leftnewsub;
		String rightnewsub;
		String middlenewsub;
		if (nDirs < 3)
		{
			leftnewsub  = (nDiffCode & DIFFCODE::FIRST)  ? subprefix[0] + dirs[0][i].filename.get() : subprefix[0] + dirs[1][j].filename.get();
			rightnewsub = (nDiffCode & DIFFCODE::SECOND) ? subprefix[1] + dirs[1][j].filename.get() : subprefix[1] + dirs[0][i].filename.get();

			// Test against filter so we don't include contents of filtered out directories
			// Also this is only place we can test for both-sides directories in recursive compare
			if ((pCtxt->m_piFilterGlobal!=nullptr && !pCtxt->m_piFilterGlobal->includeDir(leftnewsub, rightnewsub)) ||
				(pCtxt->m_bIgnoreReparsePoints && (
				(nDiffCode & DIFFCODE::FIRST) && (dirs[0][i].flags.attributes & FILE_ATTRIBUTE_REPARSE_POINT) ||
					(nDiffCode & DIFFCODE::SECOND) && (dirs[1][j].flags.attributes & FILE_ATTRIBUTE_REPARSE_POINT))
					)
				)
				nDiffCode |= DIFFCODE::SKIPPED;
		}
		else
		{
			leftnewsub   = subprefix[0];
			if (nDiffCode & DIFFCODE::FIRST)       leftnewsub += dirs[0][i].filename;
			else if (nDiffCode & DIFFCODE::SECOND) leftnewsub += dirs[1][j].filename;
			else if (nDiffCode & DIFFCODE::THIRD)  leftnewsub += dirs[2][k].filename;
			middlenewsub = subprefix[1];
			if (nDiffCode & DIFFCODE::SECOND)      middlenewsub += dirs[1][j].filename;
			else if (nDiffCode & DIFFCODE::FIRST)  middlenewsub += dirs[0][i].filename;
			else if (nDiffCode & DIFFCODE::THIRD)  middlenewsub += dirs[2][k].filename;
			rightnewsub  = subprefix[2];
			if (nDiffCode & DIFFCODE::THIRD)       rightnewsub += dirs[2][k].filename;
			else if (nDiffCode & DIFFCODE::FIRST)  rightnewsub += dirs[0][i].filename;
			else if (nDiffCode & DIFFCODE::SECOND) rightnewsub += dirs[1][j].filename;

			// Test against filter so we don't include contents of filtered out directories
			// Also this is only place we can test for both-sides directories in recursive compare
			if ((pCtxt->m_piFilterGlobal!=nullptr && !pCtxt->m_piFilterGlobal->includeDir(leftnewsub, middlenewsub, rightnewsub)) ||
				(pCtxt->m_bIgnoreReparsePoints && (
				  (nDiffCode & DIFFCODE::FIRST)  && (dirs[0][i].flags.attributes & FILE_ATTRIBUTE_REPARSE_POINT) ||
				  (nDiffCode & DIFFCODE::SECOND) && (dirs[1][j].flags.attributes & FILE_ATTRIBUTE_REPARSE_POINT) ||
				  (nDiffCode & DIFFCODE::THIRD)  && (dirs[2][k].flags.attributes & FILE_ATTRIBUTE_REPARSE_POINT))
				)
			   )
				nDiffCode |= DIFFCODE::SKIPPED;
		}

		// add to list
		if (!depth)
		{
			if (nDirs < 3)
				AddToList(subdir[0], subdir[1], 
					(nDiffCode & DIFFCODE::FIRST ) ? &dirs[0][i] : nullptr, 
					(nDiffCode & DIFFCODE::SECOND) ? &dirs[1][j] : nullptr,
					nDiffCode, myStruct, parent);
			else
				AddToList(subdir[0], subdir[1], subdir[2], 
					(nDiffCode & DIFFCODE::FIRST ) ? &dirs[0][i] : nullptr,
					(nDiffCode & DIFFCODE::SECOND) ? &dirs[1][j] : nullptr,
					(nDiffCode & DIFFCODE::THIRD ) ? &dirs[2][k] : nullptr,
					nDiffCode, myStruct, parent);
		}
		else
		{
			// Recursive compare
			assert(pCtxt->m_bRecursive);
			if (nDirs < 3)
			{
				DIFFITEM *me = AddToList(subdir[0], subdir[1], 
					(nDiffCode & DIFFCODE::FIRST ) ? &dirs[0][i] : nullptr, 
					(nDiffCode & DIFFCODE::SECOND) ? &dirs[1][j] : nullptr,
					nDiffCode, myStruct, parent);
				if ((nDiffCode & DIFFCODE::SKIPPED) == 0 && ((nDiffCode & DIFFCODE::SIDEFLAGS) == DIFFCODE::BOTH || bUniques))
				{
					// Scan recursively all subdirectories too, we are not adding folders
					String newsubdir[3] = {leftnewsub, rightnewsub};
					int result = DirScan_GetItems(paths, newsubdir, myStruct, casesensitive,
							depth - 1, me, bUniques);
					if (result == -1)
						return -1;
				}
			}
			else
			{
				DIFFITEM *me = AddToList(subdir[0], subdir[1], subdir[2], 
					(nDiffCode & DIFFCODE::FIRST ) ? &dirs[0][i] : nullptr,
					(nDiffCode & DIFFCODE::SECOND) ? &dirs[1][j] : nullptr,
					(nDiffCode & DIFFCODE::THIRD ) ? &dirs[2][k] : nullptr,
					nDiffCode, myStruct, parent);
				if ((nDiffCode & DIFFCODE::SKIPPED) == 0 && ((nDiffCode & DIFFCODE::SIDEFLAGS) == DIFFCODE::ALL || bUniques))
				{
					// Scan recursively all subdirectories too, we are not adding folders
					String newsubdir[3] = {leftnewsub, middlenewsub, rightnewsub};
					int result = DirScan_GetItems(paths, newsubdir, myStruct, casesensitive,
							depth - 1, me, bUniques);
					if (result == -1)
						return -1;
				}
			}
		}
		if (nDiffCode & DIFFCODE::FIRST)
			i++;
		if (nDiffCode & DIFFCODE::SECOND)
			j++;
		if (nDiffCode & DIFFCODE::THIRD)
			k++;
	}
	// Handle files
	// i points to current file in left list (aFiles[0])
	// j points to current file in right list (aFiles[1])
	i=0, j=0, k=0;
	while (true)
	{
		if (pCtxt->ShouldAbort())
			return -1;


		// Comparing file aFiles[0][i].name to aFiles[1][j].name
		if (i<aFiles[0].size() && (j==aFiles[1].size() ||
				collstr(aFiles[0][i].filename, aFiles[1][j].filename, casesensitive) < 0)
			&& (nDirs < 3 || 
				(k==aFiles[2].size() || collstr(aFiles[0][i].filename, aFiles[2][k].filename, casesensitive)<0) ))
		{
			if (nDirs < 3)
			{
				const unsigned nDiffCode = DIFFCODE::FIRST | DIFFCODE::FILE;
				AddToList(subdir[0], subdir[1], &aFiles[0][i], nullptr, nDiffCode, myStruct, parent);
			}
			else
			{
				const unsigned nDiffCode = DIFFCODE::FIRST | DIFFCODE::FILE;
				AddToList(subdir[0], subdir[1], subdir[2], &aFiles[0][i], nullptr, nullptr, nDiffCode, myStruct, parent);
			}
			// Advance left pointer over left-only entry, and then retest with new pointers
			++i;
			continue;
		}
		if (j<aFiles[1].size() && (i==aFiles[0].size() ||
				collstr(aFiles[0][i].filename, aFiles[1][j].filename, casesensitive) > 0)
			&& (nDirs < 3 ||
				(k==aFiles[2].size() || collstr(aFiles[1][j].filename, aFiles[2][k].filename, casesensitive)<0) ))
		{
			const unsigned nDiffCode = DIFFCODE::SECOND | DIFFCODE::FILE;
			if (nDirs < 3)
				AddToList(subdir[0], subdir[1], nullptr, &aFiles[1][j], nDiffCode, myStruct, parent);
			else
				AddToList(subdir[0], subdir[1], subdir[2], nullptr, &aFiles[1][j], nullptr, nDiffCode, myStruct, parent);
			// Advance right pointer over right-only entry, and then retest with new pointers
			++j;
			continue;
		}
		if (nDirs == 3)
		{
			if (k<aFiles[2].size() && (i==aFiles[0].size() ||
					collstr(aFiles[2][k].filename, aFiles[0][i].filename, casesensitive)<0)
				&& (j==aFiles[1].size() || collstr(aFiles[2][k].filename, aFiles[1][j].filename, casesensitive)<0) )
			{
				const unsigned nDiffCode = DIFFCODE::THIRD | DIFFCODE::FILE;
				AddToList(subdir[0], subdir[1], subdir[2], nullptr, nullptr, &aFiles[2][k], nDiffCode, myStruct, parent);
				++k;
				// Advance right pointer over right-only entry, and then retest with new pointers
				continue;
			}

			if ((i<aFiles[0].size() && j<aFiles[1].size() && collstr(aFiles[0][i].filename, aFiles[1][j].filename, casesensitive) == 0)
			    && (k==aFiles[2].size() || collstr(aFiles[0][i].filename, aFiles[2][k].filename, casesensitive) != 0))
			{
				const unsigned nDiffCode = DIFFCODE::FIRST | DIFFCODE::SECOND | DIFFCODE::FILE;
				AddToList(subdir[0], subdir[1], subdir[2], &aFiles[0][i], &aFiles[1][j], nullptr, nDiffCode, myStruct, parent);
				++i;
				++j;
				continue;
			}
			else if ((i<aFiles[0].size() && k<aFiles[2].size() && collstr(aFiles[0][i].filename, aFiles[2][k].filename, casesensitive) == 0)
			    && (j==aFiles[1].size() || collstr(aFiles[1][j].filename, aFiles[2][k].filename, casesensitive) != 0))
			{
				const unsigned nDiffCode = DIFFCODE::FIRST | DIFFCODE::THIRD | DIFFCODE::FILE;
				AddToList(subdir[0], subdir[1], subdir[2], &aFiles[0][i], nullptr, &aFiles[2][k], nDiffCode, myStruct, parent);
				++i;
				++k;
				continue;
			}
			else if ((j<aFiles[1].size() && k<aFiles[2].size() && collstr(aFiles[1][j].filename, aFiles[2][k].filename, casesensitive) == 0)
			    && (i==aFiles[0].size() || collstr(aFiles[0][i].filename, aFiles[1][j].filename, casesensitive) != 0))
			{
				const unsigned nDiffCode = DIFFCODE::SECOND | DIFFCODE::THIRD | DIFFCODE::FILE;
				AddToList(subdir[0], subdir[1], subdir[2], nullptr, &aFiles[1][j], &aFiles[2][k], nDiffCode, myStruct, parent);
				++j;
				++k;
				continue;
			}
		}
		if (i<aFiles[0].size())
		{
			if (nDirs < 3)
			{
				assert(j<aFiles[1].size());
				const unsigned nDiffCode = DIFFCODE::BOTH | DIFFCODE::FILE;
				AddToList(subdir[0], subdir[1], &aFiles[0][i], &aFiles[1][j], nDiffCode, myStruct, parent);
				++i;
				++j;
				continue;
			}
			else
			{
				assert(j<aFiles[1].size());
				assert(k<aFiles[2].size());
				const unsigned nDiffCode = DIFFCODE::ALL | DIFFCODE::FILE;
				AddToList(subdir[0], subdir[1], subdir[2], &aFiles[0][i], &aFiles[1][j], &aFiles[2][k], nDiffCode, myStruct, parent);
				++i;
				++j;
				++k;
				continue;
			}
		}
		break;
	}

	if (parent != nullptr)
	{
		for (int nIndex = 0; nIndex < nDirs; ++nIndex)
			if (parent->diffcode.exists(nIndex) && parent->diffFileInfo[nIndex].size == DirItem::FILE_SIZE_NONE)
				parent->diffFileInfo[nIndex].size = 0;
	
		DIFFITEM *dic = parent->GetFirstChild();
		while (dic)
		{
			for (int nIndex = 0; nIndex < nDirs; ++nIndex)
			{
				if (dic->diffFileInfo[nIndex].size != DirItem::FILE_SIZE_NONE)
					parent->diffFileInfo[nIndex].size += dic->diffFileInfo[nIndex].size;
			}
			dic = dic->GetFwdSiblingLink();
		}
	}

	return 1;
}

/**
 * @brief Compare DiffItems in list and add results to compare context.
 *
 * @param myStruct [in] A structure containing compare-related data.
 * @param parentdiffpos [in] Position of parent diff item 
 * @return >= 0 number of diff items, -1 if compare was aborted
 */
int DirScan_CompareItems(DiffFuncStruct *myStruct, DIFFITEM *parentdiffpos)
{
	const int compareMethod = myStruct->context->GetCompareMethod();
	int nworkers = 1;

	if (compareMethod == CMP_CONTENT || compareMethod == CMP_QUICK_CONTENT)
	{
		nworkers = GetOptionsMgr()->GetInt(OPT_CMP_COMPARE_THREADS);
		if (nworkers <= 0)
			nworkers += Environment::processorCount();
		nworkers = std::clamp(nworkers, 1, static_cast<int>(Environment::processorCount()));
	}

	ThreadPool threadPool(nworkers, nworkers);
	std::vector<DiffWorkerPtr> workers;
	NotificationQueue queue;
	myStruct->context->m_pCompareStats->SetCompareThreadCount(nworkers);
	for (int i = 0; i < nworkers; ++i)
	{
		workers.emplace_back(std::make_shared<DiffWorker>(queue, myStruct->context, i));
		threadPool.start(*workers[i]);
	}

	int res = CompareItems(queue, myStruct, parentdiffpos);

	Thread::sleep(100);
	queue.wakeUpAll();
	threadPool.joinAll();

	return res;
}

static int CompareItems(NotificationQueue& queue, DiffFuncStruct *myStruct, DIFFITEM *parentdiffpos)
{
	NotificationQueue queueResult;
	Stopwatch stopwatch;
	CDiffContext *pCtxt = myStruct->context;
	int res = 0;
	int count = 0;
	bool bCompareFailure = false;
	if (parentdiffpos == nullptr)
		myStruct->pSemaphore->wait();
	stopwatch.start();
	int nDirs = pCtxt->GetCompareDirs();
	DIFFITEM *pos = pCtxt->GetFirstChildDiffPosition(parentdiffpos);
	while (pos != nullptr)
	{
		if (pCtxt->ShouldAbort())
			break;

		if (stopwatch.elapsed() > 2000000)
		{
			int event = CDiffThread::EVENT_COMPARE_PROGRESSED;
			myStruct->m_listeners.notify(myStruct, event);
			stopwatch.restart();
		}
		myStruct->pSemaphore->wait();
		DIFFITEM *curpos = pos;
		DIFFITEM &di = pCtxt->GetNextSiblingDiffRefPosition(pos);
		bool existsalldirs = di.diffcode.existAll();
		if (di.diffcode.isDirectory() && pCtxt->m_bRecursive)
		{
			if ((di.diffcode.diffcode & DIFFCODE::CMPERR) != DIFFCODE::CMPERR)
			{	// Only clear DIFF|SAME flags if not CMPERR (eg. both flags together)
				di.diffcode.diffcode &= ~(DIFFCODE::DIFF | DIFFCODE::SAME);
			}
			int ndiff = CompareItems(queue, myStruct, curpos);
			// Propagate sub-directory status to this directory
			if (ndiff > 0)
			{	// There were differences in the sub-directories
				if (existsalldirs || pCtxt->m_bWalkUniques)
					di.diffcode.diffcode |= DIFFCODE::DIFF;
				res += ndiff;
			}
			else 
			if (ndiff == 0)
			{	// Sub-directories were identical
				if (existsalldirs)
					di.diffcode.diffcode |= DIFFCODE::SAME;
				else if (pCtxt->m_bWalkUniques && !di.diffcode.isResultFiltered())
					di.diffcode.diffcode |= DIFFCODE::DIFF;
			}
			else
			if (ndiff == -1)
			{	// There were file IO-errors during sub-directory comparison.
				di.diffcode.diffcode |= DIFFCODE::CMPERR;
				bCompareFailure = true;
			}

			if (nDirs == 3 && (di.diffcode.diffcode & DIFFCODE::COMPAREFLAGS) == DIFFCODE::DIFF && !di.diffcode.isResultFiltered())
			{
				di.diffcode.diffcode &= ~DIFFCODE::COMPAREFLAGS3WAY;
				di.diffcode.diffcode |= GetDirCompareFlags3Way(di);
			}
		}
		if (existsalldirs)
			queue.enqueueUrgentNotification(new WorkNotification(di, queueResult));
		else
			queue.enqueueNotification(new WorkNotification(di, queueResult));
		++count;
		pos = curpos;
		pCtxt->GetNextSiblingDiffRefPosition(pos);
	}

	while (count > 0)
	{
		AutoPtr<Notification> pNf(queueResult.waitDequeueNotification());
		if (pNf.get() == nullptr)
			break;
		WorkCompletedNotification* pWorkCompletedNf = dynamic_cast<WorkCompletedNotification*>(pNf.get());
		if (pWorkCompletedNf != nullptr) {
			DIFFITEM &di = pWorkCompletedNf->data();
			if (di.diffcode.isResultError()) { 
				DIFFITEM *diParent = di.GetParentLink();
				assert(diParent != nullptr);
				if (diParent != nullptr)
				{
					diParent->diffcode.diffcode |= DIFFCODE::CMPERR;
					bCompareFailure = true;
				}
			}
				
			if (di.diffcode.isResultDiff() ||
				(!di.diffcode.existAll() && !di.diffcode.isResultFiltered()))
				res++;
		}
		--count;
	}

	return bCompareFailure || pCtxt->ShouldAbort() ? -1 : res;
}

/**
 * @brief Compare DiffItems in context marked for rescan.
 *
 * @param myStruct [in,out] A structure containing compare-related data.
 * @param parentdiffpos [in] Position of parent diff item 
 * @return >= 0 number of diff items, -1 if compare was aborted
 */
static int CompareRequestedItems(DiffFuncStruct *myStruct, DIFFITEM *parentdiffpos)
{
	CDiffContext *pCtxt = myStruct->context;
	FolderCmp fc(pCtxt);
	int res = 0;
	bool bCompareFailure = false;
	bool bCompareIndeterminate = false;
	if (parentdiffpos == nullptr)
		myStruct->pSemaphore->wait();

	// Since the collect thread deletes the DiffItems in the rescan by "Refresh selected",
	// the compare thread process should not be executed until the collect thread process is completed 
	// to avoid accessing  the deleted DiffItems.
	assert(myStruct->nCollectThreadState == CDiffThread::THREAD_COMPLETED);

	int nDirs = pCtxt->GetCompareDirs();
	DIFFITEM *pos = pCtxt->GetFirstChildDiffPosition(parentdiffpos);
	while (pos != nullptr)
	{
		if (pCtxt->ShouldAbort())
		{
			res = -1;
			break;
		}

		DIFFITEM *curpos = pos;
		DIFFITEM &di = pCtxt->GetNextSiblingDiffRefPosition(pos);
		bool existsalldirs = di.diffcode.existAll();
		if (di.diffcode.isDirectory())
		{
			if (pCtxt->m_bRecursive)
			{
				di.diffcode.diffcode &= ~(DIFFCODE::DIFF | DIFFCODE::SAME);
				int ndiff = CompareRequestedItems(myStruct, curpos);
				if (ndiff > 0)
				{
					if (existsalldirs || pCtxt->m_bWalkUniques)
						di.diffcode.diffcode |= DIFFCODE::DIFF;
					res += ndiff;
				}
				else 
				if (ndiff == 0)
				{
					if (existsalldirs)
						di.diffcode.diffcode |= DIFFCODE::SAME;
					else if (pCtxt->m_bWalkUniques && !di.diffcode.isResultFiltered())
						di.diffcode.diffcode |= DIFFCODE::DIFF;
				} 
				else
				if (ndiff == -1)
				{	// There were file IO-errors during sub-directory comparison.
					di.diffcode.diffcode |= DIFFCODE::CMPERR;
					bCompareFailure = true;
				}
				else
				if (ndiff == -2)
				{	// There were files that have not been compared
					bCompareIndeterminate = true;
				}

				if (nDirs == 3 && (di.diffcode.diffcode & DIFFCODE::COMPAREFLAGS) == DIFFCODE::DIFF && !di.diffcode.isResultFiltered())
				{
					di.diffcode.diffcode &= ~DIFFCODE::COMPAREFLAGS3WAY;
					di.diffcode.diffcode |= GetDirCompareFlags3Way(di);
				}
			}
		}
		else
		{
			if (di.diffcode.isScanNeeded())
			{
				CompareDiffItem(fc, di);
				if (di.diffcode.isResultError())
				{ 
					DIFFITEM *diParent = di.GetParentLink();
					assert(diParent != nullptr);
					if (diParent != nullptr)
					{
						diParent->diffcode.diffcode |= DIFFCODE::CMPERR;
						bCompareFailure = true;
					}
				}
			}
			else
			{
				if (di.diffcode.isResultError())
					bCompareFailure = true;
				else if (di.diffcode.isResultNone() || di.diffcode.isResultAbort())
					bCompareIndeterminate = true;
			}
		}
		if (di.diffcode.isResultDiff() ||
			(!existsalldirs && !di.diffcode.isResultFiltered()))
			res++;
	}
	return bCompareIndeterminate ? -2 : (bCompareFailure ? -1 : res);
}

int DirScan_CompareRequestedItems(DiffFuncStruct *myStruct, DIFFITEM *parentdiffpos)
{
	return CompareRequestedItems(myStruct, parentdiffpos);
}

static int markChildrenForRescan(CDiffContext *pCtxt, DIFFITEM *parentdiffpos)
{
	int ncount = 0;
	DIFFITEM *pos = pCtxt->GetFirstChildDiffPosition(parentdiffpos);
	while (pos != nullptr)
	{
		DIFFITEM *curpos = pos;
		DIFFITEM &di = pCtxt->GetNextSiblingDiffRefPosition(pos);
		if (di.diffcode.isDirectory())
			ncount += markChildrenForRescan(pCtxt, curpos);
		else
		{
			di.diffcode.diffcode |= DIFFCODE::NEEDSCAN;
			++ncount;
		}
	}
	return ncount;
}

int DirScan_UpdateMarkedItems(DiffFuncStruct *myStruct, DIFFITEM *parentdiffpos)
{
	CDiffContext *pCtxt = myStruct->context;
	DIFFITEM *pos = pCtxt->GetFirstChildDiffPosition(parentdiffpos);
	int ncount = 0;

	while (pos != nullptr)
	{
		if (pCtxt->ShouldAbort())
			break;
		DIFFITEM *curpos = pos;
		DIFFITEM &di = pCtxt->GetNextSiblingDiffRefPosition(pos);
		if (di.diffcode.isScanNeeded())
		{
			bool bItemsExist = true;
			UpdateDiffItem(di, bItemsExist, pCtxt);
			if (!bItemsExist)
			{ 
				di.DelinkFromSiblings();	// delink from list of Siblings
				delete &di;					// Also delete all Children items
				continue;					// (... because `di` is now invalid)
			}
			if (!di.diffcode.isDirectory())
				++ncount;
		}
		if (di.diffcode.isDirectory() && pCtxt->m_bRecursive)
		{
			for (int i = 0; i < pCtxt->GetCompareDirs(); ++i)
				if (di.diffcode.exists(i))
					di.diffFileInfo[i].size = 0;
			if (di.diffcode.isScanNeeded() && !di.diffcode.isResultFiltered())
			{
				di.RemoveChildren();
				di.diffcode.diffcode &= ~DIFFCODE::NEEDSCAN;

				bool casesensitive = false;
				int depth = myStruct->context->m_bRecursive ? -1 : 0;
				String subdir[3];
				PathContext paths = myStruct->context->GetNormalizedPaths();
				for (int i = 0; i < pCtxt->GetCompareDirs(); ++i)
					subdir[i] = di.diffFileInfo[i].GetFile();
				DirScan_GetItems(paths, subdir, myStruct,
					casesensitive, depth, &di, myStruct->context->m_bWalkUniques);
				ncount += markChildrenForRescan(myStruct->context, curpos);
			}
			else
			{
				ncount += DirScan_UpdateMarkedItems(myStruct, curpos);
			}
		}
		if (parentdiffpos != nullptr && pCtxt->m_bRecursive)
		{
			for (int nIndex = 0; nIndex < pCtxt->GetCompareDirs(); ++nIndex)
				if (curpos->diffFileInfo[nIndex].size != DirItem::FILE_SIZE_NONE)
					parentdiffpos->diffFileInfo[nIndex].size += curpos->diffFileInfo[nIndex].size;
		}
	}
	return ncount;
}
/**
 * @brief Update diffitem file/dir infos.
 *
 * Re-tests dirs/files if sides still exists, and updates infos for
 * existing sides. This assumes filenames, or paths are not changed.
 * Since in normal situations (I can think of) they cannot change
 * after first compare.
 *
 * @param [in,out] di DiffItem to update.
 * @param [out] bExists Set to
 *  - true if one of items exists so diffitem is valid
 *  - false if items were deleted, so diffitem is not valid
 * @param [in] pCtxt Compare context
 */
static void UpdateDiffItem(DIFFITEM &di, bool & bExists, CDiffContext *pCtxt)
{
	bExists = false;
	di.diffcode.setSideNone();
	for (int i = 0; i < pCtxt->GetCompareDirs(); ++i)
	{
		di.diffFileInfo[i].ClearPartial();
		if (pCtxt->UpdateInfoFromDiskHalf(di, i))
		{
			bool bUpdated = false;
			if (di.diffFileInfo[i].IsDirectory() == di.diffcode.isDirectory())
			{
				String filepath = paths::ConcatPath(pCtxt->GetPath(i), di.diffFileInfo[i].GetFile());
				if (di.diffFileInfo[i].UpdateFileName(filepath)) {
					di.diffcode.diffcode |= DIFFCODE::FIRST << i;
					bExists = true;
					bUpdated = true;
				}
			}
			if (!bUpdated)
				di.diffFileInfo[i].ClearPartial();
		}
	}
}

/**
 * @brief Compare two diffitems and add results to difflist in context.
 *
 * This function does the actual compare for previously gathered list of
 * items. Basically we:
 * - ignore items matching file filters
 * - add non-ignored directories (no compare for directory items)
 * - add unique files
 * - compare files
 *
 * @param [in] di DiffItem to compare
 * @param [in,out] pCtxt Compare context: contains difflist, encoding info etc.
 * @todo For date compare, maybe we should use creation date if modification
 * date is missing?
 */
static void CompareDiffItem(FolderCmp &fc, DIFFITEM &di)
{
	CDiffContext *const pCtxt = fc.m_pCtxt;
	int nDirs = pCtxt->GetCompareDirs();
	// Clear rescan-request flag (not set by all codepaths)
	di.diffcode.diffcode &= ~DIFFCODE::NEEDSCAN;
	// Is it a directory?
	if (di.diffcode.isDirectory())
	{
		// We don't actually 'compare' directories, just add non-ignored
		// directories to list.
	}
	else
	{
		// 1. Test against filters
		if (pCtxt->m_piFilterGlobal==nullptr ||
			(nDirs == 2 && pCtxt->m_piFilterGlobal->includeFile(
				paths::ConcatPath(di.diffFileInfo[0].path, di.diffFileInfo[0].filename), 
				paths::ConcatPath(di.diffFileInfo[1].path, di.diffFileInfo[1].filename)
			)) ||
			(nDirs == 3 && pCtxt->m_piFilterGlobal->includeFile(
				paths::ConcatPath(di.diffFileInfo[0].path, di.diffFileInfo[0].filename),
				paths::ConcatPath(di.diffFileInfo[1].path, di.diffFileInfo[1].filename),
				paths::ConcatPath(di.diffFileInfo[2].path, di.diffFileInfo[2].filename)
			)))
		{
			di.diffcode.diffcode |= DIFFCODE::INCLUDED;
			di.diffcode.diffcode |= fc.prepAndCompareFiles(di);
			di.nsdiffs = fc.m_ndiffs;
			di.nidiffs = fc.m_ntrivialdiffs;

			for (int i = 0; i < nDirs; ++i)
			{
				// Set text statistics
				if (di.diffcode.exists(i))
				{
					di.diffFileInfo[i].m_textStats = fc.m_diffFileData.m_textStats[i];
					di.diffFileInfo[i].encoding = fc.m_diffFileData.m_FileLocation[i].encoding;
				}
			}
		}
		else
		{
			di.diffcode.diffcode |= DIFFCODE::SKIPPED;
		}
	}
	pCtxt->m_pCompareStats->AddItem(di.diffcode.diffcode);
}

/**
 * @brief Add one compare item to list.
 * @param [in] sLeftDir Left subdirectory.
 * @param [in] sRightDir Right subdirectory.
 * @param [in] lent Left item data to add.
 * @param [in] rent Right item data to add.
 * @param [in] pCtxt Compare context.
 * @param [in] parent Parent of item to be added
 */
static DIFFITEM *AddToList(const String& sLeftDir, const String& sRightDir,
	const DirItem * lent, const DirItem * rent,
	unsigned code, DiffFuncStruct *myStruct, DIFFITEM *parent)
{
	return AddToList(sLeftDir, sRightDir, sLeftDir, lent, rent, nullptr, code, myStruct, parent, 2);
}

/**
 * @brief Add one compare item to list.
 */
static DIFFITEM *AddToList(const String& sDir1, const String& sDir2, const String& sDir3,
	const DirItem *ent1, const DirItem *ent2, const DirItem *ent3,
	unsigned code, DiffFuncStruct *myStruct, DIFFITEM *parent, int nItems /*= 3*/)
{
	// We must store both paths - we cannot get paths later
	// and we need unique item paths for example when items
	// change to identical
	DIFFITEM *di = myStruct->context->AddNewDiff(parent);

	di->diffFileInfo[0].path = sDir1;
	di->diffFileInfo[1].path = sDir2;
	di->diffFileInfo[2].path = sDir3;

	if (ent1 != nullptr)
	{
		di->diffFileInfo[0].filename = ent1->filename;
		di->diffFileInfo[0].mtime = ent1->mtime;
		di->diffFileInfo[0].ctime = ent1->ctime;
		di->diffFileInfo[0].size = ent1->size;
		di->diffFileInfo[0].flags.attributes = ent1->flags.attributes;
	}
	else
	{
		// Don't break CDirView::DoCopyRightToLeft()
		if (ent3 != nullptr)
			di->diffFileInfo[0].filename = ent3->filename;
		else if (ent2 != nullptr)
			di->diffFileInfo[0].filename = ent2->filename;
	}

	if (ent2 != nullptr)
	{
		di->diffFileInfo[1].filename = ent2->filename;
		di->diffFileInfo[1].mtime = ent2->mtime;
		di->diffFileInfo[1].ctime = ent2->ctime;
		di->diffFileInfo[1].size = ent2->size;
		di->diffFileInfo[1].flags.attributes = ent2->flags.attributes;
	}
	else
	{
		// Don't break CDirView::DoCopyLeftToRight()
		if (ent1 != nullptr)
			di->diffFileInfo[1].filename = ent1->filename;
		else if (ent3 != nullptr)
			di->diffFileInfo[1].filename = ent3->filename;
	}

	if (ent3 != nullptr)
	{
		di->diffFileInfo[2].filename = ent3->filename;
		di->diffFileInfo[2].mtime = ent3->mtime;
		di->diffFileInfo[2].ctime = ent3->ctime;
		di->diffFileInfo[2].size = ent3->size;
		di->diffFileInfo[2].flags.attributes = ent3->flags.attributes;
	}
	else
	{
		// Don't break CDirView::DoCopyLeftToRight()
		if (ent1 != nullptr)
			di->diffFileInfo[2].filename = ent1->filename;
		else if (ent2 != nullptr)
			di->diffFileInfo[2].filename = ent2->filename;
	}

	if (nItems == 2)
		di->diffcode.diffcode = code;
	else
		di->diffcode.diffcode = code | DIFFCODE::THREEWAY;

	if (!myStruct->bMarkedRescan && myStruct->m_fncCollect)
	{
		myStruct->context->m_pCompareStats->IncreaseTotalItems();
		myStruct->pSemaphore->set();
	}
	return di;
}

/**
 * @brief Get the 3-way compare flag of the specified directory item.
 * @param [in] di Directory Item to get the 3-way compare flag.
 * @return 3-way compare flag of the specified directory item.
 */
static unsigned GetDirCompareFlags3Way(const DIFFITEM& di)
{
	assert(di.diffcode.isDirectory());
	assert((di.diffcode.diffcode & DIFFCODE::COMPAREFLAGS) == DIFFCODE::DIFF && !di.diffcode.isResultFiltered());

	bool bSet = false;
	unsigned code = DIFFCODE::DIFFALL;

	if (di.diffcode.isSideFirstOnly() || di.diffcode.isMissingFirstOnly())
	{
		code = DIFFCODE::DIFF1STONLY;
		bSet = true;
	}
	else if (di.diffcode.isSideSecondOnly() || di.diffcode.isMissingSecondOnly())
	{
		code = DIFFCODE::DIFF2NDONLY;
		bSet = true;
	}
	else if (di.diffcode.isSideThirdOnly() || di.diffcode.isMissingThirdOnly())
	{
		code = DIFFCODE::DIFF3RDONLY;
		bSet = true;
	}

	if (di.HasChildren())
	{
		for (DIFFITEM* pdi = di.GetFirstChild(); pdi != nullptr; pdi = pdi->GetFwdSiblingLink())
		{
			if ((pdi->diffcode.diffcode & DIFFCODE::COMPAREFLAGS) == DIFFCODE::DIFF && !pdi->diffcode.isResultFiltered())
			{
				if (!bSet)
				{
					code = (pdi->diffcode.diffcode & DIFFCODE::COMPAREFLAGS3WAY);
					bSet = true;
				}
				else
				{
					switch (code)
					{
					case DIFFCODE::DIFF1STONLY:
						if ((pdi->diffcode.diffcode & DIFFCODE::COMPAREFLAGS3WAY) != DIFFCODE::DIFF1STONLY)
							code = DIFFCODE::DIFFALL;
						break;
					case DIFFCODE::DIFF2NDONLY:
						if ((pdi->diffcode.diffcode & DIFFCODE::COMPAREFLAGS3WAY) != DIFFCODE::DIFF2NDONLY)
							code = DIFFCODE::DIFFALL;
						break;
					case DIFFCODE::DIFF3RDONLY:
						if ((pdi->diffcode.diffcode & DIFFCODE::COMPAREFLAGS3WAY) != DIFFCODE::DIFF3RDONLY)
							code = DIFFCODE::DIFFALL;
						break;
					default:
						code = DIFFCODE::DIFFALL;
					}
				}
			}
		}
	}

	return code;
}
