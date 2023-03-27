#pragma once

#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#ifdef _WIN32
#include <io.h>
extern "C" int myfstat(int fd, struct _stat64 *buf);
#else
#include <unistd.h>
#endif
#include "UnicodeString.h"

namespace cio
{
	int tsopen_s(int* fd, const String& filepath, int oflag, int shflag, int pmode);
	int tfopen_s(FILE** fp, const String& filepath, const String::value_type* mode);
#ifdef _WIN32
	typedef intptr_t ssize_t;
	typedef struct _stat64 stat;
	constexpr auto read_i = _read;
	constexpr auto write_i = _write;
	ssize_t read(int fd, void* buf, size_t size);
	ssize_t write(int fd, const void* buf, size_t size);
	constexpr auto close = ::_close;
	constexpr auto fstat = ::myfstat;
	constexpr auto pipe = ::_pipe;
#else
#define O_BINARY (0)
#define _SH_DENYNO (0)
#define _S_IREAD  (S_IRUSR | S_IRGRP | S_IROTH)
#define _S_IWRITE (S_IWUSR | S_IWGRP | S_IWOTH)
	typedef struct stat stat;
	inline int read_i(int fd, void* buf, unsigned size) { return (int)::read(fd, buf, size); }
	inline int write_i(int fd, const void* buf, unsigned size) { return (int)::write(fd, buf, size); }
	constexpr auto read = ::read;
	constexpr auto write = ::write;
	constexpr auto close = ::close;
	constexpr auto fstat = ::fstat;
	constexpr auto pipe = ::pipe;
#endif
}
