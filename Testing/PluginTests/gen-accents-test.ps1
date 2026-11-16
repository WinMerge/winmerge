# gen-accents-test.ps1  (works with Windows PowerShell 5.1 and PowerShell 7)
$ErrorActionPreference = 'Stop'
try { [Text.Encoding]::RegisterProvider([Text.CodePagesEncodingProvider]::Instance) } catch {}

# Expand "{E9}" -> U+00E9, "{1F600}" -> U+1F600 (keeps this script ASCII-only)
function U([string]$s) {
    [regex]::Replace($s, '\{([0-9A-Fa-f]{2,6})\}', { param($m)
        [char]::ConvertFromUtf32([Convert]::ToInt32($m.Groups[1].Value, 16)) })
}

# Left, Right  (expected after IgnoreAccents: = identical / != different)
$pairs = @(
  @('caf{E9}',               'cafe'),             # =  basic precomposed
  @('cafe{301}',             'cafe'),             # =  decomposed combining mark
  @('caf{E9}',               'cafe{301}'),        # =  precomposed vs decomposed
  @('Caf{E9}',               'cafe'),             # != case must be preserved
  @('cafe',                  'cafes'),            # != ordinary difference must remain
  @('S{E3}o Tom{E9}',        'Sao Tome'),         # =  multiple accents
  @('{C5}ngstr{F6}m',        'Angstrom'),         # =  multiple accents
  @('Vi{1EC7}t Nam',         'Viet Nam'),         # =  multiple combining marks on one letter
  @('{3AC}',                 '{3B1}'),            # =  Greek
  @('{439}',                 '{438}'),            # =  Cyrillic (decide whether this is intended)
  @('{F8}',                  'o'),                # != not decomposable by NFD
  @('{142}',                 'l'),                # != not decomposable by NFD
  @('{304C}',                '{304B}'),           # != must not strip dakuten
  @('{30D1}',                '{30CF}'),           # != must not strip handakuten
  @('{D55C}{AE00}',          '{D55C}{AE00}'),     # =  Hangul stays intact
  @('{D55C}{AE00}',          '{D558}{AE00}'),     # != Hangul real difference
  @('{1F600}{E9}',           '{1F600}e'),         # =  surrogate pair handling
  @('{301}abc',              'abc'),              # =  lone combining mark at line start
  @('{928}{93F}',            '{928}')             # != must not strip Indic vowel signs
)

# Build one file body; each pair is followed by a separator line
# so that difference blocks do not merge
function Build($idx, $indices) {
    $lines = foreach ($i in $indices) { (U $pairs[$i][$idx]); '-----' }
    ($lines -join "`r`n") + "`r`n"
}

$all    = 0..($pairs.Count - 1)
$latin  = 0,3,4,5,6          # pairs representable in CP1252
$sjis   = 4,12,13            # pairs representable in Shift-JIS

$encs = [ordered]@{
  'utf8'    = New-Object Text.UTF8Encoding($false)
  'utf8bom' = New-Object Text.UTF8Encoding($true)
  'utf16le' = New-Object Text.UnicodeEncoding($false, $true)
  'utf16be' = New-Object Text.UnicodeEncoding($true,  $true)
}
$dir = Join-Path $PSScriptRoot 'accents-test'
New-Item -ItemType Directory -Force $dir | Out-Null

# Unicode encodings
foreach ($name in $encs.Keys) {
  [IO.File]::WriteAllText("$dir\left_$name.txt",  (Build 0 $all), $encs[$name])
  [IO.File]::WriteAllText("$dir\right_$name.txt", (Build 1 $all), $encs[$name])
}

# Legacy code pages (Latin-only / Japanese-only subsets)
$cp1252 = [Text.Encoding]::GetEncoding(1252)
$cp932  = [Text.Encoding]::GetEncoding(932)
[IO.File]::WriteAllText("$dir\left_cp1252.txt",  (Build 0 $latin), $cp1252)
[IO.File]::WriteAllText("$dir\right_cp1252.txt", (Build 1 $latin), $cp1252)
[IO.File]::WriteAllText("$dir\left_sjis.txt",    (Build 0 $sjis),  $cp932)
[IO.File]::WriteAllText("$dir\right_sjis.txt",   (Build 1 $sjis),  $cp932)

# Edge cases: no trailing EOL, empty file
[IO.File]::WriteAllText("$dir\left_noeol.txt",  (U 'caf{E9}'), $encs['utf8'])
[IO.File]::WriteAllText("$dir\right_noeol.txt", 'cafe',        $encs['utf8'])
[IO.File]::WriteAllText("$dir\empty.txt", '', $encs['utf8'])

# Performance test: repeat all pairs many times (adjust the count to change the size)
$chunkL = (Build 0 $all); $chunkR = (Build 1 $all)
$sbL = New-Object Text.StringBuilder; $sbR = New-Object Text.StringBuilder
1..20000 | ForEach-Object { [void]$sbL.Append($chunkL); [void]$sbR.Append($chunkR) }
[IO.File]::WriteAllText("$dir\left_big.txt",  $sbL.ToString(), $encs['utf8'])
[IO.File]::WriteAllText("$dir\right_big.txt", $sbR.ToString(), $encs['utf8'])

"Done: $dir"