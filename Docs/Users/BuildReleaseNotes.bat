pandoc -f gfm -s --self-contained -t html5 -c %~dp0\github-pandoc.css %~dp0\ReleaseNotes.md -o %~dp0\ReleaseNotes.html
