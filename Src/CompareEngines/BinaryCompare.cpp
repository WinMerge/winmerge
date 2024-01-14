/**
 * @file  BinaryCompare.cpp
 *
 * @brief Implementation file for BinaryCompare
 */

#include "pch.h"
#include "BinaryCompare.h"
#include "DiffItem.h"
#include "PathContext.h"
#include "IAbortable.h"
#include "cio.h"

namespace CompareEngines
{

BinaryCompare::BinaryCompare() : m_piAbortable(nullptr)
{
}

BinaryCompare::~BinaryCompare() = default;

/**
 * @brief Set Abortable-interface.
 * @param [in] piAbortable Pointer to abortable interface.
 */
void BinaryCompare::SetAbortable(const IAbortable * piAbortable)
{
	m_piAbortable = const_cast<IAbortable*>(piAbortable);
}

static int compare_files(const String& file1, const String& file2, IAbortable *piAbortable)
{
	const size_t bufsize = 1024 * 256;
	int code;
	int fd1 = -1, fd2 = -1;
	
	cio::tsopen_s(&fd1, file1, O_BINARY | O_RDONLY, _SH_DENYNO, _S_IREAD);
	cio::tsopen_s(&fd2, file2, O_BINARY | O_RDONLY, _SH_DENYNO, _S_IREAD);
	if (fd1 != -1 && fd2 != -1)
	{
		for (;;)
		{
			if (piAbortable && piAbortable->ShouldAbort())
			{
				code = DIFFCODE::CMPABORT;
				break;
			}
			char buf1[bufsize];
			char buf2[bufsize];
			int size1 = cio::read_i(fd1, buf1, sizeof(buf1));
			int size2 = cio::read_i(fd2, buf2, sizeof(buf2));
			if (size1 <= 0 || size2 <= 0)
			{
				if (size1 < 0 || size2 < 0)
					code = DIFFCODE::CMPERR;
				else if (size1 == size2)
					code = DIFFCODE::SAME;
				else
					code = DIFFCODE::DIFF;
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
		cio::close(fd1);
	if (fd2 != -1)
		cio::close(fd2);

	return code;
}

/**
 * @brief Compare two specified files, byte-by-byte
 * @param [in] di Diffitem info.
 * @return DIFFCODE
 */
int BinaryCompare::CompareFiles(const PathContext& files, const DIFFITEM &di) const
{
	auto cmp = [&](int p1, int p2) -> unsigned
	{
		if (di.diffFileInfo[p1].size == DirItem::FILE_SIZE_NONE &&
			di.diffFileInfo[p2].size == DirItem::FILE_SIZE_NONE)
			return DIFFCODE::SAME;
		// If the file size is 0, don't immediately assume that there is a difference even if the files have different sizes, because of possible symlinks.
		if (di.diffFileInfo[p1].size == DirItem::FILE_SIZE_NONE ||
			di.diffFileInfo[p2].size == DirItem::FILE_SIZE_NONE ||
			(di.diffFileInfo[p1].size != di.diffFileInfo[p2].size &&
			 di.diffFileInfo[p1].size != 0 && di.diffFileInfo[p2].size != 0))
			return DIFFCODE::DIFF;
		return compare_files(files[p1], files[p2], m_piAbortable);
	};
	switch (files.GetSize())
	{
	case 2:
		return cmp(0, 1);
	case 3:
		unsigned code10 = cmp(1, 0);
		unsigned code12 = cmp(1, 2);
		unsigned code02 = DIFFCODE::SAME;
		if (code10 == DIFFCODE::SAME && code12 == DIFFCODE::SAME)
			return DIFFCODE::SAME;
		else if (code10 == DIFFCODE::SAME && code12 == DIFFCODE::DIFF)
			return DIFFCODE::DIFF | DIFFCODE::DIFF3RDONLY;
		else if (code10 == DIFFCODE::DIFF && code12 == DIFFCODE::SAME)
			return DIFFCODE::DIFF | DIFFCODE::DIFF1STONLY;
		else if (code10 == DIFFCODE::DIFF && code12 == DIFFCODE::DIFF)
		{
			code02 = cmp(0, 2);
			if (code02 == DIFFCODE::SAME)
				return DIFFCODE::DIFF | DIFFCODE::DIFF2NDONLY;
		}
		if (code10 == DIFFCODE::CMPERR || code12 == DIFFCODE::CMPERR || code02 == DIFFCODE::CMPERR)
			return DIFFCODE::CMPERR;
		return DIFFCODE::DIFF;
	}
	return DIFFCODE::CMPERR;
}

} // namespace CompareEngines
