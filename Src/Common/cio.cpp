#include "pch.h"
#include "cio.h"
#include "TFile.h"
#include <cerrno>

namespace cio
{
#ifdef _WIN32

int tsopen_s(int* fd, const String& filepath, int oflag, int shflag, int pmode)
{
	return _wsopen_s(fd, TFile(filepath).wpath().c_str(), oflag, shflag, pmode);
}

int tfopen_s(FILE** fp, const String& filepath, const String::value_type* mode)
{
	return _wfopen_s(fp, TFile(filepath).wpath().c_str(), mode);
}

ssize_t read(int fd, void* buf, size_t size)
{
	ssize_t pos = 0;
	while (pos < size)
	{
		unsigned rsize = (size - pos < 0x40000000) ? static_cast<unsigned>(size - pos) : 0x40000000;
		int res32 = _read(fd, reinterpret_cast<char *>(buf) + pos, rsize);
		if (res32 == -1)
			return -1;
		pos += res32;
		if (res32 < rsize)
			break;
	}
	return pos;
}

ssize_t write(int fd, const void* buf, size_t size)
{
	ssize_t pos = 0;
	while (pos < size)
	{
		unsigned wsize = (size - pos < 0x40000000) ? static_cast<unsigned>(size - pos) : 0x40000000;
		int res32 = _write(fd, reinterpret_cast<const char *>(buf) + pos, wsize);
		if (res32 == -1)
			return -1;
		pos += res32;
	}
	return pos;
}

#else

int tsopen_s(int* fd, const String& filepath, int oflag, int shflag, int pmode)
{
	*fd = open(filepath.c_str(), oflag, pmode);
	return errno;
}

int fopen_s(FILE** fp, const String& filepath, const String::value_type* mode)
{
	*fp = fopen(filepath.c_str(), mode);
	return errno;
}

#endif
}