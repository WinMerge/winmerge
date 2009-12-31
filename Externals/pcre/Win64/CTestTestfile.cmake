# CMake generated Testfile for 
# Source directory: D:/Work/WinMerge/WinMerge_SVN/Externals/pcre
# Build directory: D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win64
# 
# This file includes the relevent testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
ADD_TEST(pcre_test "cmd" "/C" "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/RunTest.bat")
ADD_TEST(pcrecpp_test "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win64/DEBUG/pcrecpp_unittest.exe")
ADD_TEST(pcre_scanner_test "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win64/DEBUG/pcre_scanner_unittest.exe")
ADD_TEST(pcre_stringpiece_test "D:/Work/WinMerge/WinMerge_SVN/Externals/pcre/Win64/DEBUG/pcre_stringpiece_unittest.exe")
