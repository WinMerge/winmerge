#include "pch.h"
#include <gtest/gtest.h>
#include "codepage_detect.h"
#include "charsets.h"

namespace
{
	// The fixture for testing paths functions.
	class CodepageDetectTest : public testing::Test
	{
	protected:
		// You can remove any or all of the following functions if its body
		// is	empty.

		CodepageDetectTest()
		{
			// You can do set-up work for each test	here.
			charsets_init();
		}

		virtual ~CodepageDetectTest()
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

	TEST_F(CodepageDetectTest, GuessCodepageEncoding0)
	{
		FileTextEncoding enc;
		enc = GuessCodepageEncoding(_T("../../Data/Unicode/UCS-2LE/DiffItem.h"), 0);
		EXPECT_EQ(1200, enc.m_codepage);
		EXPECT_EQ(true, enc.m_bom);
		EXPECT_EQ(ucr::UCS2LE, enc.m_unicoding);
		enc = GuessCodepageEncoding(_T("../../Data/Unicode/UCS-2BE/DiffItem.h"), 0);
		EXPECT_EQ(1201, enc.m_codepage);
		EXPECT_EQ(true, enc.m_bom);
		EXPECT_EQ(ucr::UCS2BE, enc.m_unicoding);
		enc = GuessCodepageEncoding(_T("../../Data/Unicode/UTF-8/DiffItem.h"), 0);
		EXPECT_EQ(65001, enc.m_codepage);
		EXPECT_EQ(true, enc.m_bom);
		EXPECT_EQ(ucr::UTF8, enc.m_unicoding);
		enc = GuessCodepageEncoding(_T("../../Data/Unicode/UTF-8-NOBOM/DiffItem.h"), 0);
		EXPECT_EQ(ucr::getDefaultCodepage(), enc.m_codepage);
		EXPECT_EQ(false, enc.m_bom);
		EXPECT_EQ(ucr::NONE, enc.m_unicoding);

		enc = GuessCodepageEncoding(_T("abcdefg12345"), 0);
		EXPECT_EQ(ucr::getDefaultCodepage(), enc.m_codepage);
		EXPECT_EQ(false, enc.m_bom);
		EXPECT_EQ(ucr::NONE, enc.m_unicoding);
	}

	TEST_F(CodepageDetectTest, GuessCodepageEncoding1)
	{
		FileTextEncoding enc;
		enc = GuessCodepageEncoding(_T("../../Data/Unicode/UTF-8-NOBOM/DiffItem.h"), 1);
		EXPECT_EQ(65001, enc.m_codepage);
		EXPECT_EQ(false, enc.m_bom);
		EXPECT_EQ(ucr::UTF8, enc.m_unicoding);

		enc = GuessCodepageEncoding(_T("../../../Docs/Users/Manual/About_Doc.xml"), 1);
		EXPECT_EQ(65001, enc.m_codepage);
		EXPECT_EQ(false, enc.m_bom);
		EXPECT_EQ(ucr::UTF8, enc.m_unicoding);

		enc = GuessCodepageEncoding(_T("../../../Docs/Developers/readme-developers.html"), 1);
		EXPECT_EQ(28591, enc.m_codepage);
		EXPECT_EQ(false, enc.m_bom);
		EXPECT_EQ(ucr::NONE, enc.m_unicoding);

		enc = GuessCodepageEncoding(_T("../../../ShellExtension/Languages/ShellExtensionRussian.rc"), 1);
		EXPECT_EQ(65001, enc.m_codepage);
		EXPECT_EQ(true, enc.m_bom);
		EXPECT_EQ(ucr::UTF8, enc.m_unicoding);
	}


}  // namespace
