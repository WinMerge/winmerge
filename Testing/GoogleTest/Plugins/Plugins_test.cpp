#include "pch.h"
#include <gtest/gtest.h>
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
		CAssureScriptsForThread asft(nullptr);
		PackingInfo *iu = nullptr;
		PrediffingInfo *ip = nullptr;
		PluginManager pm;
		IPluginInfos *ppi = &pm;
		std::vector<int> subcodes;
		ppi->FetchPluginInfos(_T("../../Data/Office/excel.xls|../../Data/Office/excel.xls"), &iu, &ip);
		String file = paths::ConcatPath(oldModulePath, _T("..\\..\\Data\\Office\\excel.xls"));
		iu->Unpacking(&subcodes, file, _T(".*\\.xls"), { file });
	}

	TEST_F(PluginsTest, ParsePluginPipeline)
	{
		String errorMessage;
		auto parseResult = PluginForFile::ParsePluginPipeline(_T(""), errorMessage);
		EXPECT_TRUE(errorMessage.empty());
		EXPECT_EQ(0, parseResult.size());
		String pluginPipeline = PluginForFile::MakePluginPipeline(parseResult);
		EXPECT_EQ(_T(""), pluginPipeline);

		parseResult = PluginForFile::ParsePluginPipeline(_T(" "), errorMessage);
		EXPECT_TRUE(errorMessage.empty());
		EXPECT_EQ(0, parseResult.size());
		pluginPipeline = PluginForFile::MakePluginPipeline(parseResult);
		EXPECT_EQ(_T(""), pluginPipeline);


		parseResult = PluginForFile::ParsePluginPipeline(_T("<Automatic>"), errorMessage);
		EXPECT_TRUE(errorMessage.empty());
		EXPECT_EQ(1, parseResult.size());
		EXPECT_EQ(_T("<Automatic>"), parseResult[0].name);
		EXPECT_EQ(0, parseResult[0].args.size());
		EXPECT_EQ(0, parseResult[0].quoteChar);
		pluginPipeline = PluginForFile::MakePluginPipeline(parseResult);
		EXPECT_EQ(_T("<Automatic>"), pluginPipeline);


		parseResult = PluginForFile::ParsePluginPipeline(_T(" <Automatic> "), errorMessage);
		EXPECT_TRUE(errorMessage.empty());
		EXPECT_EQ(1, parseResult.size());
		EXPECT_EQ(_T("<Automatic>"), parseResult[0].name);
		EXPECT_EQ(0, parseResult[0].args.size());
		EXPECT_EQ(0, parseResult[0].quoteChar);
		pluginPipeline = PluginForFile::MakePluginPipeline(parseResult);
		EXPECT_EQ(_T("<Automatic>"), pluginPipeline);

		parseResult = PluginForFile::ParsePluginPipeline(_T("'a b'"), errorMessage);
		EXPECT_TRUE(errorMessage.empty());
		EXPECT_EQ(1, parseResult.size());
		EXPECT_EQ(_T("a b"), parseResult[0].name);
		EXPECT_EQ(0, parseResult[0].args.size());
		EXPECT_EQ('\'', parseResult[0].quoteChar);
		pluginPipeline = PluginForFile::MakePluginPipeline(parseResult);
		EXPECT_EQ(_T("'a b'"), pluginPipeline);

		parseResult = PluginForFile::ParsePluginPipeline(_T("'a''b'"), errorMessage);
		EXPECT_TRUE(errorMessage.empty());
		EXPECT_EQ(1, parseResult.size());
		EXPECT_EQ(_T("a'b"), parseResult[0].name);
		EXPECT_EQ(0, parseResult[0].args.size());
		EXPECT_EQ('\'', parseResult[0].quoteChar);
		pluginPipeline = PluginForFile::MakePluginPipeline(parseResult);
		EXPECT_EQ(_T("'a''b'"), pluginPipeline);

		parseResult = PluginForFile::ParsePluginPipeline(_T("a' 'b"), errorMessage);
		EXPECT_TRUE(errorMessage.empty());
		EXPECT_EQ(1, parseResult.size());
		EXPECT_EQ(_T("a b"), parseResult[0].name);
		EXPECT_EQ(0, parseResult[0].args.size());
		EXPECT_EQ('\'', parseResult[0].quoteChar);
		pluginPipeline = PluginForFile::MakePluginPipeline(parseResult);
		EXPECT_EQ(_T("'a b'"), pluginPipeline);

		parseResult = PluginForFile::ParsePluginPipeline(_T("a' '\"b\""), errorMessage);
		EXPECT_TRUE(errorMessage.empty());
		EXPECT_EQ(1, parseResult.size());
		EXPECT_EQ(_T("a b"), parseResult[0].name);
		EXPECT_EQ(0, parseResult[0].args.size());
		EXPECT_EQ('"', parseResult[0].quoteChar);
		pluginPipeline = PluginForFile::MakePluginPipeline(parseResult);
		EXPECT_EQ(_T("\"a b\""), pluginPipeline);

		parseResult = PluginForFile::ParsePluginPipeline(_T("\"a b"), errorMessage);
		EXPECT_TRUE(!errorMessage.empty());

		parseResult = PluginForFile::ParsePluginPipeline(_T("ExecFilterCommand dir c:\\"), errorMessage);
		EXPECT_TRUE(errorMessage.empty());
		EXPECT_EQ(1, parseResult.size());
		EXPECT_EQ(_T("ExecFilterCommand"), parseResult[0].name);
		EXPECT_EQ(_T("dir"), parseResult[0].args[0]);
		EXPECT_EQ(_T("c:\\"), parseResult[0].args[1]);
		EXPECT_EQ(0, parseResult[0].quoteChar);
		pluginPipeline = PluginForFile::MakePluginPipeline(parseResult);
		EXPECT_EQ(_T("ExecFilterCommand dir c:\\"), pluginPipeline);

		parseResult = PluginForFile::ParsePluginPipeline(_T(" ExecFilterCommand  dir  c:\\ "), errorMessage);
		EXPECT_TRUE(errorMessage.empty());
		EXPECT_EQ(1, parseResult.size());
		EXPECT_EQ(_T("ExecFilterCommand"), parseResult[0].name);
		EXPECT_EQ(_T("dir"), parseResult[0].args[0]);
		EXPECT_EQ(_T("c:\\"), parseResult[0].args[1]);
		EXPECT_EQ(0, parseResult[0].quoteChar);
		pluginPipeline = PluginForFile::MakePluginPipeline(parseResult);
		EXPECT_EQ(_T("ExecFilterCommand dir c:\\"), pluginPipeline);

		parseResult = PluginForFile::ParsePluginPipeline(_T("ExecFilterCommand  'dir  c:\\ '"), errorMessage);
		EXPECT_TRUE(errorMessage.empty());
		EXPECT_EQ(1, parseResult.size());
		EXPECT_EQ(_T("ExecFilterCommand"), parseResult[0].name);
		EXPECT_EQ(_T("dir  c:\\ "), parseResult[0].args[0]);
		EXPECT_EQ('\'', parseResult[0].quoteChar);
		pluginPipeline = PluginForFile::MakePluginPipeline(parseResult);
		EXPECT_EQ(_T("ExecFilterCommand 'dir  c:\\ '"), pluginPipeline);

		parseResult = PluginForFile::ParsePluginPipeline(_T("ExecFilterCommand  'dir  c:\\ ' "), errorMessage);
		EXPECT_TRUE(errorMessage.empty());
		EXPECT_EQ(1, parseResult.size());
		EXPECT_EQ(_T("ExecFilterCommand"), parseResult[0].name);
		EXPECT_EQ(_T("dir  c:\\ "), parseResult[0].args[0]);
		EXPECT_EQ('\'', parseResult[0].quoteChar);
		pluginPipeline = PluginForFile::MakePluginPipeline(parseResult);
		EXPECT_EQ(_T("ExecFilterCommand 'dir  c:\\ '"), pluginPipeline);

		parseResult = PluginForFile::ParsePluginPipeline(_T(" 'a b'  'dir  c:\\ ' "), errorMessage);
		EXPECT_TRUE(errorMessage.empty());
		EXPECT_EQ(1, parseResult.size());
		EXPECT_EQ(_T("a b"), parseResult[0].name);
		EXPECT_EQ(_T("dir  c:\\ "), parseResult[0].args[0]);
		EXPECT_EQ('\'', parseResult[0].quoteChar);
		pluginPipeline = PluginForFile::MakePluginPipeline(parseResult);
		EXPECT_EQ(_T("'a b' 'dir  c:\\ '"), pluginPipeline);

		parseResult = PluginForFile::ParsePluginPipeline(_T(" 'a b'  dir  c:\\  "), errorMessage);
		EXPECT_TRUE(errorMessage.empty());
		EXPECT_EQ(1, parseResult.size());
		EXPECT_EQ(_T("a b"), parseResult[0].name);
		EXPECT_EQ(_T("dir"), parseResult[0].args[0]);
		EXPECT_EQ(_T("c:\\"), parseResult[0].args[1]);
		EXPECT_EQ('\'', parseResult[0].quoteChar);
		pluginPipeline = PluginForFile::MakePluginPipeline(parseResult);
		EXPECT_EQ(_T("'a b' 'dir' 'c:\\'"), pluginPipeline);

		parseResult = PluginForFile::ParsePluginPipeline(_T(" 'a b'  \"dir  c:\\ \" "), errorMessage);
		EXPECT_TRUE(errorMessage.empty());
		EXPECT_EQ(1, parseResult.size());
		EXPECT_EQ(_T("a b"), parseResult[0].name);
		EXPECT_EQ(_T("dir  c:\\ "), parseResult[0].args[0]);
		EXPECT_EQ('"', parseResult[0].quoteChar);
		pluginPipeline = PluginForFile::MakePluginPipeline(parseResult);
		EXPECT_EQ(_T("\"a b\" \"dir  c:\\ \""), pluginPipeline);

		parseResult = PluginForFile::ParsePluginPipeline(_T("ExecFilterCommand dir|MakeUpper"), errorMessage);
		EXPECT_TRUE(errorMessage.empty());
		EXPECT_EQ(2, parseResult.size());
		EXPECT_EQ(_T("ExecFilterCommand"), parseResult[0].name);
		EXPECT_EQ(_T("dir"), parseResult[0].args[0]);
		EXPECT_EQ(0, parseResult[0].quoteChar);
		EXPECT_EQ(_T("MakeUpper"), parseResult[1].name);
		EXPECT_EQ(0, parseResult[1].args.size());
		EXPECT_EQ(0, parseResult[1].quoteChar);
		pluginPipeline = PluginForFile::MakePluginPipeline(parseResult);
		EXPECT_EQ(_T("ExecFilterCommand dir|MakeUpper"), pluginPipeline);

		parseResult = PluginForFile::ParsePluginPipeline(_T("ExecFilterCommand dir | MakeUpper "), errorMessage);
		EXPECT_TRUE(errorMessage.empty());
		EXPECT_EQ(2, parseResult.size());
		EXPECT_EQ(_T("ExecFilterCommand"), parseResult[0].name);
		EXPECT_EQ(_T("dir"), parseResult[0].args[0]);
		EXPECT_EQ(0, parseResult[0].quoteChar);
		EXPECT_EQ(_T("MakeUpper"), parseResult[1].name);
		EXPECT_EQ(0, parseResult[1].args.size());
		EXPECT_EQ(0, parseResult[1].quoteChar);
		pluginPipeline = PluginForFile::MakePluginPipeline(parseResult);
		EXPECT_EQ(_T("ExecFilterCommand dir|MakeUpper"), pluginPipeline);

		parseResult = PluginForFile::ParsePluginPipeline(_T(" ExecFilterCommand dir | MakeUpper "), errorMessage);
		EXPECT_TRUE(errorMessage.empty());
		EXPECT_EQ(2, parseResult.size());
		EXPECT_EQ(_T("ExecFilterCommand"), parseResult[0].name);
		EXPECT_EQ(_T("dir"), parseResult[0].args[0]);
		EXPECT_EQ(0, parseResult[0].quoteChar);
		EXPECT_EQ(_T("MakeUpper"), parseResult[1].name);
		EXPECT_EQ(0, parseResult[1].args.size());
		EXPECT_EQ(0, parseResult[1].quoteChar);
		pluginPipeline = PluginForFile::MakePluginPipeline(parseResult);
		EXPECT_EQ(_T("ExecFilterCommand dir|MakeUpper"), pluginPipeline);

		parseResult = PluginForFile::ParsePluginPipeline(_T(" ExecFilterCommand 'dir  c:\\ ' | MakeUpper "), errorMessage);
		EXPECT_TRUE(errorMessage.empty());
		EXPECT_EQ(2, parseResult.size());
		EXPECT_EQ(_T("ExecFilterCommand"), parseResult[0].name);
		EXPECT_EQ(_T("dir  c:\\ "), parseResult[0].args[0]);
		EXPECT_EQ('\'', parseResult[0].quoteChar);
		EXPECT_EQ(_T("MakeUpper"), parseResult[1].name);
		EXPECT_EQ(0, parseResult[1].args.size());
		EXPECT_EQ(0, parseResult[1].quoteChar);
		pluginPipeline = PluginForFile::MakePluginPipeline(parseResult);
		EXPECT_EQ(_T("ExecFilterCommand 'dir  c:\\ '|MakeUpper"), pluginPipeline);

		parseResult = PluginForFile::ParsePluginPipeline(_T("ExecFilterCommand 'dir c:\\'|ExecFilterCommand 'sort'"), errorMessage);
		EXPECT_TRUE(errorMessage.empty());
		EXPECT_EQ(2, parseResult.size());
		EXPECT_EQ(_T("ExecFilterCommand"), parseResult[0].name);
		EXPECT_EQ(_T("dir c:\\"), parseResult[0].args[0]);
		EXPECT_EQ('\'', parseResult[0].quoteChar);
		EXPECT_EQ(_T("ExecFilterCommand"), parseResult[1].name);
		EXPECT_EQ(_T("sort"), parseResult[1].args[0]);
		EXPECT_EQ('\'', parseResult[1].quoteChar);
		pluginPipeline = PluginForFile::MakePluginPipeline(parseResult);
		EXPECT_EQ(_T("ExecFilterCommand 'dir c:\\'|ExecFilterCommand 'sort'"), pluginPipeline);


		parseResult = PluginForFile::ParsePluginPipeline(_T(" ExecFilterCommand 'dir  c:\\ ' | ExecFilterCommand ' sort ' "), errorMessage);
		EXPECT_TRUE(errorMessage.empty());
		EXPECT_EQ(2, parseResult.size());
		EXPECT_EQ(_T("ExecFilterCommand"), parseResult[0].name);
		EXPECT_EQ(_T("dir  c:\\ "), parseResult[0].args[0]);
		EXPECT_EQ('\'', parseResult[0].quoteChar);
		EXPECT_EQ(_T("ExecFilterCommand"), parseResult[1].name);
		EXPECT_EQ(_T(" sort "), parseResult[1].args[0]);
		EXPECT_EQ('\'', parseResult[1].quoteChar);
		pluginPipeline = PluginForFile::MakePluginPipeline(parseResult);
		EXPECT_EQ(_T("ExecFilterCommand 'dir  c:\\ '|ExecFilterCommand ' sort '"), pluginPipeline);

		parseResult = PluginForFile::ParsePluginPipeline(_T("|"), errorMessage);
		EXPECT_TRUE(!errorMessage.empty());

		parseResult = PluginForFile::ParsePluginPipeline(_T(" | "), errorMessage);
		EXPECT_TRUE(!errorMessage.empty());

		parseResult = PluginForFile::ParsePluginPipeline(_T("MakeUpper|"), errorMessage);
		EXPECT_TRUE(!errorMessage.empty());

		parseResult = PluginForFile::ParsePluginPipeline(_T("MakeUpper | "), errorMessage);
		EXPECT_TRUE(!errorMessage.empty());

		parseResult = PluginForFile::ParsePluginPipeline(_T("MakeUpper||"), errorMessage);
		EXPECT_TRUE(!errorMessage.empty());

		parseResult = PluginForFile::ParsePluginPipeline(_T("MakeUpper | | "), errorMessage);
		EXPECT_TRUE(!errorMessage.empty());
	}

	TEST_F(PluginsTest, ReplaceVariable)
	{
		String result;
		EXPECT_STREQ(_T(""), PluginForFile::MakeArguments(std::vector<String>{_T("")}, { _T("abc") }).c_str());
		EXPECT_STREQ(_T("%"), PluginForFile::MakeArguments(std::vector<String>{_T("%")}, {_T("abc")}).c_str());
		EXPECT_STREQ(_T("abc"), PluginForFile::MakeArguments(std::vector<String>{_T("%1")}, {_T("abc")}).c_str());
		EXPECT_STREQ(_T("%1"), PluginForFile::MakeArguments(std::vector<String>{_T("%%1")}, {_T("abc")}).c_str());
		EXPECT_STREQ(_T("%1a"), PluginForFile::MakeArguments(std::vector<String>{_T("%%1a")}, {_T("abc")}).c_str());
		EXPECT_STREQ(_T(""), PluginForFile::MakeArguments(std::vector<String>{_T("%2")}, {_T("abc")}).c_str());
		EXPECT_STREQ(_T("%TIME%"), PluginForFile::MakeArguments(std::vector<String>{_T("%TIME%")}, {_T("abc")}).c_str());
		EXPECT_STREQ(_T("aaaabcaaa"), PluginForFile::MakeArguments(std::vector<String>{_T("aaa%1aaa")}, {_T("abc")}).c_str());
		EXPECT_STREQ(_T("abcdef"), PluginForFile::MakeArguments(std::vector<String>{_T("%1%2")}, { _T("abc"), _T("def") }).c_str());
	}

}  // namespace
