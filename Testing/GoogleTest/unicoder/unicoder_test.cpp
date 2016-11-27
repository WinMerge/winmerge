#include <gtest/gtest.h>
#include "codepage.h"
#include "unicoder.h"

namespace
{
	// The fixture for testing paths functions.
	class UnicoderTest : public testing::Test
	{
	protected:
		// You can remove any or all of the following functions if its body
		// is	empty.

		UnicoderTest()
		{
			// You can do set-up work for each test	here.
		}

		virtual ~UnicoderTest()
		{
			// You can do clean-up work	that doesn't throw exceptions here.
		}

		// If	the	constructor	and	destructor are not enough for setting up
		// and cleaning up each test, you can define the following methods:

		virtual void SetUp()
		{
			// Code	here will be called	immediately	after the constructor (right
			// before each test).
		}

		virtual void TearDown()
		{
			// Code	here will be called	immediately	after each test	(right
			// before the destructor).
		}

		// Objects declared here can be used by all tests in the test case for Foo.
	};

	TEST_F(UnicoderTest, toUTF8)
	{
#ifdef _UNICODE
		wchar_t str[] = {'A','B','C', 0xd867, 0xde3d, 0};
		std::string utf8 = ucr::toUTF8(str);
		EXPECT_STREQ("ABC\xf0\xa9\xb8\xbd", utf8.c_str());

		EXPECT_EQ(false, ucr::CheckForInvalidUtf8(utf8.c_str(), utf8.length()));
#endif
	}

	TEST_F(UnicoderTest, CrossConvert)
	{
		wchar_t wbuf[256];
		char buf[256];
		wchar_t str_ucs2[] = {'A','B','C', 0xd867, 0xde3d, 0};
		wchar_t str_ucs2be[256];
		const char *str_utf8 = "ABC\xf0\xa9\xb8\xbd";
		bool lossy;
		int n;

		// UTF8->UCS2LE
		n = ucr::CrossConvert(str_utf8, strlen(str_utf8), (char *)wbuf, sizeof(wbuf), CP_UTF8, 1200, &lossy);
		EXPECT_STREQ(str_ucs2, wbuf);
		EXPECT_EQ(wcslen(str_ucs2) * sizeof(wchar_t), n);

		// UCS2BE->UCS2LE
		_swab((char *)str_ucs2, (char *)str_ucs2be, (wcslen(str_ucs2) + 1) * sizeof(wchar_t));
		n = ucr::CrossConvert((char *)str_ucs2be, wcslen(str_ucs2) * sizeof(wchar_t), (char *)wbuf, sizeof(wbuf), 1201, 1200, &lossy);
		EXPECT_STREQ(str_ucs2, wbuf);
		EXPECT_EQ(wcslen(str_ucs2) * sizeof(wchar_t), n);

		// UCS2LE->UCS2LE
		n = ucr::CrossConvert((char *)str_ucs2, wcslen(str_ucs2) * sizeof(wchar_t), (char *)wbuf, sizeof(wbuf), 1200, 1200, &lossy);
		EXPECT_STREQ(str_ucs2, wbuf);
		EXPECT_EQ(wcslen(str_ucs2) * sizeof(wchar_t), n);

		// UTF8->UCS2BE
		n = ucr::CrossConvert(str_utf8, strlen(str_utf8), (char *)wbuf, sizeof(wbuf), CP_UTF8, 1201, &lossy);
		EXPECT_STREQ(str_ucs2be, wbuf);
		EXPECT_EQ(wcslen(str_ucs2be) * sizeof(wchar_t), n);

		// UCS2BE->UTF8
		n = ucr::CrossConvert((char *)str_ucs2be, wcslen(str_ucs2be) * sizeof(wchar_t), buf, sizeof(buf), 1201, CP_UTF8, &lossy);
		EXPECT_STREQ(str_utf8, buf);
		EXPECT_EQ(strlen(str_utf8), n);

	}

}  // namespace
