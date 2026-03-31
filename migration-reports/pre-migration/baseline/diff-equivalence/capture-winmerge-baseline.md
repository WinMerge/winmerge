# Capture WinMerge Baseline (Windows)

## Prerequisites
- Windows machine or CI runner
- Built binaries in one directory:
  - `diff2winmergeU.exe`
  - `WinMergeU.exe`

## Run
```powershell
pwsh migration-reports/pre-migration/baseline/diff-equivalence/capture-winmerge-baseline.ps1 -WinMergeBuildDir "C:\\path\\to\\build\\MergeUnicodeRelease"
```

## Outputs
- `migration-reports/pre-migration/baseline/diff-equivalence/golden-baseline.json`
- `migration-reports/pre-migration/baseline/diff-equivalence/performance-baseline-template.json`
