/**
 * @file  PropertySystem_test.cpp
 *
 * @brief Implementation for PropertySystemTest testcase.
 */

#include "pch.h"
#include <gtest/gtest.h>
#include "PropertySystem.h"
#include "Environment.h"
#include "paths.h"

#ifdef _WIN64

namespace
{

	class PropertySystemTest : public testing::Test
	{
	protected:
		PropertySystemTest()
		{
			// You can do set-up work for each test	here.
		}

		virtual ~PropertySystemTest()
		{
			// You can do clean-up work	that doesn't throw exceptions here.
		}

		// If	the	constructor	and	destructor are not enough for setting up
		// and cleaning up each test, you can define the following methods:

		virtual void SetUp()
		{
			// Code	here will be called	immediately	after the constructor (right
			// before each test).
			CoInitialize(nullptr);
			LANGID lang = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
			m_oldlang = GetThreadUILanguage();
			SetThreadUILanguage(lang);
		}

		virtual void TearDown()
		{
			// Code	here will be called	immediately	after each test	(right
			// before the destructor).
			CoUninitialize();
			SetThreadUILanguage(m_oldlang);
		}

		// Objects declared here can be used by all tests in the test case for Foo.
		LANGID m_oldlang;
	};

	TEST_F(PropertySystemTest, GetDisplayNames)
	{
		{
			PropertySystem ps({ _T("System.Image.Dimensions"), _T("System.Image.HorizontalSize") });
			std::vector<String> names;
			ASSERT_TRUE(ps.GetDisplayNames(names));
			ASSERT_STREQ(_T("Dimensions"), names[0].c_str());
			ASSERT_STREQ(_T("Width"), names[1].c_str());
		}

		{
			PropertySystem ps2(PropertySystem::VIEWABLE);
			std::vector<String> names2;
			ASSERT_TRUE(ps2.GetDisplayNames(names2));
		}
	}

	TEST_F(PropertySystemTest, GetFormattedValues)
	{
		PropertySystem ps({ _T("System.MIMEType"), _T("System.KindText")});
		std::vector<String> values;
		String path = paths::GetLongPath(paths::ConcatPath(env::GetProgPath(), _T("..\\..\\..\\Src\\res\\splash.jpg")));
		ASSERT_TRUE(ps.GetFormattedValues(path, values));
		ASSERT_STREQ(_T("image/jpeg"), values[0].c_str());
		ASSERT_STREQ(_T("Picture"), values[1].c_str());
	}

}

#endif
