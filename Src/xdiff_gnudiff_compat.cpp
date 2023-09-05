#include "pch.h"
#include "cio.h"
#include "CompareOptions.h"
extern "C" {
#include "../Externals/xdiff/xinclude.h"
}

static bool read_mmfile(int fd, mmfile_t& mmfile)
{
	cio::stat st;
	if (cio::fstat(fd, &st) == -1)
		return false;
	if (st.st_size < 0 || st.st_size > INT32_MAX)
		return false;
	size_t sz = static_cast<size_t>(st.st_size);
	mmfile.ptr = static_cast<char *>(malloc(sz ? sz : 1));
	if (sz && cio::read(fd, mmfile.ptr, sz) == -1) {
		return false;
	}
	mmfile.size = static_cast<long>(sz);
	return true;
}

unsigned long make_xdl_flags(const DiffutilsOptions& options)
{
	unsigned long xdl_flags = 0;
	switch (options.m_diffAlgorithm)
	{
	case DIFF_ALGORITHM_MINIMAL:
		xdl_flags |= XDF_NEED_MINIMAL;
		break;
	case DIFF_ALGORITHM_PATIENCE:
		xdl_flags |= XDF_PATIENCE_DIFF;
		break;
	case DIFF_ALGORITHM_HISTOGRAM:
		xdl_flags |= XDF_HISTOGRAM_DIFF;
		break;
	case DIFF_ALGORITHM_NONE:
		xdl_flags |= XDF_NONE_DIFF;
		break;
	default:
		break;
	}
	if (options.m_bIgnoreCase)
		xdl_flags |= XDF_IGNORE_CASE;
	if (options.m_bIgnoreNumbers)
		xdl_flags |= XDF_IGNORE_NUMBERS;
	if (options.m_bIgnoreBlankLines)
		xdl_flags |= XDF_IGNORE_BLANK_LINES;
	if (options.m_bIgnoreEOLDifference)
		xdl_flags |= XDF_IGNORE_CR_AT_EOL;
	switch (options.m_ignoreWhitespace)
	{
	case WHITESPACE_IGNORE_CHANGE:
		xdl_flags |= XDF_IGNORE_WHITESPACE_CHANGE;
		break;
	case WHITESPACE_IGNORE_ALL:
		xdl_flags |= XDF_IGNORE_WHITESPACE;
		break;
	default:
		break;
	}
	if (options.m_bIndentHeuristic)
		xdl_flags |= XDF_INDENT_HEURISTIC;
	return xdl_flags;
}

static int hunk_func(long start_a, long count_a, long start_b, long count_b, void *cb_data)
{
	return 0;
}

struct change * diff_2_files_xdiff (struct file_data filevec[], int* bin_status, int bMoved_blocks_flag, unsigned xdl_flags)
{
	mmfile_t mmfile1 = { 0 }, mmfile2 = { 0 };
	change *script = nullptr;
	xdfenv_t xe;
	xdchange_t *xscr;
	xpparam_t xpp = { 0 };
	xdemitconf_t xecfg = { 0 };
	xdemitcb_t ecb = { 0 };
	int bin_flag = 0;

	read_files(filevec, no_details_flag & ~ignore_some_changes, &bin_flag);

	mmfile1.ptr = const_cast<char*>(filevec[0].prefix_end);
	mmfile1.size = static_cast<long>(filevec[0].suffix_begin - filevec[0].prefix_end) - filevec[0].missing_newline;
	mmfile2.ptr = const_cast<char*>(filevec[1].prefix_end);
	mmfile2.size = static_cast<long>(filevec[1].suffix_begin - filevec[1].prefix_end) - filevec[1].missing_newline;

	xpp.flags = xdl_flags;
	xecfg.hunk_func = hunk_func;
	if (xdl_diff_modified(&mmfile1, &mmfile2, &xpp, &xecfg, &ecb, &xe, &xscr) == 0)
	{
		change *prev = nullptr;
		for (xdchange_t* xcur = xscr; xcur; xcur = xcur->next)
		{
			change* e = static_cast<change*>(malloc(sizeof(change)));
			if (!e)
				goto abort;
			if (!script)
				script = e;
			e->line0 = xcur->i1;
			e->line1 = xcur->i2;
			e->deleted = xcur->chg1;
			e->inserted = xcur->chg2;
			e->match0 = -1;
			e->match1 = -1;
			e->trivial = static_cast<char>(xcur->ignore);
			e->link = nullptr;
			e->ignore = 0;
			if (prev)
				prev->link = e;
			prev = e;
		}

		if (bMoved_blocks_flag)
			moved_block_analysis(&script, filevec);

		xdl_free_script(xscr);
		xdl_free_env(&xe);
	}

	return script;

abort:
	return nullptr;
}
