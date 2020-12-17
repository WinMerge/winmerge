pushd ..\..\Src
Get-ChildItem -Recurse *.cpp | Select-String -Pattern "_\((""[^""]*"")\)" | foreach { $_.Matches.Groups[1].Value } | foreach { if (Select-String -SimpleMatch -Pattern $_ -Path Merge.rc -quiet) { } else { echo $_ } }
popd
