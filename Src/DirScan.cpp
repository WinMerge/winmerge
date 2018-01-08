/**
 *  @file DirScan.cpp
 *
 *  @brief Implementation of DirScan (q.v.) and helper functions
 */ 

#include "DirScan.h"
#include <cassert>
#include <memory>
#include <cstdint>
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
#include "FolderCmp.h"
#include "DirItem.h"
#include "DirTravel.h"
#include "paths.h"
#include "Plugins.h"
#include "MergeApp.h"
#include "OptionsDef.h"
#include "OptionsMgr.h"

using Poco::NotificationQueue;
using Poco::Notification;
using Poco::AutoPtr;
using Poco::Thread;
using Poco::ThreadPool;
using Poco::Runnable;
using Poco::Environment;
using Poco::Stopwatch;

// Static functions (ie, functions only used locally)
void CompareDiffItem(DIFFITEM &di, CDiffContext * pCtxt);
static void StoreDiffData(DIFFITEM &di, CDiffContext * pCtxt,
		const FolderCmp * pCmpData);
static DIFFITEM *AddToList(const String& sLeftDir, const String& sRightDir, const DirItem * lent, const DirItem * rent,
	unsigned code, DiffFuncStruct *myStruct, DIFFITEM *parent);
static DIFFITEM *AddToList(const String& sDir1, const String& sDir2, const String& sDir3, const DirItem * ent1, const DirItem * ent2, const DirItem * ent3,
	unsigned code, DiffFuncStruct *myStruct, DIFFITEM *parent, int nItems = 3);
static void UpdateDiffItem(DIFFITEM & di, bool & bExists, CDiffContext *pCtxt);
static int CompareItems(NotificationQueue& queue, DiffFuncStruct *myStruct, uintptr_t parentdiffpos);

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
		// keep the scripts alive during the Rescan
		// when we exit the thread, we delete this and release the scripts
		CAssureScriptsForThread scriptsForRescan;

		AutoPtr<Notification> pNf(m_queue.waitDequeueNotification());
		while (pNf)
		{
			WorkNotification* pWorkNf = dynamic_cast<WorkNotification*>(pNf.get());
			if (pWorkNf) {
				m_pCtxt->m_pCompareStats->BeginCompare(&pWorkNf->data(), m_id);
				if (!m_pCtxt->ShouldAbort())
					CompareDiffItem(pWorkNf->data(), m_pCtxt);
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
 * - in non-recursive mode we walk only given folders, and add files.
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
 * @param [in] casesensitive Is filename compare casesensitive?
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
	static const TCHAR backslash[] = _T("\\");
	int nDirs = paths.GetSize();
	CDiffContext *pCtxt = myStruct->context;
	String sDir[3];
	String subprefix[3];

	int nIndex;
	std::copy(paths.begin(), paths.end(), sDir);

	if (!subdir[0].empty())
	{
		for (nIndex = 0; nIndex < paths.GetSize(); nIndex++)
		{
			sDir[nIndex] = paths::ConcatPath(sDir[nIndex], subdir[nIndex]);
			subprefix[nIndex] = subdir[nIndex] + backslash;
		}
	}

	DirItemArray dirs[3], files[3];
	for (nIndex = 0; nIndex < nDirs; nIndex++)
		LoadAndSortFiles(sDir[nIndex], &dirs[nIndex], &files[nIndex], casesensitive);

	// Allow user to abort scanning
	if (pCtxt->ShouldAbort())
		return -1;

	// Handle directories
	// i points to current directory in left list (leftDirs)
	// j points to current directory in right list (rightDirs)

	for (nIndex = 0; nIndex < nDirs; nIndex++)
		if (dirs[nIndex].size() != 0 || files[nIndex].size() != 0) break;
	if (nIndex == nDirs)
		return 0;

	DirItemArray::size_type i=0, j=0, k=0;
	while (1)
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
			if ((pCtxt->m_piFilterGlobal && !pCtxt->m_piFilterGlobal->includeDir(leftnewsub, rightnewsub)) ||
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
			if ((pCtxt->m_piFilterGlobal && !pCtxt->m_piFilterGlobal->includeDir(leftnewsub, middlenewsub, rightnewsub)) ||
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
					(nDiffCode & DIFFCODE::FIRST ) ? &dirs[0][i] : NULL, 
					(nDiffCode & DIFFCODE::SECOND) ? &dirs[1][j] : NULL,
					nDiffCode, myStruct, parent);
			else
				AddToList(subdir[0], subdir[1], subdir[2], 
					(nDiffCode & DIFFCODE::FIRST ) ? &dirs[0][i] : NULL,
					(nDiffCode & DIFFCODE::SECOND) ? &dirs[1][j] : NULL,
					(nDiffCode & DIFFCODE::THIRD ) ? &dirs[2][k] : NULL,
					nDiffCode, myStruct, parent);
		}
		else
		{
			// Recursive compare
			if (nDirs < 3)
			{
				DIFFITEM *me = AddToList(subdir[0], subdir[1], 
					(nDiffCode & DIFFCODE::FIRST ) ? &dirs[0][i] : NULL, 
					(nDiffCode & DIFFCODE::SECOND) ? &dirs[1][j] : NULL,
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
					(nDiffCode & DIFFCODE::FIRST ) ? &dirs[0][i] : NULL,
					(nDiffCode & DIFFCODE::SECOND) ? &dirs[1][j] : NULL,
					(nDiffCode & DIFFCODE::THIRD ) ? &dirs[2][k] : NULL,
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
	// i points to current file in left list (files[0])
	// j points to current file in right list (files[1])
	i=0, j=0, k=0;
	while (1)
	{
		if (pCtxt->ShouldAbort())
			return -1;


		// Comparing file files[0][i].name to files[1][j].name
		if (i<files[0].size() && (j==files[1].size() ||
				collstr(files[0][i].filename, files[1][j].filename, casesensitive) < 0)
			&& (nDirs < 3 || 
				(k==files[2].size() || collstr(files[0][i].filename, files[2][k].filename, casesensitive)<0) ))
		{
			if (nDirs < 3)
			{
				const unsigned nDiffCode = DIFFCODE::FIRST | DIFFCODE::FILE;
				AddToList(subdir[0], subdir[1], &files[0][i], 0, nDiffCode, myStruct, parent);
			}
			else
			{
				const unsigned nDiffCode = DIFFCODE::FIRST | DIFFCODE::FILE;
				AddToList(subdir[0], subdir[1], subdir[2], &files[0][i], 0, 0, nDiffCode, myStruct, parent);
			}
			// Advance left pointer over left-only entry, and then retest with new pointers
			++i;
			continue;
		}
		if (j<files[1].size() && (i==files[0].size() ||
				collstr(files[0][i].filename, files[1][j].filename, casesensitive) > 0)
			&& (nDirs < 3 ||
				(k==files[2].size() || collstr(files[1][j].filename, files[2][k].filename, casesensitive)<0) ))
		{
			const unsigned nDiffCode = DIFFCODE::SECOND | DIFFCODE::FILE;
			if (nDirs < 3)
				AddToList(subdir[0], subdir[1], 0, &files[1][j], nDiffCode, myStruct, parent);
			else
				AddToList(subdir[0], subdir[1], subdir[2], 0, &files[1][j], 0, nDiffCode, myStruct, parent);
			// Advance right pointer over right-only entry, and then retest with new pointers
			++j;
			continue;
		}
		if (nDirs == 3)
		{
			if (k<files[2].size() && (i==files[0].size() ||
					collstr(files[2][k].filename, files[0][i].filename, casesensitive)<0)
				&& (j==files[1].size() || collstr(files[2][k].filename, files[1][j].filename, casesensitive)<0) )
			{
				const unsigned nDiffCode = DIFFCODE::THIRD | DIFFCODE::FILE;
				AddToList(subdir[0], subdir[1], subdir[2], 0, 0, &files[2][k], nDiffCode, myStruct, parent);
				++k;
				// Advance right pointer over right-only entry, and then retest with new pointers
				continue;
			}

			if ((i<files[0].size() && j<files[1].size() && collstr(files[0][i].filename, files[1][j].filename, casesensitive) == 0)
			    && (k==files[2].size() || collstr(files[0][i].filename, files[2][k].filename, casesensitive) != 0))
			{
				const unsigned nDiffCode = DIFFCODE::FIRST | DIFFCODE::SECOND | DIFFCODE::FILE;
				AddToList(subdir[0], subdir[1], subdir[2], &files[0][i], &files[1][j], 0, nDiffCode, myStruct, parent);
				++i;
				++j;
				continue;
			}
			else if ((i<files[0].size() && k<files[2].size() && collstr(files[0][i].filename, files[2][k].filename, casesensitive) == 0)
			    && (j==files[1].size() || collstr(files[1][j].filename, files[2][k].filename, casesensitive) != 0))
			{
				const unsigned nDiffCode = DIFFCODE::FIRST | DIFFCODE::THIRD | DIFFCODE::FILE;
				AddToList(subdir[0], subdir[1], subdir[2], &files[0][i], 0, &files[2][k], nDiffCode, myStruct, parent);
				++i;
				++k;
				continue;
			}
			else if ((j<files[1].size() && k<files[2].size() && collstr(files[1][j].filename, files[2][k].filename, casesensitive) == 0)
			    && (i==files[0].size() || collstr(files[0][i].filename, files[1][j].filename, casesensitive) != 0))
			{
				const unsigned nDiffCode = DIFFCODE::SECOND | DIFFCODE::THIRD | DIFFCODE::FILE;
				AddToList(subdir[0], subdir[1], subdir[2], 0, &files[1][j], &files[2][k], nDiffCode, myStruct, parent);
				++j;
				++k;
				continue;
			}
		}
		if (i<files[0].size())
		{
			if (nDirs < 3)
			{
				assert(j<files[1].size());
				const unsigned nDiffCode = DIFFCODE::BOTH | DIFFCODE::FILE;
				AddToList(subdir[0], subdir[1], &files[0][i], &files[1][j], nDiffCode, myStruct, parent);
				++i;
				++j;
				continue;
			}
			else
			{
				assert(j<files[1].size());
				assert(k<files[2].size());
				const unsigned nDiffCode = DIFFCODE::ALL | DIFFCODE::FILE;
				AddToList(subdir[0], subdir[1], subdir[2], &files[0][i], &files[1][j], &files[2][k], nDiffCode, myStruct, parent);
				++i;
				++j;
				++k;
				continue;
			}
		}
		break;
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
int DirScan_CompareItems(DiffFuncStruct *myStruct, uintptr_t parentdiffpos)
{
	const int compareMethod = myStruct->context->GetCompareMethod();
	int nworkers = 1;

	if (compareMethod == CMP_CONTENT || compareMethod == CMP_QUICK_CONTENT)
	{
		nworkers = GetOptionsMgr()->GetInt(OPT_CMP_COMPARE_THREADS);
		if (nworkers <= 0)
		{
			nworkers += Environment::processorCount();
			if (nworkers <= 0)
				nworkers = 1;
		}
	}

	ThreadPool threadPool(nworkers, nworkers);
	std::vector<DiffWorkerPtr> workers;
	NotificationQueue queue;
	myStruct->context->m_pCompareStats->SetCompareThreadCount(nworkers);
	for (int i = 0; i < nworkers; ++i)
	{
		workers.push_back(DiffWorkerPtr(new DiffWorker(queue, myStruct->context, i)));
		threadPool.start(*workers[i]);
	}

	int res = CompareItems(queue, myStruct, parentdiffpos);

	Thread::sleep(100);
	queue.wakeUpAll();
	threadPool.joinAll();

	return res;
}

static int CompareItems(NotificationQueue& queue, DiffFuncStruct *myStruct, uintptr_t parentdiffpos)
{
	NotificationQueue queueResult;
	Stopwatch stopwatch;
	CDiffContext *pCtxt = myStruct->context;
	int res = 0;
	int count = 0;
	if (!parentdiffpos)
		myStruct->pSemaphore->wait();
	stopwatch.start();
	uintptr_t pos = pCtxt->GetFirstChildDiffPosition(parentdiffpos);
	while (pos)
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
		uintptr_t curpos = pos;
		DIFFITEM &di = pCtxt->GetNextSiblingDiffRefPosition(pos);
		bool existsalldirs = di.diffcode.existAll();
		if (di.diffcode.isDirectory() && pCtxt->m_bRecursive)
		{
			di.diffcode.diffcode &= ~(DIFFCODE::DIFF | DIFFCODE::SAME);
			int ndiff = CompareItems(queue, myStruct, curpos);
			if (ndiff > 0)
			{
				if (existsalldirs)
					di.diffcode.diffcode |= DIFFCODE::DIFF;
				res += ndiff;
			}
			else if (ndiff == 0)
			{
				if (existsalldirs)
					di.diffcode.diffcode |= DIFFCODE::SAME;
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
		if (!pNf)
			break;
		WorkCompletedNotification* pWorkCompletedNf = dynamic_cast<WorkCompletedNotification*>(pNf.get());
		if (pWorkCompletedNf) {
			DIFFITEM &di = pWorkCompletedNf->data();
			if (di.diffcode.isResultDiff() ||
				(!di.diffcode.existAll() && !di.diffcode.isResultFiltered()))
				res++;
		}
		--count;
	}

	return pCtxt->ShouldAbort() ? -1 : res;
}

/**
 * @brief Compare DiffItems in context marked for rescan.
 *
 * @param myStruct [in,out] A structure containing compare-related data.
 * @param parentdiffpos [in] Position of parent diff item 
 * @return >= 0 number of diff items, -1 if compare was aborted
 */
static int CompareRequestedItems(DiffFuncStruct *myStruct, uintptr_t parentdiffpos)
{
	CDiffContext *pCtxt = myStruct->context;
	int res = 0;
	uintptr_t pos = pCtxt->GetFirstChildDiffPosition(parentdiffpos);
	
	while (pos != NULL)
	{
		if (pCtxt->ShouldAbort())
		{
			res = -1;
			break;
		}

		uintptr_t curpos = pos;
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
					if (existsalldirs)
						di.diffcode.diffcode |= DIFFCODE::DIFF;
					res += ndiff;
				}
				else if (ndiff == 0)
				{
					if (existsalldirs)
						di.diffcode.diffcode |= DIFFCODE::SAME;
				}
			}
		}
		else
		{
			if (di.diffcode.isScanNeeded())
				CompareDiffItem(di, pCtxt);
		}
		if (di.diffcode.isResultDiff() ||
			(!existsalldirs && !di.diffcode.isResultFiltered()))
			res++;
	}
	return res;
}

int DirScan_CompareRequestedItems(DiffFuncStruct *myStruct, uintptr_t parentdiffpos)
{
	CAssureScriptsForThread scriptsForRescan;
	return CompareRequestedItems(myStruct, parentdiffpos);
}

static int markChildrenForRescan(CDiffContext *pCtxt, uintptr_t parentdiffpos)
{
	int ncount = 0;
	uintptr_t pos = pCtxt->GetFirstChildDiffPosition(parentdiffpos);
	while (pos != NULL)
	{
		uintptr_t curpos = pos;
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

int DirScan_UpdateMarkedItems(DiffFuncStruct *myStruct, uintptr_t parentdiffpos)
{
	CDiffContext *pCtxt = myStruct->context;
	uintptr_t pos = pCtxt->GetFirstChildDiffPosition(parentdiffpos);
	int ncount = 0;
	
	while (pos != NULL)
	{
		if (pCtxt->ShouldAbort())
			break;
		uintptr_t curpos = pos;
		DIFFITEM &di = pCtxt->GetNextSiblingDiffRefPosition(pos);
		bool bItemsExist = true;
		if (di.diffcode.isScanNeeded())
		{
			UpdateDiffItem(di, bItemsExist, pCtxt);
			if (!bItemsExist)
				di.RemoveSelf();
			else if (!di.diffcode.isDirectory())
				++ncount;
		}
		if (bItemsExist && di.diffcode.isDirectory() && pCtxt->m_bRecursive)
		{
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
static void UpdateDiffItem(DIFFITEM & di, bool & bExists, CDiffContext *pCtxt)
{
	bExists = false;
	di.diffcode.setSideNone();
	for (int i = 0; i < pCtxt->GetCompareDirs(); ++i)
	{
		di.diffFileInfo[i].ClearPartial();
		if (pCtxt->UpdateInfoFromDiskHalf(di, i))
		{
			di.diffcode.diffcode |= DIFFCODE::FIRST << i;
			bExists = true;
		}
	}
}

/**
 * @brief Compare two diffitems and add results to difflist in context.
 *
 * This function does the actual compare for previously gathered list of
 * items. Basically we:
 * - ignore items matching filefilters
 * - add non-ignored directories (no compare for directory items)
 * - add  unique files
 * - compare files
 *
 * @param [in] di DiffItem to compare
 * @param [in,out] pCtxt Compare context: contains difflist, encoding info etc.
 * @todo For date compare, maybe we should use creation date if modification
 * date is missing?
 */
void CompareDiffItem(DIFFITEM &di, CDiffContext * pCtxt)
{
	int nDirs = pCtxt->GetCompareDirs();
	// Clear rescan-request flag (not set by all codepaths)
	di.diffcode.diffcode &= ~DIFFCODE::NEEDSCAN;
	// Is it a directory?
	if (di.diffcode.isDirectory())
	{
		// We don't actually 'compare' directories, just add non-ignored
		// directories to list.
		StoreDiffData(di, pCtxt, NULL);
	}
	else
	{
		// 1. Test against filters
		if (!pCtxt->m_piFilterGlobal ||
			(nDirs == 2 && pCtxt->m_piFilterGlobal->includeFile(di.diffFileInfo[0].filename, di.diffFileInfo[1].filename)) ||
			(nDirs == 3 && pCtxt->m_piFilterGlobal->includeFile(di.diffFileInfo[0].filename, di.diffFileInfo[1].filename, di.diffFileInfo[2].filename))
			)
		{
			di.diffcode.diffcode |= DIFFCODE::INCLUDED;
			FolderCmp folderCmp;
			di.diffcode.diffcode |= folderCmp.prepAndCompareFiles(pCtxt, di);
			StoreDiffData(di, pCtxt, &folderCmp);
		}
		else
		{
			di.diffcode.diffcode |= DIFFCODE::SKIPPED;
			StoreDiffData(di, pCtxt, NULL);
		}
	}
}

/**
 * @brief Send one file or directory result back through the diff context.
 * @param [in] di Data to store.
 * @param [in] pCtxt Compare context.
 * @param [in] pCmpData Folder compare data.
 */
static void StoreDiffData(DIFFITEM &di, CDiffContext * pCtxt,
		const FolderCmp * pCmpData)
{
	if (pCmpData)
	{
		di.nsdiffs = pCmpData->m_ndiffs;
		di.nidiffs = pCmpData->m_ntrivialdiffs;

		for (int i = 0; i < pCtxt->GetCompareDirs(); ++i)
		{
			// Set text statistics
			if (di.diffcode.exists(i))
			{
				di.diffFileInfo[i].m_textStats = pCmpData->m_diffFileData.m_textStats[i];
				di.diffFileInfo[i].encoding = pCmpData->m_diffFileData.m_FileLocation[i].encoding;
			}
		}
	}

	pCtxt->m_pCompareStats->AddItem(di.diffcode.diffcode);
	//pCtxt->AddDiff(di);
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
	return AddToList(sLeftDir, sRightDir, sLeftDir, lent, rent, 0, code, myStruct, parent, 2);
}

/**
 * @brief Add one compare item to list.
 */
static DIFFITEM *AddToList(const String& sDir1, const String& sDir2, const String& sDir3,
	const DirItem * ent1, const DirItem * ent2, const DirItem * ent3,
	unsigned code, DiffFuncStruct *myStruct, DIFFITEM *parent, int nItems)
{
	// We must store both paths - we cannot get paths later
	// and we need unique item paths for example when items
	// change to identical
	DIFFITEM *di = myStruct->context->AddDiff(parent);

	di->parent = parent;
	di->diffFileInfo[0].path = sDir1;
	di->diffFileInfo[1].path = sDir2;
	di->diffFileInfo[2].path = sDir3;

	if (ent1)
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
		if (ent3)
			di->diffFileInfo[0].filename = ent3->filename;
		else if (ent2)
			di->diffFileInfo[0].filename = ent2->filename;
	}

	if (ent2)
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
		if (ent1)
			di->diffFileInfo[1].filename = ent1->filename;
		else if (ent3)
			di->diffFileInfo[1].filename = ent3->filename;
	}

	if (ent3)
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
		if (ent1)
			di->diffFileInfo[2].filename = ent1->filename;
		else if (ent2)
			di->diffFileInfo[2].filename = ent2->filename;
	}

	if (nItems == 2)
		di->diffcode.diffcode = code;
	else
		di->diffcode.diffcode = code | DIFFCODE::THREEWAY;

	myStruct->context->m_pCompareStats->IncreaseTotalItems();
	myStruct->pSemaphore->set();
	return di;
}
