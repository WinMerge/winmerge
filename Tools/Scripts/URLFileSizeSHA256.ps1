foreach ($l in $(Get-Content ..\..\Build\Releases\files.txt)) {
  $filename = Split-Path $l.Replace("`"","") -leaf
  if (-not ($filename -like '*-pdb.7z')) {
    $fullpath = Join-Path -Path ..\..\Build\Releases -ChildPath $filename
    $url = "https://downloads.sourceforge.net/winmerge/$filename"
    $sha256 = (certutil -hashfile $fullpath SHA256)[1]
    echo "('$url', $((Get-ChildItem $fullpath).Length), '$sha256');"
  }
}
