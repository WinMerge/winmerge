
param(
    [string]$Version,
    [string]$ReleaseDate
)

$ErrorActionPreference = 'Stop'

# ------------------------------------------------------------
# Paths
# ------------------------------------------------------------

$scriptDir = $PSScriptRoot
$stableRoot = [System.IO.Path]::GetFullPath(
    (Join-Path $scriptDir '..\..')
)
$devRoot = Split-Path $stableRoot -Parent

$releaseDir = Join-Path $stableRoot 'build\releases'
$pagePath = Join-Path $devRoot 'website\htdocs\engine\page.inc'

if (-not (Test-Path -LiteralPath $releaseDir -PathType Container)) {
    throw "Release directory not found: $releaseDir"
}

if (-not (Test-Path -LiteralPath $pagePath -PathType Leaf)) {
    throw "page.inc not found: $pagePath"
}

# ------------------------------------------------------------
# Get and validate version
# ------------------------------------------------------------

if ([string]::IsNullOrWhiteSpace($Version)) {
    $Version = Read-Host 'Version number (e.g. 2.16.60.2)'
}

if ($Version -notmatch '^\d+\.\d+\.\d+(\.\d+)?$') {
    throw "Invalid version number: $Version"
}

# ------------------------------------------------------------
# Get and validate release date
# ------------------------------------------------------------

if ([string]::IsNullOrWhiteSpace($ReleaseDate)) {
    $ReleaseDate = Read-Host 'Release date (yyyy-MM-dd)'
}

$dateValue = [datetime]::MinValue

if (-not [datetime]::TryParseExact(
    $ReleaseDate,
    'yyyy-MM-dd',
    [System.Globalization.CultureInfo]::InvariantCulture,
    [System.Globalization.DateTimeStyles]::None,
    [ref]$dateValue
)) {
    throw "Invalid release date: $ReleaseDate (expected yyyy-MM-dd)"
}

# ------------------------------------------------------------
# Read page.inc
# ------------------------------------------------------------

$bytes = [System.IO.File]::ReadAllBytes($pagePath)

# Detect UTF-8 BOM
$hasBom = (
    $bytes.Length -ge 3 -and
    $bytes[0] -eq 0xEF -and
    $bytes[1] -eq 0xBB -and
    $bytes[2] -eq 0xBF
)

$offset = if ($hasBom) { 3 } else { 0 }

$text = [System.Text.Encoding]::UTF8.GetString(
    $bytes,
    $offset,
    $bytes.Length - $offset
)

# Detect original newline style
$newline = if ($text.Contains("`r`n")) {
    "`r`n"
}
else {
    "`n"
}

# Normalize all line endings
$text = $text -replace "`r`n|`n|`r", $newline

# ------------------------------------------------------------
# Extract constructor
# ------------------------------------------------------------

$constructorPattern = '(?s)(public function __construct\(\)\s*\{.*?)(\r?\n    \})'

$constructorMatch = [regex]::Match($text, $constructorPattern)

if (-not $constructorMatch.Success) {
    throw 'Could not find the Stable Release constructor.'
}

# Keep the closing brace separately
$constructor = $constructorMatch.Groups[1].Value
$constructorEnd = $constructorMatch.Groups[2].Value

# ------------------------------------------------------------
# Get current version
# ------------------------------------------------------------

$versionMatch = [regex]::Match(
    $constructor,
    "setVersionNumber\('([^']+)'\)"
)

if (-not $versionMatch.Success) {
    throw 'Could not find the current version number.'
}

$oldVersion = $versionMatch.Groups[1].Value

Write-Host ''
Write-Host "Current version : $oldVersion"
Write-Host "New version     : $Version"
Write-Host "Release date    : $ReleaseDate"
Write-Host ''

# ------------------------------------------------------------
# File name mapping
# ------------------------------------------------------------

$files = [ordered]@{
    'setup.exe'          = "WinMerge-$Version-Setup.exe"
    'setup64.exe'        = "WinMerge-$Version-x64-Setup.exe"
    'setup64peruser.exe' = "WinMerge-$Version-x64-PerUser-Setup.exe"
    'setuparm64.exe'     = "WinMerge-$Version-ARM64-Setup.exe"
    'exe.zip'            = "winmerge-$Version-exe.zip"
    'exe64.zip'          = "winmerge-$Version-x64-exe.zip"
    'exearm64.zip'       = "winmerge-$Version-ARM64-exe.zip"
    'src.7z'             = "winmerge-$Version-full-src.7z"
}

# ------------------------------------------------------------
# Get file sizes and SHA-256 hashes
# ------------------------------------------------------------

$downloadInfo = @{}

foreach ($key in $files.Keys) {
    $fileName = $files[$key]
    $filePath = Join-Path $releaseDir $fileName

    if (-not (Test-Path -LiteralPath $filePath -PathType Leaf)) {
        throw "Release file not found: $filePath"
    }

    $file = Get-Item -LiteralPath $filePath
    $hash = (Get-FileHash -LiteralPath $filePath -Algorithm SHA256).Hash.ToLowerInvariant()

    $downloadInfo[$key] = @{
        Size = $file.Length
        Hash = $hash
    }

    Write-Host "$key"
    Write-Host "  File: $fileName"
    Write-Host "  Size: $($file.Length)"
    Write-Host "  SHA256: $hash"
}

# ------------------------------------------------------------
# Update version number and URLs
# ------------------------------------------------------------

$constructor = $constructor.Replace($oldVersion, $Version)

# ------------------------------------------------------------
# Update release date
# ------------------------------------------------------------

# ------------------------------------------------------------
# Update release date
# ------------------------------------------------------------

$datePattern = @'
(?m)(\$this->_stablerelease->setDate\(')[^']+('\);)
'@

$dateMatch = [regex]::Match($constructor, $datePattern)

if (-not $dateMatch.Success) {
    throw 'Could not find setDate() in the constructor.'
}

$replacement = (
    $dateMatch.Groups[1].Value +
    $ReleaseDate +
    $dateMatch.Groups[2].Value
)

$constructor = $constructor.Remove(
    $dateMatch.Index,
    $dateMatch.Length
).Insert(
    $dateMatch.Index,
    $replacement
)

# ------------------------------------------------------------
# Update download information
# ------------------------------------------------------------

foreach ($key in $files.Keys) {
    $escapedKey = [regex]::Escape($key)

    # Do not allow the match to cross into another addDownload() call.
    $pattern = @'
(?s)(\$this->_stablerelease->addDownload\('__KEY__'\s*,(?:(?!\$this->_stablerelease->addDownload).)*?)(,\s*)(\d+)(\s*,\s*')([0-9a-fA-F]{64})('\s*\);)
'@

    $pattern = $pattern.Replace('__KEY__', $escapedKey)

    $match = [regex]::Match($constructor, $pattern)

    if (-not $match.Success) {
        throw "Could not find addDownload() entry for: $key"
    }

    $size = $downloadInfo[$key].Size
    $hash = $downloadInfo[$key].Hash

    # Preserve the original separators and formatting.
    $replacement = (
        $match.Groups[1].Value +
        $match.Groups[2].Value +
        $size +
        $match.Groups[4].Value +
        $hash +
        $match.Groups[6].Value
    )

    $constructor = $constructor.Remove(
        $match.Index,
        $match.Length
    ).Insert(
        $match.Index,
        $replacement
    )
}

# ------------------------------------------------------------
# Replace constructor in the original text
# ------------------------------------------------------------

$newConstructor = $constructor + $constructorEnd

$text = $text.Remove(
    $constructorMatch.Index,
    $constructorMatch.Length
).Insert(
    $constructorMatch.Index,
    $newConstructor
)

# ------------------------------------------------------------
# Normalize line endings again
# ------------------------------------------------------------

$text = $text -replace "`r`n|`n|`r", $newline

# ------------------------------------------------------------
# Preview and confirmation
# ------------------------------------------------------------

Write-Host ''
Write-Host 'The following changes will be applied:'
Write-Host "  Version     : $oldVersion -> $Version"
Write-Host "  Release date: $ReleaseDate"
Write-Host "  File        : $pagePath"
Write-Host "  Newline     : $(if ($newline -eq "`r`n") { 'CRLF' } else { 'LF' })"
Write-Host ''

$answer = Read-Host 'Update page.inc? (Y/N)'

if ($answer -notmatch '^[Yy]$') {
    Write-Host 'Update cancelled.'
    exit 0
}

# ------------------------------------------------------------
# Backup and write
# ------------------------------------------------------------

$backupPath = "$pagePath.bak"

Copy-Item -LiteralPath $pagePath -Destination $backupPath -Force

$encoding = [System.Text.UTF8Encoding]::new($hasBom)

[System.IO.File]::WriteAllText(
    $pagePath,
    $text,
    $encoding
)

Write-Host ''
Write-Host 'page.inc has been updated successfully.'
Write-Host "Backup: $backupPath"
