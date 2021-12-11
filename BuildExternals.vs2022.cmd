cd /d "%~dp0"

for %%i in (Externals\sevenzip Externals\winimerge Externals\frhed ArchiveSupport\Merge7z ShellExtension Plugins) do (
  pushd %%i
  rem call BuildBin.vs2022.cmd %*
  popd
)

if "%1" == "ARM" (
  mkdir Build\ARM\Release\Merge7z 2> NUL
  mkdir Build\ARM\Release\Merge7z\Lang 2> NUL
  mkdir Build\ARM\Release\WinIMerge 2> NUL
  mkdir Build\ARM\Release\frhed\Docs 2> NUL
  mkdir Build\ARM\Release\frhed\Languages 2> NUL
  mkdir Build\ARM\Release\ColorSchemes 2> NUL
  mkdir Build\ARM\Release\Filters 2> NUL
  mkdir Build\ARM\Release\MergePlugins 2> NUL

  mkdir Build\ARM\Merge7z 2> NUL
  mkdir Build\ARM\Merge7z\Lang 2> NUL
  mkdir Build\ARM\frhed\Docs 2> NUL
  mkdir Build\ARM\frhed\Languages 2> NUL
  mkdir Build\WinIMerge\binARM 2> NUL

  copy Build\ARM\Release\ShellExtensionARM.dll Build\ShellExtension\

  copy Externals\sevenzip\CPP\7zip\Bundles\Format7zF\ARM\7z.dll Build\ARM\Release\Merge7z\
  copy Build\ARM64\Release\Merge7z\*.txt Build\ARM\Release\Merge7z\
  copy Build\ARM64\Release\Merge7z\Lang\*.txt Build\ARM\Release\Merge7z\Lang\
  copy Externals\sevenzip\CPP\7zip\Bundles\Format7zF\ARM\7z.dll Build\ARM\Merge7z\
  copy Build\ARM64\Release\Merge7z\*.txt Build\ARM\Merge7z\
  copy Build\ARM64\Release\Merge7z\Lang\*.txt Build\ARM\Merge7z\Lang\

  copy Externals\frhed\Build\frhed\ARM\Release\hekseditU.dll Build\ARM\frhed\
  copy Externals\frhed\Build\frhed\ARM\Release\Docs\*.txt Build\ARM\frhed\Docs\
  copy Externals\frhed\Build\frhed\ARM\Release\Languages\*.po Build\ARM\frhed\Languages\
  copy Externals\frhed\Build\frhed\ARM\Release\hekseditU.dll Build\ARM\Release\frhed\
  copy Externals\frhed\Build\frhed\ARM\Release\Docs\*.txt Build\ARM\Release\frhed\Docs\
  copy Externals\frhed\Build\frhed\ARM\Release\Languages\*.po Build\ARM\Release\frhed\Languages\

  copy Externals\winimerge\Build\ARM\Release\WinIMergeLib.dll Build\WinIMerge\binARM\
  copy Externals\winimerge\Build\ARM\Release\WinIMergeLib.dll Build\ARM\Release\WinIMerge\
  copy Externals\winimerge\*.txt Build\WinIMerge\binARM\
  copy Externals\winimerge\*.txt Build\ARM\Release\WinIMerge\
)

