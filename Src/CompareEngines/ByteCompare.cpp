/** 
 * @file  ByteCompare.cpp
 *
 * @brief Implementation file for ByteCompare
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "StdAfx.h"
#include <io.h>
#include "FileLocation.h"
#include "UnicodeString.h"
#include "IAbortable.h"
#include "CompareOptions.h"
#include "DiffContext.h"
#include "DIFF.H"
#include "ByteComparator.h"
#include "ByteCompare.h"

namespace CompareEngines
{

static const int KILO = 1024; // Kilo(byte)

/** @brief Quick contents compare's file buffer size. */
static const int WMCMPBUFF = 32 * KILO;

static void CopyTextStats(const FileTextStats * stats, FileTextStats * myTextStats);

/**
 * @brief Default constructor.
 */
ByteCompare::ByteCompare()
: m_pOptions(NULL)
, m_piAbortable(NULL)
, m_inf(NULL)
{
}

/**
 * @brief Default destructor.
 */
ByteCompare::~ByteCompare()
{
	ClearCompareOptions();
}

/**
 * @brief Set compare options from general compare options.
 * @param [in ]options General compare options.
 * @return true if succeeded, false otherwise.
 */
bool ByteCompare::SetCompareOptions(const CompareOptions & options)
{
	if (m_pOptions != NULL)
		ClearCompareOptions();

	m_pOptions = new QuickCompareOptions(options);
	if (m_pOptions == NULL)
		return false;
	return true;
}

/**
 * @brief Clear current compare options.
 */
void ByteCompare::ClearCompareOptions()
{
	delete m_pOptions;
	m_pOptions = NULL;
}

/**
 * @brief Set compare-type specific options.
 * @param [in] stopAfterFirstDiff Do we stop compare after first found diff.
 */
void ByteCompare::SetAdditionalOptions(bool stopAfterFirstDiff)
{
	m_pOptions->m_bStopAfterFirstDiff = stopAfterFirstDiff;
}

/**
 * @brief Set Abortable-interface.
 * @param [in] piAbortable Pointer to abortable interface.
 */
void ByteCompare::SetAbortable(const IAbortable * piAbortable)
{
	m_piAbortable = const_cast<IAbortable*>(piAbortable);
}

/**
 * @brief Set filedata.
 * @param [in] items Count of filedata items to set.
 * @param [in] data File data.
 * @note Path names are set by SetPaths() function.
 */
void ByteCompare::SetFileData(int items, file_data *data)
{
	// We support only two files currently!
	ASSERT(items == 2);
	m_inf = data;
}


/** 
 * @brief Compare two specified files, byte-by-byte
 * @param [in] bStopAfterFirstDiff Stop compare after we find first difference?
 * @param [in] piAbortable Interface allowing to abort compare
 * @return DIFFCODE
 */
int ByteCompare::CompareFiles(FileLocation *location)
{
	// TODO
	// Right now, we assume files are in 8-bit encoding
	// because transform code converted any UCS-2 files to UTF-8
	// We could compare directly in UCS-2LE here, as an optimization, in that case
	char buff[2][WMCMPBUFF]; // buffered access to files
	int i;
	int result;
	UINT diffcode = 0;

	// area of buffer currently holding data
	INT64 bfstart[2]; // offset into buff[i] where current data resides
	INT64 bfend[2]; // past-the-end pointer into buff[i], giving end of current data
	// buff[0] has bytes to process from buff[0][bfstart[0]] to buff[0][bfend[0]-1]

	bool eof[2]; // if we've finished file

	// initialize our buffer pointers and end of file flags
	for (i=0; i<2; ++i)
	{
		bfstart[i] = bfend[i] = 0;
		eof[i] = false;
	}

	ByteComparator comparator(m_pOptions);

	// Begin loop
	// we handle the files in WMCMPBUFF sized buffers (variable buff[][])
	// That is, we do one buffer full at a time
	// or even less, as we process until one side buffer is empty, then reload that one
	// and continue
	while (!eof[0] || !eof[1])
	{
		if (m_piAbortable && m_piAbortable->ShouldAbort())
			return DIFFCODE::CMPABORT;

		// load or update buffers as appropriate
		for (i=0; i<2; ++i)
		{
			if (!eof[i] && bfstart[i]==countof(buff[i]))
			{
				bfstart[i] = bfend[i] = 0;
			}
			if (!eof[i] && bfend[i]<countof(buff[i])-1)
			{
				// Assume our blocks are in range of int
				int space = RTL_NUMBER_OF(buff[i]) - (int) bfend[i];
				int rtn = read(m_inf[i].desc, &buff[i][bfend[i]], (unsigned int)space);
				if (rtn == -1)
					return DIFFCODE::CMPERR;
				if (rtn < space)
					eof[i] = true;
				bfend[i] += rtn;
				if (m_inf[0].desc == m_inf[1].desc)
				{
					location[1].encoding = location[0].encoding;
					return diffcode | DIFFCODE::SAME;
				}
			}
		}
		// where to start comparing right now
		const char* ptr0 = &buff[0][bfstart[0]];
		const char* ptr1 = &buff[1][bfstart[1]];

		// remember where we started
		const char* orig0 = ptr0;
		const char* orig1 = ptr1;

		// how far can we go right now?
		const char* end0 = &buff[0][bfend[0]];
		const char* end1 = &buff[1][bfend[1]];

		INT64 offset0 = (ptr0 - &buff[0][0]);
		INT64 offset1 = (ptr1 - &buff[1][0]);

		// are these two buffers the same?
		result = comparator.CompareBuffers(m_textStats[0], m_textStats[1],
			ptr0, ptr1, end0, end1, eof[0], eof[1], offset0, offset1);
		if (result == ByteComparator::RESULT_DIFF)
		{
			if (m_pOptions->m_bStopAfterFirstDiff)
			{
				// By bailing out here
				// we leave our text statistics incomplete
				return diffcode | DIFFCODE::DIFF;
			}
			else
			{
				diffcode |= DIFFCODE::DIFF;
				ptr0 = end0;
				ptr1 = end1;
				// move our current pointers over what we just compared
				ASSERT(ptr0 >= orig0);
				ASSERT(ptr1 >= orig1);
				bfstart[0] += ptr0 - orig0;
				bfstart[1] += ptr1 - orig1;
			}
		}
		else if (result == ByteComparator::NEED_MORE_0)
		{
			const int m = (int)(ptr0 - &buff[0][0]);
			const int l = (int)(end0 - ptr0);
			//move uncompared data to begin of buff0
			memcpy( &buff[0][0], &buff[0][m], l );
			bfstart[0] = 0;
			bfstart[1] = ptr1 - orig1;
			bfend[0] = l;
		}
		else if (result == ByteComparator::NEED_MORE_1)
		{
			const int m = (int)(ptr1 - &buff[1][0]);
			const int l = (int)(end1 - ptr1);
			//move uncompared data to begin of buff1
			memcpy( &buff[1][0], &buff[1][m], l );
			bfstart[1]=0;
			bfstart[0]=ptr0 - orig0;
			bfend[1] = l;
		}
		else if (result == ByteComparator::NEED_MORE_BOTH)
		{
			if ((end0 == ptr0) && (end1 == ptr1))
			{
				bfstart[0] = ptr0 - orig0;
				bfend[0] = 0;
				bfstart[1] = ptr1 - orig1;
				bfend[1] = 0;
			}
			else
			{
				if (ptr0 < end0)
				{
					const int m = (int)(ptr0 - orig0);
					const int l = (int)(end0 - ptr0);
					//move uncompared data to begin of buff0
					memcpy( &buff[0][0], &buff[0][m], l );
					bfstart[0] = 0;
					bfend[0] = l;
				}
				if (ptr1 < end1)
				{
					const int m = (int)(ptr1 - orig1);
					const int l = (int)(end1 - ptr1);
					//move uncompared data to begin of buff1
					memcpy( &buff[1][0], &buff[1][ m], l );
					bfstart[1] = 0;
					bfend[1] = l;
				}
			}
		}

		// Did we finish both files?
		// We set the text/binary status only for fully compared files. Only
		// then the result is reliable.
		if (eof[0] && eof[1])
		{
			BOOL bBin0 = (m_textStats[0].nzeros>0);
			BOOL bBin1 = (m_textStats[1].nzeros>0);

			if (bBin0 && bBin1)
				diffcode |= DIFFCODE::BIN;
			else if (bBin0)
				diffcode |= DIFFCODE::BINSIDE1;
			else if (bBin1)
				diffcode |= DIFFCODE::BINSIDE2;
			else
				diffcode |= DIFFCODE::TEXT;

			// If either unfinished, they differ
			if (ptr0 != end0 || ptr1 != end1)
				diffcode = (diffcode & DIFFCODE::DIFF);
			if (diffcode & DIFFCODE::DIFF)
				return diffcode | DIFFCODE::DIFF;
			else
				return diffcode | DIFFCODE::SAME;
		}
	}
	return diffcode;
}

/**
 * @brief Copy text stat results from diffutils back into the FileTextStats structure
 */
static void CopyTextStats(const FileTextStats * stats, FileTextStats * myTextStats)
{
	myTextStats->ncrlfs = stats->ncrlfs;
	myTextStats->ncrs = stats->ncrs;
	myTextStats->nlfs = stats->nlfs;
}

/**
 * @brief Return text statistics for last compare.
 * @param [in] side For which file to return statistics.
 * @param [out] stats Stats as asked.
 */
void ByteCompare::GetTextStats(int side, FileTextStats *stats)
{
	CopyTextStats(&m_textStats[side], stats);
}

} // namespace CompareEngines
