$version = $(git describe --abbrev=0).Replace("v", "")
foreach ($l in $(Get-Content ..\..\Build\Releases\files.txt)) {
  $filename = Split-Path $l.Replace("`"","") -leaf
  $fullpath = Join-Path -Path ..\..\Build\Releases -ChildPath $filename
  $url = "https://github.com/WinMerge/winmerge/releases/download/v$version/$filename"
  $sha256 = (certutil -hashfile $fullpath SHA256)[1]
  echo "'$url', $((Get-ChildItem $fullpath).Length), '$sha256'"
}
