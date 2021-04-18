#include "pch.h"
#include <gtest/gtest.h>
#include "UnicodeString.h"
#include "DiffItem.h"

namespace
{
	// The fixture for testing paths functions.
	class DiffCodeTest : public testing::Test
	{
	protected:
		// You can remove any or all of the following functions if its body
		// is	empty.

		DiffCodeTest()
		{
			// You can do set-up work for each test	here.
		}

		virtual ~DiffCodeTest()
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

	TEST_F(DiffCodeTest, Construct1)
	{
		DIFFCODE dc(0);
		EXPECT_TRUE(dc.diffcode == 0);
	}

	TEST_F(DiffCodeTest, swap2wayBinside)
	{
		DIFFCODE dc(DIFFCODE::BIN | DIFFCODE::BINSIDE1);
		dc.swap(0, 1);
		EXPECT_TRUE(dc.diffcode == (DIFFCODE::BIN | DIFFCODE::BINSIDE2));
		dc.swap(0, 1);
		EXPECT_TRUE(dc.diffcode == (DIFFCODE::BIN | DIFFCODE::BINSIDE1));
	}

	TEST_F(DiffCodeTest, swap3wayBinside)
	{
		DIFFCODE dc(DIFFCODE::BIN | DIFFCODE::BINSIDE1 | DIFFCODE::BINSIDE3);
		dc.swap(0, 1);
		EXPECT_TRUE(dc.diffcode == (DIFFCODE::BIN | DIFFCODE::BINSIDE2 | DIFFCODE::BINSIDE3));
		dc.swap(0, 2);
		EXPECT_TRUE(dc.diffcode == (DIFFCODE::BIN | DIFFCODE::BINSIDE2 | DIFFCODE::BINSIDE1));
		dc.swap(1, 2);
		EXPECT_TRUE(dc.diffcode == (DIFFCODE::BIN | DIFFCODE::BINSIDE3 | DIFFCODE::BINSIDE1));
	}

	TEST_F(DiffCodeTest, swap3wayCompareFlags)
	{
		DIFFCODE dc(DIFFCODE::THREEWAY | DIFFCODE::DIFF1STONLY);
		dc.swap(1, 2);
		EXPECT_TRUE(dc.diffcode == (DIFFCODE::THREEWAY | DIFFCODE::DIFF1STONLY));
		dc.swap(0, 1);
		EXPECT_TRUE(dc.diffcode == (DIFFCODE::THREEWAY | DIFFCODE::DIFF2NDONLY));
		dc.swap(0, 2);
		EXPECT_TRUE(dc.diffcode == (DIFFCODE::THREEWAY | DIFFCODE::DIFF2NDONLY));
		dc.swap(1, 2);
		EXPECT_TRUE(dc.diffcode == (DIFFCODE::THREEWAY | DIFFCODE::DIFF3RDONLY));
		dc.swap(0, 1);
		EXPECT_TRUE(dc.diffcode == (DIFFCODE::THREEWAY | DIFFCODE::DIFF3RDONLY));
	}

}  // namespace
