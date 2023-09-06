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

struct change * diff_2_files_xdiff (struct file_data filevec[], int* bin_status, int bMoved_blocks_flag, int* bin_file, unsigned xdl_flags)
{
	mmfile_t mmfile1 = { 0 }, mmfile2 = { 0 };
	change *script = nullptr;
	xdfenv_t xe;
	xdchange_t *xscr;
	xpparam_t xpp = { 0 };
	xdemitconf_t xecfg = { 0 };
	xdemitcb_t ecb = { 0 };

	if (read_files(filevec, no_details_flag & ~ignore_some_changes, bin_file))
	{
		int i;
		int changes;
		// copy from analyze.c
		//  We can now safely assume to have a pair of Binary files.

		// Are both files Open and Regular (no Pipes, Directories, Devices (e.g. NUL))
		if (filevec[0].desc < 0 || filevec[1].desc < 0 ||
			!(S_ISREG (filevec[0].stat.st_mode)) || !(S_ISREG (filevec[1].stat.st_mode))   )
			changes = 1;
		else
		//  Files with different lengths must be different.  
		if (filevec[0].stat.st_size != filevec[1].stat.st_size)
			changes = 1;
		else
		//  Identical descriptor implies identical files
		if (filevec[0].desc == filevec[1].desc)
			changes = 0;
		//  Scan both files, a buffer at a time, looking for a difference.  
		else
		{
			//  Same-sized buffers for both files were allocated in read_files().  
			size_t buffer_size = filevec[0].bufsize;
			
			for (;;)
			{
				//  Read a buffer's worth from both files.  
				for (i = 0; i < 2; i++)
					while (filevec[i].buffered_chars < buffer_size)
					  {
						int r = _read (filevec[i].desc,
									   filevec[i].buffer	+ filevec[i].buffered_chars,
									   (int)(buffer_size - filevec[i].buffered_chars));
						if (r == 0)
							break;
						if (r < 0)
							pfatal_with_name (filevec[i].name);
						filevec[i].buffered_chars += r;
					  }
						
				//  If the buffers have different number of chars, the files differ.  
				if (filevec[0].buffered_chars != filevec[1].buffered_chars)
				{
					changes = 1;
					break;
				}

				//  If we reach end-of-file, the files are the same.  
				if (filevec[0].buffered_chars==0) // therefore: filevec[1].buffered_chars==0
				{
					changes = 0;
					break;
				}	

				//	If buffers have different contents, the files are different.
				if (memcmp (filevec[0].buffer,
							filevec[1].buffer,
							filevec[0].buffered_chars) != 0)
				{
					changes = 1;
					break;
				}

				//	Files appear identical so far...
				//	Prepare to loop again for the next pair of buffers.
				filevec[0].buffered_chars = filevec[1].buffered_chars = 0;
			}
		}
		if (bin_status != NULL)
			*bin_status = (changes != 0 ? -1 : 1);
	}
	else
	{
		mmfile1.ptr = const_cast<char*>(filevec[0].prefix_end);
		mmfile1.size = static_cast<long>(filevec[0].suffix_begin - filevec[0].prefix_end);
		mmfile2.ptr = const_cast<char*>(filevec[1].prefix_end);
		mmfile2.size = static_cast<long>(filevec[1].suffix_begin - filevec[1].prefix_end);

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
	}

	return script;

abort:
	return nullptr;
}
