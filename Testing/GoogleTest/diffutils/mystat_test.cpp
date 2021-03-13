#include "pch.h"
#include <gtest/gtest.h>
#include <sys/stat.h>
#include "Environment.h"
#include "paths.h"
#include "diff.h"

TEST(diffutils, stat)
{
	struct _stat64 buf, buf2, buf3, buf4, buf5;
	String filename  = paths::ConcatPath(env::GetProgPath(), _T("../TestData/_tmp_test.txt"));
	String filename2 = paths::ConcatPath(env::GetProgPath(), _T("../TestData/LeftAndRight.WinMerge"));
	int fd;
	EXPECT_EQ(0, _tsopen_s(&fd, filename.c_str(), _O_RDWR | _O_CREAT, _SH_DENYNO, _S_IREAD | _S_IWRITE));
	_write(fd, "0123456789", 10);
	time_t t = time(&t);

	EXPECT_EQ(0, myfstat(fd, &buf));
	EXPECT_EQ(buf.st_size, 10);
	EXPECT_EQ(t, buf.st_mtime);
	EXPECT_NE(0, buf.st_mode & _S_IFREG);
	EXPECT_NE(0, buf.st_mode & _S_IREAD);
	EXPECT_NE(0, buf.st_mode & _S_IWRITE);
	_close(fd);

	mywstat(filename.c_str(), &buf2);
	EXPECT_EQ(buf2.st_size, 10);
	EXPECT_EQ(t, buf2.st_mtime);
	EXPECT_NE(0, buf2.st_mode & _S_IFREG);
	EXPECT_NE(0, buf2.st_mode & _S_IREAD);
	EXPECT_NE(0, buf2.st_mode & _S_IWRITE);
	_tremove(filename.c_str());

	int fds[2];
	char data[256];
	EXPECT_EQ(0, _pipe(fds, 256, O_BINARY));
	_write(fds[1], "0123456789", 10);
	EXPECT_EQ(0, myfstat(fds[0], &buf3));
	EXPECT_EQ(10, buf3.st_size);
	EXPECT_NE(0, buf3.st_mode & _S_IFIFO);
	_read(fds[0], data, sizeof(data));
	_close(fds[0]);
	_close(fds[1]);

	_tsopen_s(&fd, _T("CON"), _O_TEXT, _SH_DENYNO, _S_IWRITE);
	EXPECT_EQ(0, myfstat(fd, &buf4));
	EXPECT_NE(0, buf4.st_mode & _S_IFCHR);
	_close(fd);

	_tsopen_s(&fd, _T("NUL"), _O_TEXT, _SH_DENYNO, _S_IWRITE);
	EXPECT_EQ(0, myfstat(fd, &buf5));
	EXPECT_NE(0, buf5.st_mode & _S_IFCHR);
	_close(fd);

	//EXPECT_EQ(-1, myfstat(999, &buf));
	EXPECT_EQ(-1, myfstat(1, NULL));
	EXPECT_EQ(-1, mywstat(L"aaaa", NULL));
	EXPECT_EQ(-1, mywstat(L"aaaa", &buf2));
	EXPECT_EQ(-1, mywstat(filename2.c_str(), NULL));

	EXPECT_EQ(0, mywstat(L"CON", &buf2));
	
}
