#include "pch.h"

// command line examples
//   --exepath=c:\dev\winmerge\build\x64\debug\winmergeu.exe --all-languages --gtest_filter=*/OptionsDialogTest.*/* 
//  .\GUITests.exe --exepath=c:\dev\winmerge\build\x64\debug\winmergeu.exe --gtest_filter=*/FileTestInstance/FileTest*/*
int _tmain(int argc, TCHAR **argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
