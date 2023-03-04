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

static void append_equivs(const xdfile_t& xdf, struct file_data& filevec, std::vector<xrecord_t *>& equivs, unsigned xdl_flags)
{
	std::unordered_map<unsigned long, std::vector<int>> equivs_map;
	for (int i = 0; i < static_cast<int>(equivs.size()); ++i)
	{
		unsigned long ha = equivs[i]->ha;
		if (equivs_map.find(ha) != equivs_map.end())
			equivs_map[ha].push_back(i);
		else
			equivs_map.emplace(ha, std::vector<int>{i});
	}

	for (int i = 0; i < xdf.nrec; ++i)
	{
		unsigned long ha = xdf.recs[i]->ha;
		if (equivs_map.find(ha) != equivs_map.end())
		{
			bool found = false;
			for (auto j: equivs_map[ha])
			{
				if (xdl_recmatch(equivs[j]->ptr, equivs[j]->size, xdf.recs[i]->ptr, xdf.recs[i]->size, xdl_flags))
				{
					found = true;
					filevec.equivs[i] = j;
					equivs_map.emplace(ha, std::vector<int>{j});
					break;
				}
			}
			if (!found)
			{
				filevec.equivs[i] = static_cast<int>(equivs.size());
				equivs_map[ha].push_back(filevec.equivs[i]);
				equivs.push_back(xdf.recs[i]);
			}
		}
		else
		{
			filevec.equivs[i] = static_cast<int>(equivs.size());
			equivs_map.emplace(ha, std::vector<int>{filevec.equivs[i]});
			equivs.push_back(xdf.recs[i]);
		}
	}
}

static int is_missing_newline(const mmfile_t& mmfile)
{
	if (mmfile.size == 0 || mmfile.ptr[mmfile.size - 1] == '\r' || mmfile.ptr[mmfile.size - 1] == '\n')
		return 0;
	return 1;
}

struct change * diff_2_files_xdiff (struct file_data filevec[], int bMoved_blocks_flag, unsigned xdl_flags)
{
	mmfile_t mmfile1 = { 0 }, mmfile2 = { 0 };
	change *script = nullptr;
	xdfenv_t xe;
	xdchange_t *xscr;
	xpparam_t xpp = { 0 };
	xdemitconf_t xecfg = { 0 };
	xdemitcb_t ecb = { 0 };

	if (!read_mmfile(filevec[0].desc, mmfile1))
		goto abort;
	if (!read_mmfile(filevec[1].desc, mmfile2))
		goto abort;

	xpp.flags = xdl_flags;
	xecfg.hunk_func = hunk_func;
	if (xdl_diff_modified(&mmfile1, &mmfile2, &xpp, &xecfg, &ecb, &xe, &xscr) == 0)
	{
		filevec[0].buffer = mmfile1.ptr;
		filevec[1].buffer = mmfile2.ptr;
		filevec[0].bufsize = mmfile1.size;
		filevec[1].bufsize = mmfile2.size;
		filevec[0].buffered_chars = mmfile1.size;
		filevec[1].buffered_chars = mmfile2.size;
		filevec[0].linbuf_base = 0;
		filevec[1].linbuf_base = 0;
		filevec[0].valid_lines = xe.xdf1.nrec;
		filevec[1].valid_lines = xe.xdf2.nrec;
		filevec[0].linbuf = static_cast<const char **>(malloc(sizeof(char *) * (xe.xdf1.nrec + 1)));
		if (!filevec[0].linbuf)
			goto abort;
		filevec[1].linbuf = static_cast<const char **>(malloc(sizeof(char *) * (xe.xdf2.nrec + 1)));
		if (!filevec[1].linbuf)
			goto abort;
		filevec[0].equivs = static_cast<int *>(malloc(sizeof(int) * xe.xdf1.nrec));
		if (!filevec[0].equivs)
			goto abort;
		filevec[1].equivs = static_cast<int *>(malloc(sizeof(int) * xe.xdf2.nrec));
		if (!filevec[1].equivs)
			goto abort;
		for (int i = 0; i < xe.xdf1.nrec; ++i)
		{
			filevec[0].linbuf[i] = xe.xdf1.recs[i]->ptr;
			filevec[0].equivs[i] = -1;
		}
		if (xe.xdf1.nrec > 0)
			filevec[0].linbuf[xe.xdf1.nrec] = xe.xdf1.recs[xe.xdf1.nrec - 1]->ptr + xe.xdf1.recs[xe.xdf1.nrec - 1]->size;
		for (int i = 0; i < xe.xdf2.nrec; ++i)
		{
			filevec[1].linbuf[i] = xe.xdf2.recs[i]->ptr;
			filevec[1].equivs[i] = -1;
		}
		if (xe.xdf2.nrec > 0)
			filevec[1].linbuf[xe.xdf2.nrec] = xe.xdf2.recs[xe.xdf2.nrec - 1]->ptr + xe.xdf2.recs[xe.xdf2.nrec - 1]->size;
		filevec[0].missing_newline = is_missing_newline(mmfile1);
		filevec[1].missing_newline = is_missing_newline(mmfile2);

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
		{
			std::vector<xrecord_t *> equivs;
			append_equivs(xe.xdf1, filevec[0], equivs, xdl_flags);
			append_equivs(xe.xdf2, filevec[1], equivs, xdl_flags);
			moved_block_analysis(&script, filevec);
		}

		xdl_free_script(xscr);
		xdl_free_env(&xe);
	}

	return script;

abort:
	free(mmfile1.ptr);
	free(mmfile2.ptr);
	return nullptr;
}
