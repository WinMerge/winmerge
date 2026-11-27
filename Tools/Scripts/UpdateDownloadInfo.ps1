
$ErrorActionPreference = 'Stop'

$scriptDir = $PSScriptRoot

$releaseDir = Join-Path $scriptDir '..\..\build\releases'
$pagePath = Join-Path $scriptDir '..\..\..\website\htdocs\engine\page.inc'

# Convert to absolute paths
$releaseDir = [System.IO.Path]::GetFullPath($releaseDir)
$pagePath = [System.IO.Path]::GetFullPath($pagePath)

# Read page.inc and detect UTF-8 BOM.
$originalBytes = [System.IO.File]::ReadAllBytes($pagePath)
$hasBom = $originalBytes.Length -ge 3 -and
    $originalBytes[0] -eq 0xEF -and
    $originalBytes[1] -eq 0xBB -and
    $originalBytes[2] -eq 0xBF

$utf8 = New-Object System.Text.UTF8Encoding($false)
$offset = if ($hasBom) { 3 } else { 0 }

$text = $utf8.GetString(
    $originalBytes,
    $offset,
    $originalBytes.Length - $offset
)

# Get the release version from page.inc.
$versionMatch = [regex]::Match(
    $text,
    "setVersionNumber\('([^']+)'\)"
)

if (-not $versionMatch.Success) {
    throw 'Could not find setVersionNumber() in page.inc.'
}

$version = $versionMatch.Groups[1].Value
Write-Host "Release version: $version"

# page.inc key -> release filename
$files = [ordered]@{
    'setup.exe'         = "WinMerge-$version-Setup.exe"
    'setup64.exe'       = "WinMerge-$version-x64-Setup.exe"
    'setup64peruser.exe'= "WinMerge-$version-x64-PerUser-Setup.exe"
    'setuparm64.exe'    = "WinMerge-$version-ARM64-Setup.exe"
    'exe.zip'           = "winmerge-$version-exe.zip"
    'exe64.zip'         = "winmerge-$version-x64-exe.zip"
    'exearm64.zip'      = "winmerge-$version-ARM64-exe.zip"
    'src.7z'            = "winmerge-$version-full-src.7z"
}

$updated = 0

foreach ($key in $files.Keys) {
    $filePath = Join-Path $releaseDir $files[$key]

    if (-not (Test-Path -LiteralPath $filePath -PathType Leaf)) {
        throw "Release file not found: $filePath"
    }

    $file = Get-Item -LiteralPath $filePath
    $size = $file.Length
    $hash = (Get-FileHash -LiteralPath $filePath -Algorithm SHA256).Hash.ToLowerInvariant()

    $escapedKey = [regex]::Escape($key)

    # Find the addDownload() line for this key.
    $linePattern = "(?m)^.*addDownload\('$escapedKey'.*$"

    $lineMatch = [regex]::Match($text, $linePattern)

    if (-not $lineMatch.Success) {
        throw "Could not find addDownload() entry for '$key'."
    }

    # Remove Windows line-ending characters for matching.
    $line = $lineMatch.Value.TrimEnd("`r", "`n")

    # Match the size and SHA-256 at the end of addDownload().
    $tailPattern = ",\s*(\d+)\s*,\s*'([0-9a-fA-F]{64})'\s*\);\s*$"

    $tailMatch = [regex]::Match($line, $tailPattern)

    if (-not $tailMatch.Success) {
        Write-Host "Failed to match the end of the line:"
        Write-Host $line
        throw "Could not find size/hash arguments for '$key'."
    }

    # Replace only the size and hash.
    $newLine = $line.Substring(0, $tailMatch.Index) +
        ", $size, '$hash');"

    if ($newLine -eq $lineMatch.Value) {
        throw "Could not replace size/hash for '$key'."
    }

    $text = $text.Replace($lineMatch.Value, $newLine)

    Write-Host ("{0,-20} {1,12} bytes  {2}" -f $key, $size, $hash)
    $updated++
}

if ($updated -eq 0) {
    throw 'No entries were updated.'
}

# Create a backup before modifying page.inc.
$backupPath = "$pagePath.bak"

Copy-Item -LiteralPath $pagePath -Destination $backupPath -Force

# Preserve the original BOM setting.
$newBytes = $utf8.GetBytes($text)

if ($hasBom) {
    $bom = [byte[]](0xEF, 0xBB, 0xBF)
    $newBytes = $bom + $newBytes
}

[System.IO.File]::WriteAllBytes($pagePath, $newBytes)

Write-Host ""
Write-Host "Updated $updated download entries."
Write-Host "Backup: $backupPath"
