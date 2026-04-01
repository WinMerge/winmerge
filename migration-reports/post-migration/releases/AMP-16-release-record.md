# AMP-16 Release Record (Phase 2c-4)

## Scope

- Issue: `AMP-16`
- Phase: Phase 2c-4 — Hex Merge View
- QA-cleared commit set: `e29cad8b3`, `31cfee44a`
- Migrated UI surface:
  - `winmerge-desktop/src/main/java/org/winmerge/desktop/ui/hex/HexDocModel.java`
  - `winmerge-desktop/src/main/java/org/winmerge/desktop/ui/hex/HexController.java`
  - `winmerge-desktop/src/main/java/org/winmerge/desktop/ui/hex/HexGridCanvas.java`
  - `winmerge-desktop/src/main/resources/org/winmerge/desktop/ui/hex/HexPane.fxml`

## AMP Project Status Check

- Endpoint: `GET /api/projects/68ac7d9f-c65a-412d-ba90-a14b19cd4e22`
- Result at release-check time (2026-04-01): `status=backlog`
- Gate validation: project is not `FAILED`; release preparation allowed

## QA Handoff

- QA gate: PASS
- Health score: 95/100
- Regression tests: 48 passed, 0 failed, 0 errors, 1 skipped
- Benchmark deltas vs baseline `d32e60995`:
  - compile: +2.75%
  - desktop tests: +2.93%
  - jar size: +4.23%

Artifacts:

- `migration-reports/qa/AMP-16/qa-report.md`
- `migration-reports/qa/AMP-16/headless-results.json`
- `migration-reports/qa/AMP-16/benchmark-comparison.json`
- `migration-reports/qa/AMP-16/post-b25b10bb1-summary.png`
- `migration-reports/qa/AMP-16/post-b25b10bb1-hex-controller-fx.png`

## Release Verification

- Full build gate: `./gradlew --no-daemon clean test build` (PASS)
- Test coverage audit from XML results:
  - suites: 30
  - tests: 103
  - failures: 0
  - errors: 0
  - skipped: 1
- Hex migration regression coverage present in full-suite execution:
  - `HexDocModelTest` (5 tests)
  - `HexLoadExecutorTest` (2 tests)
  - `HexControllerFxIntegrationTest` (3 tests)
  - `HexGridCanvasSnapshotTest` (1 skipped; environment-dependent snapshot case)

## Versioning and Release Docs

- Project version bumped to `0.1.2` (`build.gradle.kts`)
- WinMerge metadata bumped to `2.16.57.0` (`Version.h`, `SetVersion.cmd`)
- Changelog updated: `Docs/Users/ChangeLog.md`
- Release documentation updated:
  - `README.md`
  - `ARCHITECTURE.md`
  - `CONTRIBUTING.md`

## Canary / Production

- `/land-and-deploy`: pending
- Deployment window confirmation from Engineering Manager: pending
- `/canary` 15-minute production monitoring loop: pending deploy completion
- Canary screenshots and go/no-go summary will be appended after deploy
