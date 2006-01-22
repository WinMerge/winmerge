diff2winmerge.exe & diff2winmergeU.exe

diff2winmerge(U) is a utility for invoking WinMerge with gnu diff style options



For example, running

diff2winmerge path/WinMerge.exe -bi /minimize /noninteractive

will invoke WinMerge in this fashion:

path/WinMerge.exe /ignorews:1 /ignorecase:1 /minimize /noninteractive


Note that only gnu diff options are translated; other options (such as "/minimize"
above) are passed directly through to WinMerge.



diff2winmergeU.exe is compiled as UCS-2 (Unicode), for use with WinMergeU.exe.

