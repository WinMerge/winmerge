#include "pch.h"
#include <gtest/gtest.h>
#include "charsets.h"

namespace
{
	// The fixture for testing paths functions.
	class CharsetsTest : public testing::Test
	{
	protected:
		// You can remove any or all of the following functions if its body
		// is	empty.

		CharsetsTest()
		{
			// You can do set-up work for each test	here.
			charsets_init();
		}

		virtual ~CharsetsTest()
		{
			// You can do clean-up work	that doesn't throw exceptions here.
			charsets_cleanup();
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

	TEST_F(CharsetsTest, name_to_codepage)
	{
		EXPECT_EQ(65001, GetEncodingCodePageFromId(GetEncodingIdFromName("UTF-8")));
		EXPECT_EQ(65001, GetEncodingCodePageFromName("utf-8"));
		EXPECT_EQ(0, GetEncodingIdFromName("abc"));
		EXPECT_EQ(0, GetEncodingCodePageFromId(GetEncodingIdFromName("abc")));
		EXPECT_EQ(0, GetEncodingCodePageFromName("abc"));
	}

	TEST_F(CharsetsTest, codepage_to_name)
	{
		EXPECT_STREQ("utf-8", GetEncodingNameFromId(GetEncodingIdFromCodePage(65001)));
		EXPECT_STREQ("utf-8", GetEncodingNameFromCodePage(65001));
		EXPECT_EQ(0, GetEncodingNameFromId(GetEncodingIdFromCodePage(99999)));
		EXPECT_EQ(NULL, GetEncodingNameFromCodePage(99999));
	}


}  // namespace
