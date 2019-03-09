#include "pch.h"
#include <gtest/gtest.h>
#include <tchar.h>
#include <vector>
#include "FileTransform.h"
#include "PluginManager.h"
#include "Plugins.h"
#include "paths.h"
#include "Environment.h"

using std::vector;

namespace
{
	// The fixture for testing string differencing functions.
	class PluginsTest : public testing::Test
	{
	protected:
		// You can remove any or all of the following functions if its body
		// is	empty.

		PluginsTest()
		{
			// You can do set-up work for each test	here.
		}

		virtual ~PluginsTest()
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

	TEST_F(PluginsTest, Unpack)
	{
		String oldModulePath = env::GetProgPath();
		env::SetProgPath(_T("c:/Program Files/WinMerge"));
		CAssureScriptsForThread asft;
		PackingInfo *iu = nullptr;
		PrediffingInfo *ip = nullptr;
		PluginManager pm;
		IPluginInfos *ppi = &pm;
		ppi->FetchPluginInfos(_T("../../Data/Office/excel.xls|../../Data/Office/excel.xls"), &iu, &ip);
		String file = paths::ConcatPath(oldModulePath, _T("..\\..\\Data\\Office\\excel.xls"));
		FileTransform::Unpacking(file, _T(".*\\.xls"), iu, &iu->m_subcode);
	}

}  // namespace
