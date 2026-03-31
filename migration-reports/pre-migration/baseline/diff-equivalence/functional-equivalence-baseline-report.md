# Functional Equivalence Baseline Report (Pre-Migration)

## Status
- Corpus creation: Complete
- Diff algorithm entry-point inventory: Complete
- Regression tests added in C++ suite: Complete
- JUnit 5 parity harness template: Complete
- Golden baseline capture from WinMerge binaries: Blocked (Windows runtime missing)

## Delivered artifacts
- `Testing/Data/DiffAlgorithmCorpus/manifest.json`
- `Testing/Data/DiffAlgorithmCorpus/cases/*`
- `Testing/Data/DiffAlgorithmCorpus/generate-large-fixtures.sh`
- `migration-reports/pre-migration/baseline/diff-equivalence/diff-entry-points.json`
- `migration-reports/pre-migration/baseline/diff-equivalence/capture-winmerge-baseline.ps1`
- `migration-reports/pre-migration/baseline/diff-equivalence/golden-baseline.json`
- `migration-reports/pre-migration/baseline/diff-equivalence/performance-baseline-template.json`
- `migration-reports/pre-migration/baseline/diff-equivalence/junit5-harness-template/*`

## Regression test coverage added
- Unicode mutation handling
- Whitespace-only change behavior
- Binary file detection behavior
- Large-file comparison over 1 MiB with elapsed-time guardrail

## Blocker
The current execution environment cannot run `WinMergeU.exe` and `diff2winmergeU.exe`. Final golden baseline capture and performance baseline population must be executed on a Windows runner.
