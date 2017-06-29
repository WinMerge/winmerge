#include "StdAfx.h"
#ifdef TEST_WINMERGE
#include "TestMain.h"
#include "OptionsMgr.h"
#include "../../Externals/gtest/src/gtest-all.cc"

namespace WinMergeTest
{

void TestAll()
{
	int argc;
	wchar_t **argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	testing::InitGoogleTest(&argc, argv);
	for (const auto& key : GetOptionsMgr()->GetNameList())
		GetOptionsMgr()->Reset(key);
	RUN_ALL_TESTS();
}

}
#endif
