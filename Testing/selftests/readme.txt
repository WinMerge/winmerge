selftests

The testdiff.pl perl script runs WinMerge through a series of diff tests,
using test files in the subdirectories: m for files with mac line endings,
u for unix line endings, and w for files with windows line endings.

This file invokes the Unicode Debug compiled versions of diff2winmergeU.exe
and WinMergeU.exe, via relative paths.
