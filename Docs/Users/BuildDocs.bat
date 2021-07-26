pushd %~dp0
for %%i in (ChangeLog*.md ReleaseNotes*.md) do (
  pandoc -f gfm -s --self-contained -t html5 -c %~dp0\github-pandoc.css %%i -o %%~ni.html
)
popd

