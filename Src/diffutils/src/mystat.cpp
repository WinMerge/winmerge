// my own _fstat() and _wstat() implementation for the bug https://connect.microsoft.com/VisualStudio/feedback/details/1600505/stat-not-working-on-windows-xp-using-v14-xp-platform-toolset-vs2015
#include "pch.h"
#include <sys/stat.h>
#include <io.h>
#include <cerrno>
#include <windows.h>

inline time_t filetime_to_time_t(const FILETIME& ft)
{
	if (ft.dwHighDateTime == 0 && ft.dwLowDateTime == 0)
		return 0;
	else
		return ((static_cast<time_t>(ft.dwHighDateTime) << 32) + ft.dwLowDateTime) / 10000000ULL - 11644473600ULL;
}

template<typename FileInfo>
inline void set_statbuf(const FileInfo& hfi, struct _stat64& buf)
{
	buf.st_size = (static_cast<__int64>(hfi.nFileSizeHigh) << 32) | hfi.nFileSizeLow;
	buf.st_atime = filetime_to_time_t(hfi.ftLastAccessTime);
	buf.st_mtime = filetime_to_time_t(hfi.ftLastWriteTime);
	buf.st_ctime = filetime_to_time_t(hfi.ftCreationTime);
	buf.st_mode = 
		((hfi.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? _S_IFDIR : _S_IFREG) |
		((hfi.dwFileAttributes & FILE_ATTRIBUTE_READONLY ) ? 
			 (_S_IREAD              | ( _S_IREAD              >> 3) | ( _S_IREAD              >> 6)) : 
			((_S_IREAD | _S_IWRITE) | ((_S_IREAD | _S_IWRITE) >> 3) | ((_S_IREAD | _S_IWRITE) >> 6)));
}

extern "C" int myfstat(int fd, struct _stat64 *buf)
{
	if (buf == nullptr)
	{
		errno = EINVAL;
		return -1;
	}
	HANDLE hFile = reinterpret_cast<HANDLE>(_get_osfhandle(fd));
	if (hFile == INVALID_HANDLE_VALUE)
	{
		errno = EBADF;
		return -1;
	}
	memset(buf, 0, sizeof(*buf));
	switch (GetFileType(hFile) & ~FILE_TYPE_REMOTE)
	{
	case FILE_TYPE_CHAR:
		buf->st_mode = _S_IFCHR;
		return 0;
	case FILE_TYPE_PIPE:
		buf->st_mode = _S_IFIFO;
		DWORD nBufferSize;
		if (PeekNamedPipe(hFile, nullptr, 0, nullptr, &nBufferSize, nullptr))
			buf->st_size = nBufferSize;
		return 0;
	case FILE_TYPE_DISK:
		BY_HANDLE_FILE_INFORMATION hfi;
		if (!GetFileInformationByHandle(hFile, &hfi))
		{
			errno = EBADF;
			return -1;
		}
		set_statbuf(hfi, *buf);
		return 0;
	default:
		errno = EBADF;
		return -1;
	}
}

extern "C" int mywstat(const wchar_t *filename, struct _stat64 *buf)
{
	if (buf == nullptr)
	{
		errno = EINVAL;
		return -1;
	}
	if (wcspbrk(filename, L"*?") != nullptr)
	{
		errno = ENOENT;
		return -1;
	}
	WIN32_FIND_DATAW ffd;
	HANDLE hFindFile = FindFirstFileW(filename, &ffd);
	if (hFindFile == INVALID_HANDLE_VALUE)
	{
		errno = ENOENT;
		return -1;
	}
	FindClose(hFindFile);
	memset(buf, 0, sizeof(*buf));
	set_statbuf(ffd, *buf);
	return 0;
}
