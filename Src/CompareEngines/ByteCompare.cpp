/**
 * @file  ByteCompare.cpp
 *
 * @brief Implementation file for ByteCompare
 */

#include "pch.h"
#include "ByteCompare.h"
#include <cassert>
#include "cio.h"
#include "FileLocation.h"
#include "UnicodeString.h"
#include "IAbortable.h"
#include "CompareOptions.h"
#include "DiffContext.h"
#include "diff.h"
#include "ByteComparator.h"

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
		: m_pOptions(nullptr)
		, m_piAbortable(nullptr)
		, m_inf(nullptr)
{
}

/**
 * @brief Default destructor.
 */
ByteCompare::~ByteCompare() = default;

/**
 * @brief Set compare options from general compare options.
 * @param [in ]options General compare options.
 */
void ByteCompare::SetCompareOptions(const CompareOptions & options)
{
	m_pOptions.reset(new QuickCompareOptions(options));
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
	assert(items == 2);
	m_inf = data;
	m_textStats[0].clear();
	m_textStats[1].clear();
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
	unsigned diffcode = 0;

	// area of buffer currently holding data
	int64_t bfstart[2]; // offset into buff[i] where current data resides
	int64_t bfend[2]; // past-the-end pointer into buff[i], giving end of current data
	// buff[0] has bytes to process from buff[0][bfstart[0]] to buff[0][bfend[0]-1]

	bool eof[2]; // if we've finished file

	// initialize our buffer pointers and end of file flags
	for (i = 0; i < 2; ++i)
	{
		bfstart[i] = bfend[i] = 0;
		eof[i] = false;
	}

	ByteComparator comparator(m_pOptions.get());

	// Begin loop
	// we handle the files in WMCMPBUFF sized buffers (variable buff[][])
	// That is, we do one buffer full at a time
	// or even less, as we process until one side buffer is empty, then reload that one
	// and continue
	while (!eof[0] || !eof[1])
	{
		if (m_piAbortable != nullptr && m_piAbortable->ShouldAbort())
			return DIFFCODE::CMPABORT;

		// load or update buffers as appropriate
		for (i = 0; i < 2; ++i)
		{
			if (!eof[i] && bfstart[i] == sizeof(buff[i])/sizeof(buff[i][0]))
			{
				bfstart[i] = bfend[i] = 0;
			}
			if (!eof[i] && bfend[i] < sizeof(buff[i])/sizeof(buff[i][0]) - 1)
			{
				// Assume our blocks are in range of int
				int space = sizeof(buff[i])/sizeof(buff[i][0]) - (int) bfend[i];
				int rtn = cio::read_i(m_inf[i].desc, &buff[i][bfend[i]], space);
				if (rtn == -1)
					return DIFFCODE::CMPERR;
				if (rtn < space)
					eof[i] = true;
				bfend[i] += rtn;
				if (m_inf[0].desc == m_inf[1].desc)
				{
					bfstart[1] = bfstart[0];
					bfend[1] = bfend[0];
					eof[1] = eof[0];
					location[1] = location[0];
					memcpy(&buff[1][bfend[1] - rtn], &buff[0][bfend[0] - rtn], rtn);
					break;
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

		int64_t offset0 = (ptr0 - &buff[0][0]);
		int64_t offset1 = (ptr1 - &buff[1][0]);

		// are these two buffers the same?
		int result = comparator.CompareBuffers(m_textStats[0], m_textStats[1],
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
				assert(ptr0 >= orig0);
				assert(ptr1 >= orig1);
				bfstart[0] += ptr0 - orig0;
				bfstart[1] += ptr1 - orig1;
			}
		}
		else if (result == ByteComparator::NEED_MORE_0)
		{
			const int m = (int)(ptr0 - &buff[0][0]);
			const int l = (int)(end0 - ptr0);
			//move uncompared data to begin of buff0
			memcpy(&buff[0][0], &buff[0][m], l);
			bfstart[0] = 0;
			bfstart[1] += ptr1 - orig1;
			bfend[0] = l;
		}
		else if (result == ByteComparator::NEED_MORE_1)
		{
			const int m = (int)(ptr1 - &buff[1][0]);
			const int l = (int)(end1 - ptr1);
			//move uncompared data to begin of buff1
			memcpy(&buff[1][0], &buff[1][m], l);
			bfstart[1] = 0;
			bfstart[0] += ptr0 - orig0;
			bfend[1] = l;
		}
		else if (result == ByteComparator::NEED_MORE_BOTH)
		{
			if ((end0 == ptr0) && (end1 == ptr1))
			{
				bfstart[0] += ptr0 - orig0;
				bfend[0] = 0;
				bfstart[1] += ptr1 - orig1;
				bfend[1] = 0;
			}
			else
			{
				if (ptr0 < end0)
				{
					const int m = (int)(ptr0 - orig0);
					const int l = (int)(end0 - ptr0);
					//move uncompared data to begin of buff0
					memcpy(&buff[0][0], &buff[0][m], l);
					bfstart[0] = 0;
					bfend[0] += l;
				}
				if (ptr1 < end1)
				{
					const int m = (int)(ptr1 - orig1);
					const int l = (int)(end1 - ptr1);
					//move uncompared data to begin of buff1
					memcpy(&buff[1][0], &buff[1][ m], l);
					bfstart[1] = 0;
					bfend[1] += l;
				}
			}
		}
		else
		{
			assert(result == ByteComparator::RESULT_SAME);
		}

		// Did we finish both files?
		// We set the text/binary status only for fully compared files. Only
		// then the result is reliable.
		if (eof[0] && eof[1])
		{
			bool bBin0 = (m_textStats[0].nzeros > 0);
			bool bBin1 = (m_textStats[1].nzeros > 0);

			if (bBin0 && bBin1)
				diffcode |= DIFFCODE::BIN | DIFFCODE::BINSIDE1 | DIFFCODE::BINSIDE2;
			else if (bBin0)
				diffcode |= DIFFCODE::BIN | DIFFCODE::BINSIDE1;
			else if (bBin1)
				diffcode |= DIFFCODE::BIN | DIFFCODE::BINSIDE2;
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
	myTextStats->nzeros = stats->nzeros;
}

/**
 * @brief Return text statistics for last compare.
 * @param [in] side For which file to return statistics.
 * @param [out] stats Stats as asked.
 */
void ByteCompare::GetTextStats(int side, FileTextStats *stats) const
{
	CopyTextStats(&m_textStats[side], stats);
}

} // namespace CompareEngines
