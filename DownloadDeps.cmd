@echo off
setlocal
set path="%ProgramFiles%\7-zip";"%ProgramFiles(x86)%\7-zip";%path%
7z > NUL
if not %ERRORLEVEL% == 0 (
  echo 7-Zip is not installed
  goto :eof
)
@echo on

set downloadsdir=%~dp0\build\WinMergeDownloadDeps
set urls_destdirs=^
https://github.com/WinMerge/winmerge/releases/download/winmerge_manual_another_build_tools_v2/winmerge_manual_another_build_tools_v2.zip!Docs\Manual\Tools ^
https://github.com/WinMerge/winmerge/releases/download/ShellExtension-1.18.7.0/ShellExtension-1.18.7.0.zip!Build ^
https://github.com/WinMerge/winmerge/releases/download/Merge7z2500.0/Merge7z2500.0-x86.zip!Build\x86\Release ^
https://github.com/WinMerge/winmerge/releases/download/Merge7z2500.0/Merge7z2500.0-x64.zip!Build\X64\Release ^
https://github.com/WinMerge/winmerge/releases/download/Merge7z2500.0/Merge7z2500.0-ARM.zip!Build\ARM64\Release ^
https://github.com/WinMerge/winmerge/releases/download/Merge7z2500.0/Merge7z2500.0-ARM64.zip!Build\ARM64\Release ^
https://github.com/WinMerge/frhed/releases/download/0.10904.2017/frhed-0.10904.2017.16-x86.zip!Build\x86\Release ^
https://github.com/WinMerge/frhed/releases/download/0.10904.2017/frhed-0.10904.2017.16-x64.zip!Build\x64\Release ^
https://github.com/WinMerge/frhed/releases/download/0.10904.2017/frhed-0.10904.2017.16-ARM.zip!Build\ARM\Release ^
https://github.com/WinMerge/frhed/releases/download/0.10904.2017/frhed-0.10904.2017.16-ARM64.zip!Build\ARM64\Release ^
https://github.com/WinMerge/winimerge/releases/download/v1.0.45/winimerge-1.0.45.0-x86.zip!Build\x86\Release ^
https://github.com/WinMerge/winimerge/releases/download/v1.0.45/winimerge-1.0.45.0-x64.zip!Build\x64\Release ^
https://github.com/WinMerge/winimerge/releases/download/v1.0.45/winimerge-1.0.45.0-ARM.zip!Build\ARM\Release ^
https://github.com/WinMerge/winimerge/releases/download/v1.0.45/winimerge-1.0.45.0-ARM64.zip!Build\ARM64\Release ^
https://github.com/WinMerge/winwebdiff/releases/download/v1.0.18/winwebdiff-1.0.18.0-x86.zip!Build\x86\Release ^
https://github.com/WinMerge/winwebdiff/releases/download/v1.0.18/winwebdiff-1.0.18.0-x64.zip!Build\x64\Release ^
https://github.com/WinMerge/winwebdiff/releases/download/v1.0.18/winwebdiff-1.0.18.0-ARM64.zip!Build\ARM64\Release ^
https://github.com/htacg/tidy-html5/releases/download/5.4.0/tidy-5.4.0-w32-mt-XP.zip!Build\tidy-html5 ^
https://github.com/htacg/tidy-html5/archive/refs/tags/5.4.0.zip!Build\tidy-html5 ^
https://github.com/jqlang/jq/releases/download/jq-1.7.1/jq-windows-i386.exe!Build\jq ^
https://github.com/jqlang/jq/archive/refs/tags/jq-1.7.1.zip!Build\jq ^
https://github.com/facebook/zstd/releases/download/v1.5.2/zstd-v1.5.2-win64.zip!Build\zstd ^
https://mirror.msys2.org/mingw/mingw32/mingw-w64-i686-md4c-0.5.2-1-any.pkg.tar.zst!Build\md4c ^
https://mirror.msys2.org/msys/i686/gcc-libs-10.2.0-1-i686.pkg.tar.zst!Build\msys2_tmp ^
https://mirror.msys2.org/msys/i686/msys2-runtime-3.2.0-14-i686.pkg.tar.zst!Build\msys2_tmp ^
https://mirror.msys2.org/msys/i686/patch-2.7.6-1-i686.pkg.tar.xz!Build\msys2_tmp ^
https://mirror.msys2.org/msys/i686/lemon-3.46.1-1-i686.pkg.tar.zst!Build\msys2_tmp ^
https://mirror.msys2.org/msys/i686/re2c-3.1-2-i686.pkg.tar.zst!Build\msys2_tmp ^
https://mirror.msys2.org/msys/i686/gcc-libs-13.3.0-1-i686.pkg.tar.zst!Build\msys2_tmp ^
http://www.magicnotes.com/steelbytes/SBAppLocale_ENG.zip!Docs\Manual\Tools

pushd "%~dp0"
mkdir %downloadsdir% 2> NUL

for %%p in (%urls_destdirs%) do (
  for /F "tokens=1,2 delims=!" %%u in ("%%p") do (
    if not exist %downloadsdir%\%%~nxu (
      powershell -command "[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; Invoke-WebRequest %%u -Outfile %downloadsdir%\%%~nxu"
    )
    if "%%~xu" == ".zip" (
      7z x %downloadsdir%\%%~nxu -aoa -o%%v
    ) else (
      if "%%~xu" == ".xz" (
        7z x %downloadsdir%\%%~nxu -so | 7z x -aoa -si -ttar -o%%v
      ) else (
        mkdir %%v > NUL
        if "%%~xu" == ".zst" (
          Build\zstd\zstd-v1.5.2-win64\zstd.exe -dc %downloadsdir%\%%~nxu | tar xf - -C %%v
        ) else (
          copy %downloadsdir%\%%~nxu %%v
        )
      )
    )
  )
)

for /d %%i in (build\tidy-html5\tidy-5.4.0-w32-mt-XP\*) do move %%i build\tidy-html5\

echo Copy msys2 files
mkdir Build\msys2\usr\bin 2> NUL
mkdir Build\msys2\usr\share 2> NUL
copy Build\msys2_tmp\usr\bin\patch.exe Build\msys2\usr\bin\
copy Build\msys2_tmp\usr\bin\msys-2.0.dll Build\msys2\usr\bin\
copy Build\msys2_tmp\usr\bin\msys-gcc_s-1.dll Build\msys2\usr\bin\
copy Build\msys2_tmp\usr\bin\msys-gcc_s-seh-1.dll Build\msys2\usr\bin\
copy "Build\msys2_tmp\usr\bin\msys-stdc++-6.dll" Build\msys2\usr\bin\
copy Build\msys2_tmp\usr\bin\lemon.exe Build\msys2\usr\bin\
copy Build\msys2_tmp\usr\bin\re2c.exe Build\msys2\usr\bin\
xcopy /s /y Build\msys2_tmp\usr\share\*.* Build\msys2\usr\share\
rmdir /q /s Build\msys2_tmp\ > NUL 2> NUL

for %%i in (x86 x64 ARM ARM64) do (
  for %%j in (Release Debug Test) do (
    echo **** %%i %%j ****
    mkdir Build\%%i\%%j\Merge7z 2> NUL
    mkdir Build\%%i\%%j\WinIMerge 2> NUL
    mkdir Build\%%i\%%j\WinWebDiff 2> NUL
    mkdir Build\%%i\%%j\Frhed 2> NUL
    mkdir Build\%%i\%%j\Filters 2> NUL
    mkdir Build\%%i\%%j\ColorSchemes 2> NUL
    mkdir Build\%%i\%%j\MergePlugins 2> NUL
    mkdir Build\%%i\%%j\Commands\jq 2> NUL
    mkdir Build\%%i\%%j\Commands\tidy-html5 2> NUL
    mkdir Build\%%i\%%j\Commands\msys2\usr\bin 2> NUL
    mkdir Build\%%i\%%j\Commands\md4c 2> NUL
    mkdir Build\%%i\%%j\Resources 2> NUL
    if not "Build\%%i\Release" == "Build\%%i\%%j" (
      echo ** Merge7z
      xcopy /s/y Build\%%i\Release\Merge7z Build\%%i\%%j\Merge7z\
      echo ** Frhed
      xcopy /s/y Build\%%i\Release\Frhed Build\%%i\%%j\Frhed\
      echo ** WinIMerge
      copy Build\%%i\Release\WinIMerge\WinIMergeLib.dll Build\%%i\%%j\WinIMerge\
      copy Build\%%i\Release\WinWebDiff\WinWebDiffLib.dll Build\%%i\%%j\WinWebDiff\ 2> NUL
    )
    echo ** jq
    copy Build\jq\jq-windows-i386.exe Build\%%i\%%j\Commands\jq\jq.exe
    copy Build\jq\jq-jq-1.7.1\COPYING Build\%%i\%%j\Commands\jq\
    echo ** tidy-html5
    copy Build\tidy-html5\bin\tidy.* Build\%%i\%%j\Commands\tidy-html5\
    copy Build\tidy-html5\tidy-html5-5.4.0\README\LICENSE.md Build\%%i\%%j\Commands\tidy-html5\
    echo ** md4c
    copy Build\md4c\mingw32\bin\*.exe Build\%%i\%%j\Commands\md4c\
    copy Build\md4c\mingw32\bin\*.dll Build\%%i\%%j\Commands\md4c\
    copy Build\md4c\mingw32\share\licenses\md4c\LICENSE.md Build\%%i\%%j\Commands\md4c\
    echo ** msys2
    copy Build\msys2\usr\bin\patch.exe Build\%%i\%%j\Commands\msys2\usr\bin\
    copy Build\msys2\usr\bin\msys-2.0.dll Build\%%i\%%j\Commands\msys2\usr\bin\
    copy Build\msys2\usr\bin\msys-gcc_s-1.dll Build\%%i\%%j\Commands\msys2\usr\bin\
    echo ** Commands
    xcopy /s/y Plugins\Commands Build\%%i\%%j\Commands
    echo ** Filters
    xcopy /s/y Filters Build\%%i\%%j\Filters\
    echo ** ColorSchemes
    xcopy /s/y ColorSchemes Build\%%i\%%j\ColorSchemes\
    echo ** Plugins
    xcopy /s/y Plugins\dlls\*.sct Build\%%i\%%j\MergePlugins\
    xcopy /s/y Plugins\Plugins.xml Build\%%i\%%j\MergePlugins\
    copy Plugins\dlls\%%i\*.dll Build\%%i\%%j\MergePlugins\
    echo ** ShellExtension
    xcopy /s/y Build\ShellExtension\WinMergeContextMenuPackage.msix Build\%%i\%%j
    echo ** Resources
    copy Src\res\splash.png Build\%%i\%%j\Resources\
  )
)

popd
