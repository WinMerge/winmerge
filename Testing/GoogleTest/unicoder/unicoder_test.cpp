#include <gtest/gtest.h>
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
		wchar_t str[] = {'A','B','C', 0xd867, 0xde3d, 0};
		std::string utf8 = ucr::toUTF8(str);
		EXPECT_STREQ("ABC\xf0\xa9\xb8\xbd", utf8.c_str());

		EXPECT_EQ(false, ucr::CheckForInvalidUtf8(utf8.c_str(), utf8.length()));
	}


}  // namespace
