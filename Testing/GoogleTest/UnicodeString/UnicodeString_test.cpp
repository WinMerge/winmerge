#include "pch.h"
#include <gtest/gtest.h>
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
		String orig(_T("OneTwoThree"));
		String find(_T("Blaa"));
		String rep(_T("Five"));
		strutils::replace(orig, find, rep);
		EXPECT_EQ(_T("OneTwoThree"), orig);
	}

	TEST_F(UnicodeStringTest, ReplaceMiddle1)
	{
		String orig(_T("OneTwoThree"));
		String find(_T("Two"));
		String rep(_T("Five"));
		strutils::replace(orig, find, rep);
		EXPECT_EQ(_T("OneFiveThree"), orig);
	}

	TEST_F(UnicodeStringTest, ReplaceMiddle2)
	{
		String orig(_T("OneTwoThree"));
		String find(_T("Two"));
		String rep(_T(""));
		strutils::replace(orig, find, rep);
		EXPECT_EQ(_T("OneThree"), orig);
	}

	TEST_F(UnicodeStringTest, ReplaceBegin1)
	{
		String orig(_T("OneTwoThree"));
		String find(_T("One"));
		String rep(_T("five"));
		strutils::replace(orig, find, rep);
		EXPECT_EQ(_T("fiveTwoThree"), orig);
	}

	TEST_F(UnicodeStringTest, ReplaceBegin2)
	{
		String orig(_T("OneTwoThree"));
		String find(_T("One"));
		String rep(_T(""));
		strutils::replace(orig, find, rep);
		EXPECT_EQ(_T("TwoThree"), orig);
	}

	TEST_F(UnicodeStringTest, ReplaceEnd1)
	{
		String orig(_T("OneTwoThree"));
		String find(_T("Three"));
		String rep(_T("seven"));
		strutils::replace(orig, find, rep);
		EXPECT_EQ(_T("OneTwoseven"), orig);
	}

	TEST_F(UnicodeStringTest, ReplaceEnd2)
	{
		String orig(_T("OneTwoThree"));
		String find(_T("Three"));
		String rep(_T(""));
		strutils::replace(orig, find, rep);
		EXPECT_EQ(_T("OneTwo"), orig);
	}

	TEST_F(UnicodeStringTest, MakelowerEmpty)
	{
		String orig(_T(""));
		String result = strutils::makelower(orig);
		EXPECT_EQ(_T(""), result);
	}

	TEST_F(UnicodeStringTest, MakelowerWhitespace)
	{
		String orig(_T(" \t"));
		String result = strutils::makelower(orig);
		EXPECT_EQ(_T(" \t"), result);
	}

	TEST_F(UnicodeStringTest, Makelower1)
	{
		String orig(_T("onetwothree"));
		String result = strutils::makelower(orig);
		EXPECT_EQ(_T("onetwothree"), result);
	}

	TEST_F(UnicodeStringTest, Makelower2)
	{
		String orig(_T("OneTwoThree"));
		String result = strutils::makelower(orig);
		EXPECT_EQ(_T("onetwothree"), result);
	}

	TEST_F(UnicodeStringTest, TrimwsbeginEmpty)
	{
		String orig(_T(""));
		String result = strutils::trim_ws_begin(orig);
		EXPECT_EQ(_T(""), result);
	}

	TEST_F(UnicodeStringTest, TrimwsbeginNoWs)
	{
		String orig(_T("abc"));
		String result = strutils::trim_ws_begin(orig);
		EXPECT_EQ(_T("abc"), result);
	}

	TEST_F(UnicodeStringTest, TrimwsbeginWsEnd)
	{
		String orig(_T("abc\t"));
		String result = strutils::trim_ws_begin(orig);
		EXPECT_EQ(_T("abc\t"), result);
	}

	TEST_F(UnicodeStringTest, TrimwsbeginWsOnly)
	{
		String orig(_T("\t"));
		String result = strutils::trim_ws_begin(orig);
		EXPECT_EQ(_T(""), result);
	}

	TEST_F(UnicodeStringTest, TrimwsbeginWs1)
	{
		String orig(_T("\tabc"));
		String result = strutils::trim_ws_begin(orig);
		EXPECT_EQ(_T("abc"), result);
	}
	
	TEST_F(UnicodeStringTest, TrimwsbeginWs2)
	{
		String orig(_T("\t\n abc"));
		String result = strutils::trim_ws_begin(orig);
		EXPECT_EQ(_T("abc"), result);
	}

	TEST_F(UnicodeStringTest, TrimwsendEmpty)
	{
		String orig(_T(""));
		String result = strutils::trim_ws_end(orig);
		EXPECT_EQ(_T(""), result);
	}

	TEST_F(UnicodeStringTest, TrimwsendNoWs)
	{
		String orig(_T("abc"));
		String result = strutils::trim_ws_end(orig);
		EXPECT_EQ(_T("abc"), result);
	}

	TEST_F(UnicodeStringTest, TrimwsendWsBegin)
	{
		String orig(_T("\tabc"));
		String result = strutils::trim_ws_end(orig);
		EXPECT_EQ(_T("\tabc"), result);
	}

	TEST_F(UnicodeStringTest, TrimwsendWsOnly)
	{
		String orig(_T("\t"));
		String result = strutils::trim_ws_end(orig);
		EXPECT_EQ(_T(""), result);
	}

	TEST_F(UnicodeStringTest, TrimwsendWs1)
	{
		String orig(_T("abc\t"));
		String result = strutils::trim_ws_end(orig);
		EXPECT_EQ(_T("abc"), result);
	}
	
	TEST_F(UnicodeStringTest, TrimwsendWs2)
	{
		String orig(_T("abc\t\n "));
		String result = strutils::trim_ws_end(orig);
		EXPECT_EQ(_T("abc"), result);
	}

	TEST_F(UnicodeStringTest, TrimwsEmpty)
	{
		String orig(_T(""));
		String result = strutils::trim_ws(orig);
		EXPECT_EQ(_T(""), result);
	}

	TEST_F(UnicodeStringTest, TrimwsNoWs)
	{
		String orig(_T("abc"));
		String result = strutils::trim_ws(orig);
		EXPECT_EQ(_T("abc"), result);
	}

	TEST_F(UnicodeStringTest, TrimwsWsOnly)
	{
		String orig(_T("\t"));
		String result = strutils::trim_ws(orig);
		EXPECT_EQ(_T(""), result);
	}

	TEST_F(UnicodeStringTest, TrimwsWsOnly2)
	{
		String orig(_T("\t \n"));
		String result = strutils::trim_ws(orig);
		EXPECT_EQ(_T(""), result);
	}

	TEST_F(UnicodeStringTest, TrimwsWsOnly3)
	{
		String orig(_T(" \t  \t "));
		String result = strutils::trim_ws(orig);
		EXPECT_EQ(_T(""), result);
	}

	TEST_F(UnicodeStringTest, TrimwsWsBegin)
	{
		String orig(_T("\tabc"));
		String result = strutils::trim_ws(orig);
		EXPECT_EQ(_T("abc"), result);
	}

	TEST_F(UnicodeStringTest, TrimwsWsBegin2)
	{
		String orig(_T("\t\n abc"));
		String result = strutils::trim_ws(orig);
		EXPECT_EQ(_T("abc"), result);
	}

	TEST_F(UnicodeStringTest, TrimwsWsEnd1)
	{
		String orig(_T("abc\t"));
		String result = strutils::trim_ws(orig);
		EXPECT_EQ(_T("abc"), result);
	}
	
	TEST_F(UnicodeStringTest, TrimwsWsEnd2)
	{
		String orig(_T("abc\t\n "));
		String result = strutils::trim_ws(orig);
		EXPECT_EQ(_T("abc"), result);
	}

	TEST_F(UnicodeStringTest, TrimwsWsBoth1)
	{
		String orig(_T("\tabc\t"));
		String result = strutils::trim_ws(orig);
		EXPECT_EQ(_T("abc"), result);
	}

	TEST_F(UnicodeStringTest, TrimwsWsBoth2)
	{
		String orig(_T("\n \tabc \t\n"));
		String result = strutils::trim_ws(orig);
		EXPECT_EQ(_T("abc"), result);
	}

	TEST_F(UnicodeStringTest, ComparenocaseEmpty)
	{
		EXPECT_EQ(0, strutils::compare_nocase(_T(""), _T("")));
	}

	TEST_F(UnicodeStringTest, ComparenocaseWhitespace1)
	{
		EXPECT_EQ(0, strutils::compare_nocase(_T(" "), _T(" ")));
	}

	TEST_F(UnicodeStringTest, ComparenocaseWhitespace)
	{
		EXPECT_NE(0, strutils::compare_nocase(_T(" "), _T("\t")));
	}

	TEST_F(UnicodeStringTest, ComparenocaseLowercase)
	{
		EXPECT_EQ(0, strutils::compare_nocase(_T("abc"), _T("abc")));
	}

	TEST_F(UnicodeStringTest, ComparenocaseUppercase)
	{
		EXPECT_EQ(0, strutils::compare_nocase(_T("ABC"), _T("ABC")));
	}

	TEST_F(UnicodeStringTest, ComparenocaseMixedcase)
	{
		EXPECT_EQ(0, strutils::compare_nocase(_T("AbC"), _T("AbC")));
	}

	TEST_F(UnicodeStringTest, ComparenocaseIdentical1)
	{
		EXPECT_EQ(0, strutils::compare_nocase(_T("abc"), _T("aBc")));
	}

	TEST_F(UnicodeStringTest, ComparenocaseIdentical2)
	{
		EXPECT_EQ(0, strutils::compare_nocase(_T("ABc"), _T("abc")));
	}

	TEST_F(UnicodeStringTest, ComparenocaseDifferent1)
	{
		EXPECT_NE(0, strutils::compare_nocase(_T("abc"), _T("adc")));
	}

	TEST_F(UnicodeStringTest, ComparenocaseDifferent2)
	{
		EXPECT_NE(0, strutils::compare_nocase(_T("aBc"), _T("adc")));
	}

	TEST_F(UnicodeStringTest, ComparenocaseDifferent3)
	{
		EXPECT_NE(0, strutils::compare_nocase(_T("abc"), _T("aDc")));
	}

	TEST_F(UnicodeStringTest, Format1)
	{
		EXPECT_TRUE(String(255, ' ') == strutils::format(_T("%s"), String(255, ' ').c_str()));
		EXPECT_TRUE(String(256, ' ') == strutils::format(_T("%s"), String(256, ' ').c_str()));
		EXPECT_TRUE(String(257, ' ') == strutils::format(_T("%s"), String(257, ' ').c_str()));
	}

	TEST_F(UnicodeStringTest, Join)
	{
		std::vector<String> strs;
		strs = std::vector<String>{ _T("") };
		EXPECT_EQ(_T(""), strutils::join(strs.begin(), strs.end(), _T("|")));
		strs = std::vector<String>();
		EXPECT_EQ(_T(""), strutils::join(strs.begin(), strs.end(), _T("|")));
		strs = std::vector<String>{ _T(""), _T("") };
		EXPECT_EQ(_T("|"), strutils::join(strs.begin(), strs.end(), _T("|")));
		strs = std::vector<String>{ _T("a") };
		EXPECT_EQ(_T("a"), strutils::join(strs.begin(), strs.end(), _T("|")));
		strs = std::vector<String>{ _T("a"),  _T("b") };
		EXPECT_EQ(_T("a|b"), strutils::join(strs.begin(), strs.end(), _T("|")));
		strs = std::vector<String>{ _T(""),  _T("b") };
		EXPECT_EQ(_T("|b"), strutils::join(strs.begin(), strs.end(), _T("|")));
		strs = std::vector<String>{ _T("a"),  _T("") };
		EXPECT_EQ(_T("a|"), strutils::join(strs.begin(), strs.end(), _T("|")));
		strs = std::vector<String>{ _T("a"),  _T("b"), _T("c") };
		EXPECT_EQ(_T("a|b|c"), strutils::join(strs.begin(), strs.end(), _T("|")));
		strs = std::vector<String>{ _T(""),  _T("b"), _T("c") };
		EXPECT_EQ(_T("|b|c"), strutils::join(strs.begin(), strs.end(), _T("|")));
		strs = std::vector<String>{ _T("a"),  _T(""), _T("c") };
		EXPECT_EQ(_T("a||c"), strutils::join(strs.begin(), strs.end(), _T("|")));
		strs = std::vector<String>{ _T("a"),  _T("b"), _T("") };
		EXPECT_EQ(_T("a|b|"), strutils::join(strs.begin(), strs.end(), _T("|")));

		strs = std::vector<String>{ };
		EXPECT_EQ(_T(""), strutils::join(strs.begin(), strs.end(), _T("|"),
			[](const auto& str) { return strutils::makeupper(str); }));
		strs = std::vector<String>{ _T("") };
		EXPECT_EQ(_T(""), strutils::join(strs.begin(), strs.end(), _T("|"),
			[](const auto& str) { return strutils::makeupper(str); }));
		strs = std::vector<String>{ _T(""), _T("")};
		EXPECT_EQ(_T("|"), strutils::join(strs.begin(), strs.end(), _T("|"),
			[](const auto& str) { return strutils::makeupper(str); }));
		strs = std::vector<String>{ _T("a"),  _T("b"), _T("c") };
		EXPECT_EQ(_T("A|B|C"), strutils::join(strs.begin(), strs.end(), _T("|"),
			[](const auto& str) { return strutils::makeupper(str); }));
		strs = std::vector<String>{ _T(""),  _T("b"), _T("c") };
		EXPECT_EQ(_T("|B|C"), strutils::join(strs.begin(), strs.end(), _T("|"),
			[](const auto& str) { return strutils::makeupper(str); }));
		strs = std::vector<String>{ _T("a"),  _T(""), _T("c") };
		EXPECT_EQ(_T("A||C"), strutils::join(strs.begin(), strs.end(), _T("|"),
			[](const auto& str) { return strutils::makeupper(str); }));
		strs = std::vector<String>{ _T("a"),  _T("b"), _T("") };
		EXPECT_EQ(_T("A|B|"), strutils::join(strs.begin(), strs.end(), _T("|"),
			[](const auto& str) { return strutils::makeupper(str); }));
	}

}  // namespace
