#include "pch.h"

// command line example
//   --exepath=c:\dev\winmerge\build\x64\debug\winmergeu.exe --all-languages --gtest_filter=OptionsDialogTest.* 
int _tmain(int argc, TCHAR **argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
