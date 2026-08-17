#include "pch.h"
#include <gtest/gtest.h>
#include <vector>
#include "FileTransform.h"
#include "PluginManager.h"
#include "Plugins.h"
#include "paths.h"
#include "Environment.h"
#include "OptionsMgr.h"
#include "OptionsDef.h"
#include "MergeApp.h"

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
		iu->Unpacking(0, &subcodes, file, _T(".*\\.xls"), { file });
		env::SetProgPath(oldModulePath);
	}

	TEST_F(PluginsTest, Unpack_FilterExpression)
	{
		GetOptionsMgr()->InitOption(OPT_CMP_CSV_FILEPATTERNS, _T("*.csv"));
		GetOptionsMgr()->InitOption(OPT_CMP_CSV_DELIM_CHAR, _T(","));
		GetOptionsMgr()->InitOption(OPT_CMP_TSV_FILEPATTERNS, _T("*.tsv"));
		GetOptionsMgr()->InitOption(OPT_CMP_DSV_FILEPATTERNS, _T(""));
		GetOptionsMgr()->InitOption(OPT_CMP_DSV_DELIM_CHAR, _T(";"));
		GetOptionsMgr()->InitOption(OPT_CMP_TBL_ALLOW_NEWLINES_IN_QUOTES, true);
		GetOptionsMgr()->InitOption(OPT_CMP_TBL_QUOTE_CHAR, _T("\""));
		GetOptionsMgr()->InitOption(OPT_CP_DETECT, 0);
		std::vector<int> subcodes;
		String filepath[] = { _T("../../selftests/w/t001a.txt"), _T("../../selftests/u/t001a.txt"), _T("../../selftests/m/t001a.txt") };
		String filepaths = filepath[0] + _T("|") + filepath[1] + _T("|") + filepath[2];

		PackingInfo pi(String(_T("le:toUpper(Line)")));
		String file = filepath[0];
		EXPECT_TRUE(pi.Unpacking(0, &subcodes, file, filepaths, {filepath[0]}));
	}

	TEST_F(PluginsTest, Unpack_FilterExpression_Invalid)
	{
		GetOptionsMgr()->InitOption(OPT_CMP_CSV_FILEPATTERNS, _T("*.csv"));
		GetOptionsMgr()->InitOption(OPT_CMP_CSV_DELIM_CHAR, _T(","));
		GetOptionsMgr()->InitOption(OPT_CMP_TSV_FILEPATTERNS, _T("*.tsv"));
		GetOptionsMgr()->InitOption(OPT_CMP_DSV_FILEPATTERNS, _T(""));
		GetOptionsMgr()->InitOption(OPT_CMP_DSV_DELIM_CHAR, _T(";"));
		GetOptionsMgr()->InitOption(OPT_CMP_TBL_ALLOW_NEWLINES_IN_QUOTES, true);
		GetOptionsMgr()->InitOption(OPT_CMP_TBL_QUOTE_CHAR, _T("\""));
		GetOptionsMgr()->InitOption(OPT_CP_DETECT, 0);
		std::vector<int> subcodes;
		String filepath[] = { _T("../../selftests/w/t001a.txt"), _T("../../selftests/u/t001a.txt"), _T("../../selftests/m/t001a.txt") };
		String filepaths = filepath[0] + _T("|") + filepath[1] + _T("|") + filepath[2];

		PackingInfo pi(String(_T("le:toUpper(Line")));
		String file = filepath[0];
		EXPECT_FALSE(pi.Unpacking(0, &subcodes, file, filepaths, {filepath[0]}));
	}

	TEST_F(PluginsTest, EditorScript_FilterExpression)
	{
		GetOptionsMgr()->InitOption(OPT_CP_DETECT, 0);
		std::vector<int> subcodes;
		String filepath[] = { _T("../../selftests/w/t001a.txt"), _T("../../selftests/u/t001a.txt"), _T("../../selftests/m/t001a.txt") };
		String filepaths = filepath[0] + _T("|") + filepath[1] + _T("|") + filepath[2];

		EditorScriptInfo esi(_T("le:toUpper(Line)"));
		String file = filepath[0];
		bool changed = false;
		String text = _T("test");
		EXPECT_TRUE(esi.TransformText(0, text, {filepath[0]}, changed));
		EXPECT_STREQ(_T("TEST"), text.c_str());

		// Invalid filter expression
		esi.SetPluginPipeline(String(_T("le:toUpper(Line")));
		file = filepath[0];
		EXPECT_FALSE(esi.TransformText(0, text, {filepath[0]}, changed));
	}

	TEST_F(PluginsTest, EditorScript_FilterExpression_Column)
	{
		GetOptionsMgr()->InitOption(OPT_CP_DETECT, 0);
		std::vector<int> subcodes;
		String filepath[] = { _T("../../selftests/w/t005a.csv"), _T("../../selftests/u/t005a.csv"), _T("../../selftests/m/t005a.csv") };
		String filepaths = filepath[0] + _T("|") + filepath[1] + _T("|") + filepath[2];

		EditorScriptInfo esi(_T("le:Column(1) + Column(2) + Column(3) + Column(4)"));
		String file = filepath[0];
		bool changed = false;
		String text =
			LR"(Hi,World
Bye,World
Hi,World,1
Bye,World,2
Hi,,World
Bye,,World
,World
,Earth
Hi,World,
Bye,Earth,
Hi,World

Bye,World
Hi,"Hello,World"
Bye,"Goodbye,World"
Hi,"Hello ""World"""
Bye,"Goodbye ""World"""
Hi,"Hello, ""World""",1
Bye,"Goodbye, ""World""",2
Hi,"",World
Bye,"",Earth
"Hi","World"
"Bye","World"
Hi, World
Bye, World
)";
		String expected =
			LR"(HiWorld
ByeWorld
HiWorld1
ByeWorld2
HiWorld
ByeWorld
World
Earth
HiWorld
ByeEarth
HiWorld

ByeWorld
Hi"Hello,World"
Bye"Goodbye,World"
Hi"Hello ""World"""
Bye"Goodbye ""World"""
Hi"Hello, ""World"""1
Bye"Goodbye, ""World"""2
Hi""World
Bye""Earth
"Hi""World"
"Bye""World"
Hi World
Bye World
)";
		EXPECT_TRUE(esi.TransformText(0, text, {filepath[0]}, changed));
		EXPECT_STREQ(expected.c_str(), text.c_str());
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
		auto* parseResult0 = &PluginForFile::GetPluginPipelineItem(parseResult[0]);
		EXPECT_EQ(_T("<Automatic>"), parseResult0->name);
		EXPECT_EQ(0, parseResult0->args.size());
		EXPECT_EQ(0, parseResult0->quoteChar);
		pluginPipeline = PluginForFile::MakePluginPipeline(parseResult);
		EXPECT_EQ(_T("<Automatic>"), pluginPipeline);


		parseResult = PluginForFile::ParsePluginPipeline(_T(" <Automatic> "), errorMessage);
		EXPECT_TRUE(errorMessage.empty());
		EXPECT_EQ(1, parseResult.size());
		parseResult0 = &PluginForFile::GetPluginPipelineItem(parseResult[0]);
		EXPECT_EQ(_T("<Automatic>"), parseResult0->name);
		EXPECT_EQ(0, parseResult0->args.size());
		EXPECT_EQ(0, parseResult0->quoteChar);
		pluginPipeline = PluginForFile::MakePluginPipeline(parseResult);
		EXPECT_EQ(_T("<Automatic>"), pluginPipeline);

		parseResult = PluginForFile::ParsePluginPipeline(_T("'a b'"), errorMessage);
		EXPECT_TRUE(errorMessage.empty());
		EXPECT_EQ(1, parseResult.size());
		parseResult0 = &PluginForFile::GetPluginPipelineItem(parseResult[0]);
		EXPECT_EQ(_T("a b"), parseResult0->name);
		EXPECT_EQ(0, parseResult0->args.size());
		EXPECT_EQ('\'', parseResult0->quoteChar);
		pluginPipeline = PluginForFile::MakePluginPipeline(parseResult);
		EXPECT_EQ(_T("'a b'"), pluginPipeline);

		parseResult = PluginForFile::ParsePluginPipeline(_T("'a''b'"), errorMessage);
		EXPECT_TRUE(errorMessage.empty());
		EXPECT_EQ(1, parseResult.size());
		parseResult0 = &PluginForFile::GetPluginPipelineItem(parseResult[0]);
		EXPECT_EQ(_T("a'b"), parseResult0->name);
		EXPECT_EQ(0, parseResult0->args.size());
		EXPECT_EQ('\'', parseResult0->quoteChar);
		pluginPipeline = PluginForFile::MakePluginPipeline(parseResult);
		EXPECT_EQ(_T("'a''b'"), pluginPipeline);

		parseResult = PluginForFile::ParsePluginPipeline(_T("a' 'b:1"), errorMessage);
		EXPECT_TRUE(errorMessage.empty());
		EXPECT_EQ(1, parseResult.size());
		parseResult0 = &PluginForFile::GetPluginPipelineItem(parseResult[0]);
		EXPECT_EQ(_T("a b"), parseResult0->name);
		EXPECT_EQ(0b001, parseResult0->targetFlags);
		EXPECT_EQ(0, parseResult0->args.size());
		EXPECT_EQ('\'', parseResult0->quoteChar);
		pluginPipeline = PluginForFile::MakePluginPipeline(parseResult);
		EXPECT_EQ(_T("'a b':1"), pluginPipeline);

		parseResult = PluginForFile::ParsePluginPipeline(_T("a' '\"b\":1,3"), errorMessage);
		EXPECT_TRUE(errorMessage.empty());
		EXPECT_EQ(1, parseResult.size());
		parseResult0 = &PluginForFile::GetPluginPipelineItem(parseResult[0]);
		EXPECT_EQ(_T("a b"), parseResult0->name);
		EXPECT_EQ(0b101, parseResult0->targetFlags);
		EXPECT_EQ(0, parseResult0->args.size());
		EXPECT_EQ('"', parseResult0->quoteChar);
		pluginPipeline = PluginForFile::MakePluginPipeline(parseResult);
		EXPECT_EQ(_T("\"a b\":1,3"), pluginPipeline);

		parseResult = PluginForFile::ParsePluginPipeline(_T("\"a b"), errorMessage);
		EXPECT_TRUE(!errorMessage.empty());

		parseResult = PluginForFile::ParsePluginPipeline(_T("ExecFilterCommand dir c:\\"), errorMessage);
		EXPECT_TRUE(errorMessage.empty());
		EXPECT_EQ(1, parseResult.size());
		parseResult0 = &PluginForFile::GetPluginPipelineItem(parseResult[0]);
		EXPECT_EQ(_T("ExecFilterCommand"), parseResult0->name);
		EXPECT_EQ(_T("dir"), parseResult0->args[0]);
		EXPECT_EQ(_T("c:\\"), parseResult0->args[1]);
		EXPECT_EQ(0, parseResult0->quoteChar);
		pluginPipeline = PluginForFile::MakePluginPipeline(parseResult);
		EXPECT_EQ(_T("ExecFilterCommand dir c:\\"), pluginPipeline);

		parseResult = PluginForFile::ParsePluginPipeline(_T(" ExecFilterCommand  dir  c:\\ "), errorMessage);
		EXPECT_TRUE(errorMessage.empty());
		EXPECT_EQ(1, parseResult.size());
		parseResult0 = &PluginForFile::GetPluginPipelineItem(parseResult[0]);
		EXPECT_EQ(_T("ExecFilterCommand"), parseResult0->name);
		EXPECT_EQ(_T("dir"), parseResult0->args[0]);
		EXPECT_EQ(_T("c:\\"), parseResult0->args[1]);
		EXPECT_EQ(0, parseResult0->quoteChar);
		pluginPipeline = PluginForFile::MakePluginPipeline(parseResult);
		EXPECT_EQ(_T("ExecFilterCommand dir c:\\"), pluginPipeline);

		parseResult = PluginForFile::ParsePluginPipeline(_T("ExecFilterCommand  'dir  c:\\ '"), errorMessage);
		EXPECT_TRUE(errorMessage.empty());
		EXPECT_EQ(1, parseResult.size());
		parseResult0 = &PluginForFile::GetPluginPipelineItem(parseResult[0]);
		EXPECT_EQ(_T("ExecFilterCommand"), parseResult0->name);
		EXPECT_EQ(_T("dir  c:\\ "), parseResult0->args[0]);
		EXPECT_EQ('\'', parseResult0->quoteChar);
		pluginPipeline = PluginForFile::MakePluginPipeline(parseResult);
		EXPECT_EQ(_T("ExecFilterCommand 'dir  c:\\ '"), pluginPipeline);

		parseResult = PluginForFile::ParsePluginPipeline(_T("ExecFilterCommand  'dir  c:\\ ' "), errorMessage);
		EXPECT_TRUE(errorMessage.empty());
		EXPECT_EQ(1, parseResult.size());
		parseResult0 = &PluginForFile::GetPluginPipelineItem(parseResult[0]);
		EXPECT_EQ(_T("ExecFilterCommand"), parseResult0->name);
		EXPECT_EQ(_T("dir  c:\\ "), parseResult0->args[0]);
		EXPECT_EQ('\'', parseResult0->quoteChar);
		pluginPipeline = PluginForFile::MakePluginPipeline(parseResult);
		EXPECT_EQ(_T("ExecFilterCommand 'dir  c:\\ '"), pluginPipeline);

		parseResult = PluginForFile::ParsePluginPipeline(_T(" 'a b'  'dir  c:\\ ' "), errorMessage);
		EXPECT_TRUE(errorMessage.empty());
		EXPECT_EQ(1, parseResult.size());
		parseResult0 = &PluginForFile::GetPluginPipelineItem(parseResult[0]);
		EXPECT_EQ(_T("a b"), parseResult0->name);
		EXPECT_EQ(_T("dir  c:\\ "), parseResult0->args[0]);
		EXPECT_EQ('\'', parseResult0->quoteChar);
		pluginPipeline = PluginForFile::MakePluginPipeline(parseResult);
		EXPECT_EQ(_T("'a b' 'dir  c:\\ '"), pluginPipeline);

		parseResult = PluginForFile::ParsePluginPipeline(_T(" 'a b'  dir  c:\\  "), errorMessage);
		EXPECT_TRUE(errorMessage.empty());
		EXPECT_EQ(1, parseResult.size());
		parseResult0 = &PluginForFile::GetPluginPipelineItem(parseResult[0]);
		EXPECT_EQ(_T("a b"), parseResult0->name);
		EXPECT_EQ(_T("dir"), parseResult0->args[0]);
		EXPECT_EQ(_T("c:\\"), parseResult0->args[1]);
		EXPECT_EQ('\'', parseResult0->quoteChar);
		pluginPipeline = PluginForFile::MakePluginPipeline(parseResult);
		EXPECT_EQ(_T("'a b' 'dir' 'c:\\'"), pluginPipeline);

		parseResult = PluginForFile::ParsePluginPipeline(_T(" 'a b'  \"dir  c:\\ \" "), errorMessage);
		EXPECT_TRUE(errorMessage.empty());
		EXPECT_EQ(1, parseResult.size());
		parseResult0 = &PluginForFile::GetPluginPipelineItem(parseResult[0]);
		EXPECT_EQ(_T("a b"), parseResult0->name);
		EXPECT_EQ(_T("dir  c:\\ "), parseResult0->args[0]);
		EXPECT_EQ('"', parseResult0->quoteChar);
		pluginPipeline = PluginForFile::MakePluginPipeline(parseResult);
		EXPECT_EQ(_T("\"a b\" \"dir  c:\\ \""), pluginPipeline);

		parseResult = PluginForFile::ParsePluginPipeline(_T("ExecFilterCommand dir|MakeUpper"), errorMessage);
		EXPECT_TRUE(errorMessage.empty());
		EXPECT_EQ(2, parseResult.size());
		parseResult0 = &PluginForFile::GetPluginPipelineItem(parseResult[0]);
		EXPECT_EQ(_T("ExecFilterCommand"), parseResult0->name);
		EXPECT_EQ(_T("dir"), parseResult0->args[0]);
		EXPECT_EQ(0, parseResult0->quoteChar);
		auto* parseResult1 = &PluginForFile::GetPluginPipelineItem(parseResult[1]);
		EXPECT_EQ(_T("MakeUpper"), parseResult1->name);
		EXPECT_EQ(0, parseResult1->args.size());
		EXPECT_EQ(0, parseResult1->quoteChar);
		pluginPipeline = PluginForFile::MakePluginPipeline(parseResult);
		EXPECT_EQ(_T("ExecFilterCommand dir|MakeUpper"), pluginPipeline);

		parseResult = PluginForFile::ParsePluginPipeline(_T("ExecFilterCommand dir | MakeUpper "), errorMessage);
		EXPECT_TRUE(errorMessage.empty());
		EXPECT_EQ(2, parseResult.size());
		parseResult0 = &PluginForFile::GetPluginPipelineItem(parseResult[0]);
		EXPECT_EQ(_T("ExecFilterCommand"), parseResult0->name);
		EXPECT_EQ(_T("dir"), parseResult0->args[0]);
		EXPECT_EQ(0, parseResult0->quoteChar);
		parseResult1 = &PluginForFile::GetPluginPipelineItem(parseResult[1]);
		EXPECT_EQ(_T("MakeUpper"), parseResult1->name);
		EXPECT_EQ(0, parseResult1->args.size());
		EXPECT_EQ(0, parseResult1->quoteChar);
		pluginPipeline = PluginForFile::MakePluginPipeline(parseResult);
		EXPECT_EQ(_T("ExecFilterCommand dir|MakeUpper"), pluginPipeline);

		parseResult = PluginForFile::ParsePluginPipeline(_T(" ExecFilterCommand dir | MakeUpper "), errorMessage);
		EXPECT_TRUE(errorMessage.empty());
		EXPECT_EQ(2, parseResult.size());
		parseResult0 = &PluginForFile::GetPluginPipelineItem(parseResult[0]);
		EXPECT_EQ(_T("ExecFilterCommand"), parseResult0->name);
		EXPECT_EQ(_T("dir"), parseResult0->args[0]);
		EXPECT_EQ(0, parseResult0->quoteChar);
		parseResult1 = &PluginForFile::GetPluginPipelineItem(parseResult[1]);
		EXPECT_EQ(_T("MakeUpper"), parseResult1->name);
		EXPECT_EQ(0, parseResult1->args.size());
		EXPECT_EQ(0, parseResult1->quoteChar);
		pluginPipeline = PluginForFile::MakePluginPipeline(parseResult);
		EXPECT_EQ(_T("ExecFilterCommand dir|MakeUpper"), pluginPipeline);

		parseResult = PluginForFile::ParsePluginPipeline(_T(" ExecFilterCommand 'dir  c:\\ ' | MakeUpper "), errorMessage);
		EXPECT_TRUE(errorMessage.empty());
		EXPECT_EQ(2, parseResult.size());
		parseResult0 = &PluginForFile::GetPluginPipelineItem(parseResult[0]);
		EXPECT_EQ(_T("ExecFilterCommand"), parseResult0->name);
		EXPECT_EQ(_T("dir  c:\\ "), parseResult0->args[0]);
		EXPECT_EQ('\'', parseResult0->quoteChar);
		parseResult1 = &PluginForFile::GetPluginPipelineItem(parseResult[1]);
		EXPECT_EQ(_T("MakeUpper"), parseResult1->name);
		EXPECT_EQ(0, parseResult1->args.size());
		EXPECT_EQ(0, parseResult1->quoteChar);
		pluginPipeline = PluginForFile::MakePluginPipeline(parseResult);
		EXPECT_EQ(_T("ExecFilterCommand 'dir  c:\\ '|MakeUpper"), pluginPipeline);

		parseResult = PluginForFile::ParsePluginPipeline(_T("ExecFilterCommand 'dir c:\\'|ExecFilterCommand 'sort'"), errorMessage);
		EXPECT_TRUE(errorMessage.empty());
		EXPECT_EQ(2, parseResult.size());
		parseResult0 = &PluginForFile::GetPluginPipelineItem(parseResult[0]);
		EXPECT_EQ(_T("ExecFilterCommand"), parseResult0->name);
		EXPECT_EQ(_T("dir c:\\"), parseResult0->args[0]);
		EXPECT_EQ('\'', parseResult0->quoteChar);
		parseResult1 = &PluginForFile::GetPluginPipelineItem(parseResult[1]);
		EXPECT_EQ(_T("ExecFilterCommand"), parseResult1->name);
		EXPECT_EQ(_T("sort"), parseResult1->args[0]);
		EXPECT_EQ('\'', parseResult1->quoteChar);
		pluginPipeline = PluginForFile::MakePluginPipeline(parseResult);
		EXPECT_EQ(_T("ExecFilterCommand 'dir c:\\'|ExecFilterCommand 'sort'"), pluginPipeline);


		parseResult = PluginForFile::ParsePluginPipeline(_T(" ExecFilterCommand 'dir  c:\\ ' | ExecFilterCommand ' sort ' "), errorMessage);
		EXPECT_TRUE(errorMessage.empty());
		EXPECT_EQ(2, parseResult.size());
		parseResult0 = &PluginForFile::GetPluginPipelineItem(parseResult[0]);
		EXPECT_EQ(_T("ExecFilterCommand"), parseResult0->name);
		EXPECT_EQ(_T("dir  c:\\ "), parseResult0->args[0]);
		EXPECT_EQ('\'', parseResult0->quoteChar);
		parseResult1 = &PluginForFile::GetPluginPipelineItem(parseResult[1]);
		EXPECT_EQ(_T("ExecFilterCommand"), parseResult1->name);
		EXPECT_EQ(_T(" sort "), parseResult1->args[0]);
		EXPECT_EQ('\'', parseResult1->quoteChar);
		pluginPipeline = PluginForFile::MakePluginPipeline(parseResult);
		EXPECT_EQ(_T("ExecFilterCommand 'dir  c:\\ '|ExecFilterCommand ' sort '"), pluginPipeline);

		parseResult = PluginForFile::ParsePluginPipeline(_T("le:replace(Line, \"\\s+\", \"\")|le:1:replace(Line, \"|\", \"\")|le:1,3:replace(Line, \"\\s+\", \"\")|SelectLines:1,3 'abc'"), errorMessage);
		EXPECT_TRUE(errorMessage.empty());
		EXPECT_EQ(4, parseResult.size());
		auto& lineExpressionItem0 = std::get<PluginForFile::FilterExpressionPipelineItem>(parseResult[0]);
		EXPECT_EQ(_T("replace(Line, \"\\s+\", \"\")"), lineExpressionItem0.expression);

		auto& lineExpressionItem1 = std::get<PluginForFile::FilterExpressionPipelineItem>(parseResult[1]);
		EXPECT_EQ(0b001, lineExpressionItem1.targetFlags);
		EXPECT_EQ(_T("replace(Line, \"|\", \"\")"), lineExpressionItem1.expression);

		auto& lineExpressionItem2 = std::get<PluginForFile::FilterExpressionPipelineItem>(parseResult[2]);
		EXPECT_EQ(0b101, lineExpressionItem2.targetFlags);
		EXPECT_EQ(_T("replace(Line, \"\\s+\", \"\")"), lineExpressionItem2.expression);
		pluginPipeline = PluginForFile::MakePluginPipeline(parseResult);

		auto* parseResult3 = &PluginForFile::GetPluginPipelineItem(parseResult[3]);
		EXPECT_EQ(_T("SelectLines"), parseResult3->name);
		EXPECT_EQ(_T("abc"), parseResult3->args[0]);
		EXPECT_EQ('\'', parseResult3->quoteChar);

		pluginPipeline = PluginForFile::MakePluginPipeline(parseResult);
		EXPECT_EQ(_T("le:replace(Line, \"\\s+\", \"\")|le:1:replace(Line, \"|\", \"\")|le:1,3:replace(Line, \"\\s+\", \"\")|SelectLines:1,3 'abc'"), pluginPipeline);

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
		EXPECT_STREQ(_T(""), PluginForFile::MakeArguments(std::vector<String>{}, { _T("abc") }).c_str());
		EXPECT_STREQ(_T("\"\""), PluginForFile::MakeArguments(std::vector<String>{_T("")}, { _T("abc") }).c_str());
		EXPECT_STREQ(_T("%"), PluginForFile::MakeArguments(std::vector<String>{_T("%")}, {_T("abc")}).c_str());
		EXPECT_STREQ(_T("abc"), PluginForFile::MakeArguments(std::vector<String>{_T("%1")}, {_T("abc")}).c_str());
		EXPECT_STREQ(_T("%1"), PluginForFile::MakeArguments(std::vector<String>{_T("%%1")}, {_T("abc")}).c_str());
		EXPECT_STREQ(_T("%1a"), PluginForFile::MakeArguments(std::vector<String>{_T("%%1a")}, {_T("abc")}).c_str());
		EXPECT_STREQ(_T("\"\""), PluginForFile::MakeArguments(std::vector<String>{_T("%2")}, {_T("abc")}).c_str());
		EXPECT_STREQ(_T("%TIME%"), PluginForFile::MakeArguments(std::vector<String>{_T("%TIME%")}, {_T("abc")}).c_str());
		EXPECT_STREQ(_T("aaaabcaaa"), PluginForFile::MakeArguments(std::vector<String>{_T("aaa%1aaa")}, {_T("abc")}).c_str());
		EXPECT_STREQ(_T("abcdef"), PluginForFile::MakeArguments(std::vector<String>{_T("%1%2")}, { _T("abc"), _T("def") }).c_str());
	}

}  // namespace
