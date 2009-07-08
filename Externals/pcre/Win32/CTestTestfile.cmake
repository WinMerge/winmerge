# CMake generated Testfile for 
# Source directory: G:/WinMerge/WinMerge_SVN/Externals/pcre
# Build directory: G:/WinMerge/WinMerge_SVN/Externals/pcre/Win32
# 
# This file replicates the SUBDIRS() and ADD_TEST() commands from the source
# tree CMakeLists.txt file, skipping any SUBDIRS() or ADD_TEST() commands
# that are excluded by CMake control structures, i.e. IF() commands.
ADD_TEST(pcre_test "cmd" "/C" "G:/WinMerge/WinMerge_SVN/Externals/pcre/RunTest.bat")
ADD_TEST(pcrecpp_test "G:/WinMerge/WinMerge_SVN/Externals/pcre/Win32/DEBUG/pcrecpp_unittest.exe")
ADD_TEST(pcre_scanner_test "G:/WinMerge/WinMerge_SVN/Externals/pcre/Win32/DEBUG/pcre_scanner_unittest.exe")
ADD_TEST(pcre_stringpiece_test "G:/WinMerge/WinMerge_SVN/Externals/pcre/Win32/DEBUG/pcre_stringpiece_unittest.exe")
