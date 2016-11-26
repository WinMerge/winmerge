/**
 * @file  BinaryCompare.cpp
 *
 * @brief Implementation file for BinaryCompare
 */

#include "BinaryCompare.h"
#include "DiffItem.h"
#include "PathContext.h"
#ifdef _WIN32
# include <io.h>
#else
# include <unistd.h>
#endif
#include <fcntl.h>

namespace CompareEngines
{

BinaryCompare::BinaryCompare()
{
}

BinaryCompare::~BinaryCompare()
{
}

static int compare_files(const String& file1, const String& file2)
{
	const size_t bufsize = 1024 * 256;
	int code;
	int fd1 = _topen(file1.c_str(), O_BINARY | O_RDONLY);
	int fd2 = _topen(file2.c_str(), O_BINARY | O_RDONLY);
	if (fd1 != -1 && fd2 != -1)
	{
		for (;;)
		{
			char buf1[bufsize];
			char buf2[bufsize];
			int size1 = read(fd1, buf1, sizeof(buf1));
			int size2 = read(fd2, buf2, sizeof(buf2));
			if (size1 <= 0 || size2 <= 0)
			{
				if (size1 < 0 || size2 < 0)
					code = DIFFCODE::CMPERR;
				else
					code = DIFFCODE::SAME;
				break;
			}
			if (size1 != size2 || memcmp(buf1, buf2, size1) != 0)
			{
				code = DIFFCODE::DIFF;
				break;
			}
		}
	}
	else
	{
		code = DIFFCODE::CMPERR;
	}
	if (fd1 != -1)
		close(fd1);
	if (fd2 != -1)
		close(fd2);

	return code;
}

/**
 * @brief Compare two specified files, byte-by-byte
 * @param [in] di Diffitem info.
 * @return DIFFCODE
 */
int BinaryCompare::CompareFiles(const PathContext& files, const DIFFITEM &di) const
{
	unsigned code = DIFFCODE::DIFF;
	if (files.GetSize() == 2 && di.diffFileInfo[0].size == di.diffFileInfo[1].size)
	{
		code = compare_files(files[0], files[1]);
	}
	else if (files.GetSize() == 3 && 
		di.diffFileInfo[0].size == di.diffFileInfo[1].size &&
		di.diffFileInfo[1].size == di.diffFileInfo[2].size)
	{
		code = compare_files(files[0], files[1]);
		if (code == DIFFCODE::SAME)
			code = compare_files(files[1], files[2]);
	}
	return code;
}

} // namespace CompareEngines
