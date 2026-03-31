param(
  [Parameter(Mandatory = $true)]
  [string]$WinMergeBuildDir
)

$ErrorActionPreference = 'Stop'

$repoRoot = (Resolve-Path "$PSScriptRoot/../../../..").Path
$corpusManifestPath = Join-Path $repoRoot 'Testing/Data/DiffAlgorithmCorpus/manifest.json'
$goldenOutputPath = Join-Path $repoRoot 'migration-reports/pre-migration/baseline/diff-equivalence/golden-baseline.json'
$perfOutputPath = Join-Path $repoRoot 'migration-reports/pre-migration/baseline/diff-equivalence/performance-baseline-template.json'

$diff2winmerge = Join-Path $WinMergeBuildDir 'diff2winmergeU.exe'
$winmerge = Join-Path $WinMergeBuildDir 'WinMergeU.exe'

if (-not (Test-Path $diff2winmerge)) { throw "Missing executable: $diff2winmerge" }
if (-not (Test-Path $winmerge)) { throw "Missing executable: $winmerge" }

$casesDir = Join-Path $repoRoot 'Testing/Data/DiffAlgorithmCorpus/cases'
$largeLeft = Join-Path $casesDir 'large_over_1mb.left.txt'
$largeRight = Join-Path $casesDir 'large_over_1mb.right.txt'

if (-not (Test-Path $largeLeft) -or -not (Test-Path $largeRight)) {
  New-Item -ItemType Directory -Path $casesDir -Force | Out-Null
  Set-Content -Path $largeLeft -Value '' -NoNewline
  Set-Content -Path $largeRight -Value '' -NoNewline
  $leftStream = [System.IO.StreamWriter]::new($largeLeft, $true, [System.Text.Encoding]::UTF8)
  $rightStream = [System.IO.StreamWriter]::new($largeRight, $true, [System.Text.Encoding]::UTF8)
  try {
    for ($i = 0; $i -lt 75000; $i++) {
      $leftLine = ('line:{0:D5}:stable' -f $i)
      $rightLine = if ($i -eq 74000) { ('line:{0:D5}:changed' -f $i) } else { ('line:{0:D5}:stable' -f $i) }
      $leftStream.WriteLine($leftLine)
      $rightStream.WriteLine($rightLine)
    }
  }
  finally {
    $leftStream.Dispose()
    $rightStream.Dispose()
  }
}

$manifest = Get-Content -Raw -Path $corpusManifestPath | ConvertFrom-Json
$results = @()

foreach ($case in $manifest.cases) {
  $left = Join-Path (Split-Path $corpusManifestPath -Parent) $case.left
  $right = Join-Path (Split-Path $corpusManifestPath -Parent) $case.right

  if (-not (Test-Path $left)) { throw "Missing case input: $left" }
  if (-not (Test-Path $right)) { throw "Missing case input: $right" }

  $sw = [System.Diagnostics.Stopwatch]::StartNew()
  & $diff2winmerge $winmerge /noprefs /noninteractive /minimize /ub -q $left $right | Out-Null
  $exitCode = $LASTEXITCODE
  $sw.Stop()

  $results += [PSCustomObject]@{
    id = $case.id
    left = $case.left
    right = $case.right
    expected = $case.expected
    exit_code = $exitCode
    elapsed_ms = $sw.ElapsedMilliseconds
  }
}

$golden = [PSCustomObject]@{
  generated_at = (Get-Date).ToString('s')
  status = 'captured'
  runtime = [PSCustomObject]@{
    platform = 'Windows'
    build_dir = $WinMergeBuildDir
    diff2winmerge = $diff2winmerge
    winmerge = $winmerge
  }
  cases = $results
}

$perfLarge = $results | Where-Object { $_.id -eq 'large_over_1mb' } | Select-Object -First 1
$perf = [PSCustomObject]@{
  generated_at = (Get-Date).ToString('s')
  status = 'captured'
  metrics = [PSCustomObject]@{
    large_over_1mb = [PSCustomObject]@{
      elapsed_ms = $perfLarge.elapsed_ms
      peak_working_set_bytes = $null
      notes = 'Peak working set must be captured from OS profiler during run.'
    }
  }
}

$golden | ConvertTo-Json -Depth 8 | Set-Content -Path $goldenOutputPath
$perf | ConvertTo-Json -Depth 8 | Set-Content -Path $perfOutputPath

Write-Host "Wrote $goldenOutputPath"
Write-Host "Wrote $perfOutputPath"
