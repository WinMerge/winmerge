# AMP-12 Release Record (Phase 2b Wave 3b)

## Scope

- Issue: `AMP-12`
- Phase: Phase 2b Wave 3b — Compare Engines Port
- Implementation commit validated by QA: `62ce3b0f7`
- QA artifact commit: `ba3792ebb`

## QA Handoff

- QA gate: PASS
- Health score: 96/100
- Regression tests: 55 passed, 0 failed
- Benchmark deltas:
  - build time: -10.48%
  - compare-engine tests: +7.91%
  - artifacts: jar +0.33%, sources jar +0.41%

Artifacts:

- `migration-reports/qa/AMP-12/qa-report.md`
- `migration-reports/qa/AMP-12/headless-results.json`
- `migration-reports/qa/AMP-12/benchmark-comparison.json`
- `migration-reports/qa/AMP-12/post-62ce3b0f7-summary.png`
- `migration-reports/qa/AMP-12/post-62ce3b0f7-compare-engines.png`

## Release Verification

- Full build gate: `./gradlew clean test build` (PASS)
- Core regression rerun: `./gradlew :winmerge-core:test --rerun-tasks` (PASS)
- Test coverage audit (test inventory): 14 suites / 55 tests, 0 failures
- Compare-engine focused coverage: 12 tests in `CompareEnginesTest`

## Versioning and Release Docs

- Project version bumped to `0.1.1` (`build.gradle.kts`)
- WinMerge version metadata bumped to `2.16.56.0` (`Version.h`, `SetVersion.cmd`)
- Changelog updated: `Docs/Users/ChangeLog.md`
- Release documentation updated:
  - `README.md`
  - `ARCHITECTURE.md`
  - `CONTRIBUTING.md`

## Canary / Production

Pending release-window confirmation from Engineering Manager before merge/deploy.
Canary loop output and final go/no-go summary are appended after deployment.
