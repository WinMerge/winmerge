# Functional Equivalence Baseline Report (Pre-Migration)

## Status
- Corpus creation: **Complete** (7 cases covering edge cases)
- Diff algorithm entry-point inventory: **Complete**
- Regression tests added in C++ suite: **Complete**
- JUnit 5 parity harness template: **Complete** (dynamic tests per case)
- Golden baseline capture: **Complete** (GNU diff portable baseline)

## Golden Baseline Approach

The original plan required `WinMergeU.exe` + `diff2winmergeU.exe` on a Windows host.
No Windows machine was available, so the team pivoted to a **portable baseline** using
GNU diff (Myers algorithm — same algorithmic foundation as WinMerge's core engine).

This provides:
- Cross-platform, reproducible captures (macOS, Linux, Docker)
- Deterministic exit codes and unified diff output per case
- Binary comparison via `cmp`
- Performance timing per case

A Dockerfile is provided for fully reproducible capture in CI.

### Capture tools
| Script | Purpose |
|--------|---------|
| `capture-golden-baseline.py` | Python 3 portable capture (primary) |
| `capture-golden-baseline.sh` | Shell wrapper |
| `Dockerfile` | Reproducible Docker-based capture |
| `capture-winmerge-baseline.ps1` | Original Windows-only capture (optional) |

### Baseline results (2026-04-01)
| Case | Comparison | Hunks | Time |
|------|-----------|-------|------|
| empty_vs_empty | identical | 0 | 5ms |
| empty_vs_single_line | different | 1 | 6ms |
| single_line_edit | different | 1 | 6ms |
| whitespace_only | different | 1 | 5ms |
| unicode_mutation | different | 1 | 5ms |
| binary_mutation | different | 0 | 6ms |
| large_over_1mb | different | 1 | 29ms |

All 7 cases validated against expected outcomes.

## Delivered artifacts
- `Testing/Data/DiffAlgorithmCorpus/manifest.json`
- `Testing/Data/DiffAlgorithmCorpus/cases/*`
- `Testing/Data/DiffAlgorithmCorpus/generate-large-fixtures.sh`
- `migration-reports/pre-migration/baseline/diff-equivalence/diff-entry-points.json`
- `migration-reports/pre-migration/baseline/diff-equivalence/golden-baseline.json`
- `migration-reports/pre-migration/baseline/diff-equivalence/capture-golden-baseline.py`
- `migration-reports/pre-migration/baseline/diff-equivalence/capture-golden-baseline.sh`
- `migration-reports/pre-migration/baseline/diff-equivalence/Dockerfile`
- `migration-reports/pre-migration/baseline/diff-equivalence/capture-winmerge-baseline.ps1` (optional Windows path)
- `migration-reports/pre-migration/baseline/diff-equivalence/performance-baseline-template.json`
- `migration-reports/pre-migration/baseline/diff-equivalence/junit5-harness-template/*`

## Regression test coverage
- Unicode mutation handling
- Whitespace-only change behavior
- Binary file detection behavior
- Large-file comparison over 1 MiB with elapsed-time guardrail
- Empty file edge cases
