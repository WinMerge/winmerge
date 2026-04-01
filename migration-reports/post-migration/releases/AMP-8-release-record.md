# AMP-8 Release Record (Phase 2d)

## Scope

- Issue: `AMP-8`
- Phase: Phase 2d - Shell Integration
- Implementation commit validated by QA: `601f06c91`
- QA artifact commit: `46fbf05ef`
- Migrated module surface:
  - `winmerge-shell/src/main/java/org/winmerge/shell/ShellLauncher.java`
  - `winmerge-shell/src/main/java/org/winmerge/shell/ShellPreferencesStore.java`
  - `winmerge-shell/src/main/java/org/winmerge/shell/ShellRegistrationManager.java`
  - `winmerge-shell/src/main/java/org/winmerge/shell/WinMergeShellIntegration.java`

## AMP Project Status Check

- Endpoint: `GET /api/projects/68ac7d9f-c65a-412d-ba90-a14b19cd4e22`
- Result at release-check time (2026-04-02): `status=backlog`
- Gate validation: project is not `FAILED`; release preparation allowed

## QA Handoff

- QA gate: PASS
- Health score: 92/100
- Regression tests (`:winmerge-shell`): 13 passed, 0 failed, 0 errors, 0 skipped
- Benchmark deltas vs baseline `c8912c266`:
  - compile: -10.51%
  - module test time: +17.39% (expected due newly added shell tests)
  - `winmerge-shell-0.1.2.jar` size: +2352.72% (expected vs near-empty baseline)

Artifacts:

- `migration-reports/qa/AMP-8/qa-report.md`
- `migration-reports/qa/AMP-8/headless-results.json`
- `migration-reports/qa/AMP-8/benchmark-comparison.json`
- `migration-reports/qa/AMP-8/pre-c8912c266-summary.png`
- `migration-reports/qa/AMP-8/post-601f06c91-summary.png`
- `migration-reports/qa/AMP-8/post-601f06c91-shell-launcher.png`
- `migration-reports/qa/AMP-8/post-601f06c91-shell-preferences.png`
- `migration-reports/qa/AMP-8/post-601f06c91-shell-registration.png`
- `migration-reports/qa/AMP-8/post-601f06c91-shell-integration.png`

## Release Verification

- Full build gate: `./gradlew --no-daemon clean test build --rerun-tasks` (PASS)
- Coverage audit from XML test results:
  - suites: 38
  - tests: 127
  - failures: 0
  - errors: 0
  - skipped: 1
- AMP-8 migration regression coverage present:
  - `ShellLauncherTest` (4 tests)
  - `ShellPreferencesStoreTest` (2 tests)
  - `ShellRegistrationManagerTest` (5 tests)
  - `WinMergeShellIntegrationTest` (2 tests)

## Versioning and Release Docs

- Project version bumped to `0.1.3` (`build.gradle.kts`)
- WinMerge metadata bumped to `2.16.58.0` (`Version.h`, `SetVersion.cmd`)
- Changelog updated: `Docs/Users/ChangeLog.md`
- Release documentation updated:
  - `README.md`
  - `ARCHITECTURE.md`
  - `CONTRIBUTING.md`

## Canary / Production

- `/land-and-deploy`: pending deployment window confirmation from Engineering Manager
- `/canary` 15-minute production monitoring loop: pending deploy completion
- Canary screenshots and go/no-go recommendation will be posted to Migration Director after deploy
