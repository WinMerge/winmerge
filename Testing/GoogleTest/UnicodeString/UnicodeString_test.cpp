#include <gtest/gtest.h>
#include <windows.h>
#include <tchar.h>

#include "UnicodeString.h"

namespace
{
	// The fixture for testing paths functions.
	class UnicodeStringTest : public testing::Test
	{
	protected:
		// You can remove any or all of the following functions if its body
		// is	empty.

		UnicodeStringTest()
		{
			// You can do set-up work for each test	here.
		}

		virtual ~UnicodeStringTest()
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

	TEST_F(UnicodeStringTest, Construct1)
	{
		String str;
		EXPECT_TRUE(str.empty());
	}

	TEST_F(UnicodeStringTest, ReplaceNotFound)
	{
		String orig("OneTwoThree");
		String find("Blaa");
		String rep("Five");
		string_replace(orig, find, rep);
		EXPECT_EQ("OneTwoThree", orig);
	}

	TEST_F(UnicodeStringTest, ReplaceMiddle1)
	{
		String orig("OneTwoThree");
		String find("Two");
		String rep("Five");
		string_replace(orig, find, rep);
		EXPECT_EQ("OneFiveThree", orig);
	}

	TEST_F(UnicodeStringTest, ReplaceMiddle2)
	{
		String orig("OneTwoThree");
		String find("Two");
		String rep("");
		string_replace(orig, find, rep);
		EXPECT_EQ("OneThree", orig);
	}

	TEST_F(UnicodeStringTest, ReplaceBegin1)
	{
		String orig("OneTwoThree");
		String find("One");
		String rep("five");
		string_replace(orig, find, rep);
		EXPECT_EQ("fiveTwoThree", orig);
	}

	TEST_F(UnicodeStringTest, ReplaceBegin2)
	{
		String orig("OneTwoThree");
		String find("One");
		String rep("");
		string_replace(orig, find, rep);
		EXPECT_EQ("TwoThree", orig);
	}

	TEST_F(UnicodeStringTest, ReplaceEnd1)
	{
		String orig("OneTwoThree");
		String find("Three");
		String rep("seven");
		string_replace(orig, find, rep);
		EXPECT_EQ("OneTwoseven", orig);
	}

	TEST_F(UnicodeStringTest, ReplaceEnd2)
	{
		String orig("OneTwoThree");
		String find("Three");
		String rep("");
		string_replace(orig, find, rep);
		EXPECT_EQ("OneTwo", orig);
	}

	TEST_F(UnicodeStringTest, MakelowerEmpty)
	{
		String orig("");
		String result = string_makelower(orig);
		EXPECT_EQ("", result);
	}

	TEST_F(UnicodeStringTest, MakelowerWhitespace)
	{
		String orig(" \t");
		String result = string_makelower(orig);
		EXPECT_EQ(" \t", result);
	}

	TEST_F(UnicodeStringTest, Makelower1)
	{
		String orig("onetwothree");
		String result = string_makelower(orig);
		EXPECT_EQ("onetwothree", result);
	}

	TEST_F(UnicodeStringTest, Makelower2)
	{
		String orig("OneTwoThree");
		String result = string_makelower(orig);
		EXPECT_EQ("onetwothree", result);
	}

	TEST_F(UnicodeStringTest, TrimwsbeginEmpty)
	{
		String orig("");
		String result = string_trim_ws_begin(orig);
		EXPECT_EQ("", result);
	}

	TEST_F(UnicodeStringTest, TrimwsbeginNoWs)
	{
		String orig("abc");
		String result = string_trim_ws_begin(orig);
		EXPECT_EQ("abc", result);
	}

	TEST_F(UnicodeStringTest, TrimwsbeginWsEnd)
	{
		String orig("abc\t");
		String result = string_trim_ws_begin(orig);
		EXPECT_EQ("abc\t", result);
	}

	TEST_F(UnicodeStringTest, TrimwsbeginWsOnly)
	{
		String orig("\t");
		String result = string_trim_ws_begin(orig);
		EXPECT_EQ("", result);
	}

	TEST_F(UnicodeStringTest, TrimwsbeginWs1)
	{
		String orig("\tabc");
		String result = string_trim_ws_begin(orig);
		EXPECT_EQ("abc", result);
	}
	
	TEST_F(UnicodeStringTest, TrimwsbeginWs2)
	{
		String orig("\t\n abc");
		String result = string_trim_ws_begin(orig);
		EXPECT_EQ("abc", result);
	}

	TEST_F(UnicodeStringTest, TrimwsendEmpty)
	{
		String orig("");
		String result = string_trim_ws_end(orig);
		EXPECT_EQ("", result);
	}

	TEST_F(UnicodeStringTest, TrimwsendNoWs)
	{
		String orig("abc");
		String result = string_trim_ws_end(orig);
		EXPECT_EQ("abc", result);
	}

	TEST_F(UnicodeStringTest, TrimwsendWsBegin)
	{
		String orig("\tabc");
		String result = string_trim_ws_end(orig);
		EXPECT_EQ("\tabc", result);
	}

	TEST_F(UnicodeStringTest, TrimwsendWsOnly)
	{
		String orig("\t");
		String result = string_trim_ws_end(orig);
		EXPECT_EQ("", result);
	}

	TEST_F(UnicodeStringTest, TrimwsendWs1)
	{
		String orig("abc\t");
		String result = string_trim_ws_end(orig);
		EXPECT_EQ("abc", result);
	}
	
	TEST_F(UnicodeStringTest, TrimwsendWs2)
	{
		String orig("abc\t\n ");
		String result = string_trim_ws_end(orig);
		EXPECT_EQ("abc", result);
	}

	TEST_F(UnicodeStringTest, TrimwsEmpty)
	{
		String orig("");
		String result = string_trim_ws(orig);
		EXPECT_EQ("", result);
	}

	TEST_F(UnicodeStringTest, TrimwsNoWs)
	{
		String orig("abc");
		String result = string_trim_ws(orig);
		EXPECT_EQ("abc", result);
	}

	TEST_F(UnicodeStringTest, TrimwsWsOnly)
	{
		String orig("\t");
		String result = string_trim_ws(orig);
		EXPECT_EQ("", result);
	}

	TEST_F(UnicodeStringTest, TrimwsWsOnly2)
	{
		String orig("\t \n");
		String result = string_trim_ws(orig);
		EXPECT_EQ("", result);
	}

	TEST_F(UnicodeStringTest, TrimwsWsOnly3)
	{
		String orig(" \t  \t ");
		String result = string_trim_ws(orig);
		EXPECT_EQ("", result);
	}

	TEST_F(UnicodeStringTest, TrimwsWsBegin)
	{
		String orig("\tabc");
		String result = string_trim_ws(orig);
		EXPECT_EQ("abc", result);
	}

	TEST_F(UnicodeStringTest, TrimwsWsBegin2)
	{
		String orig("\t\n abc");
		String result = string_trim_ws(orig);
		EXPECT_EQ("abc", result);
	}

	TEST_F(UnicodeStringTest, TrimwsWsEnd1)
	{
		String orig("abc\t");
		String result = string_trim_ws(orig);
		EXPECT_EQ("abc", result);
	}
	
	TEST_F(UnicodeStringTest, TrimwsWsEnd2)
	{
		String orig("abc\t\n ");
		String result = string_trim_ws(orig);
		EXPECT_EQ("abc", result);
	}

	TEST_F(UnicodeStringTest, TrimwsWsBoth1)
	{
		String orig("\tabc\t");
		String result = string_trim_ws(orig);
		EXPECT_EQ("abc", result);
	}

	TEST_F(UnicodeStringTest, TrimwsWsBoth2)
	{
		String orig("\n \tabc \t\n");
		String result = string_trim_ws(orig);
		EXPECT_EQ("abc", result);
	}

	TEST_F(UnicodeStringTest, ComparenocaseEmpty)
	{
		EXPECT_EQ(0, string_compare_nocase("", ""));
	}

	TEST_F(UnicodeStringTest, ComparenocaseWhitespace1)
	{
		EXPECT_EQ(0, string_compare_nocase(" ", " "));
	}

	TEST_F(UnicodeStringTest, ComparenocaseWhitespace)
	{
		EXPECT_NE(0, string_compare_nocase(" ", "\t"));
	}

	TEST_F(UnicodeStringTest, ComparenocaseLowercase)
	{
		EXPECT_EQ(0, string_compare_nocase("abc", "abc"));
	}

	TEST_F(UnicodeStringTest, ComparenocaseUppercase)
	{
		EXPECT_EQ(0, string_compare_nocase("ABC", "ABC"));
	}

	TEST_F(UnicodeStringTest, ComparenocaseMixedcase)
	{
		EXPECT_EQ(0, string_compare_nocase("AbC", "AbC"));
	}

	TEST_F(UnicodeStringTest, ComparenocaseIdentical1)
	{
		EXPECT_EQ(0, string_compare_nocase("abc", "aBc"));
	}

	TEST_F(UnicodeStringTest, ComparenocaseIdentical2)
	{
		EXPECT_EQ(0, string_compare_nocase("ABc", "abc"));
	}

	TEST_F(UnicodeStringTest, ComparenocaseDifferent1)
	{
		EXPECT_NE(0, string_compare_nocase("abc", "adc"));
	}

	TEST_F(UnicodeStringTest, ComparenocaseDifferent2)
	{
		EXPECT_NE(0, string_compare_nocase("aBc", "adc"));
	}

	TEST_F(UnicodeStringTest, ComparenocaseDifferent3)
	{
		EXPECT_NE(0, string_compare_nocase("abc", "aDc"));
	}

}  // namespace
