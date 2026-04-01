# QA Report — AMP-16 (Phase 2c-4 Hex Merge View)

## Decision
- **QA Status:** PASS
- **Health Score:** **95/100**
- **Gate Tier Used:** **Standard** (effective migration risk documented as MEDIUM in `migration-reports/pre-migration/plan/migration-plan.md`)
- **Release Recommendation:** Ready for handoff to Release Engineer

## Scope Verified
- Commit under QA: `b25b10bb1` + QA hardening for deterministic test execution
- Baseline for comparison: `d32e60995`
- Migrated surface under test:
  - `winmerge-desktop/src/main/java/org/winmerge/desktop/ui/hex/HexDocModel.java`
  - `winmerge-desktop/src/main/java/org/winmerge/desktop/ui/hex/HexController.java`
  - `winmerge-desktop/src/main/java/org/winmerge/desktop/ui/hex/HexGridCanvas.java`
  - `winmerge-desktop/src/main/resources/org/winmerge/desktop/ui/hex/HexPane.fxml`

## Regression Test Results
- Command: `./gradlew --no-daemon :winmerge-desktop:clean :winmerge-desktop:test --rerun-tasks`
  - Candidate result: PASS
  - Candidate totals: 48 tests, 0 failures, 0 errors, 1 skipped
- Baseline (`d32e60995`) full suite:
  - PASS
  - 31 tests, 0 failures, 0 errors, 0 skipped

### QA-Added Regression Coverage (Committed)
- `winmerge-desktop/src/test/java/org/winmerge/desktop/ui/hex/HexControllerFxIntegrationTest.java`
  - `loadFilesPublishesDiffStatusForBinaryPair`
  - `loadFilesReportsSizeGuardFailureWhenLimitExceeded`
  - `leftAndRightScrollBarsStaySynchronized`
- Hardening fix in existing snapshot regression test:
  - `winmerge-desktop/src/test/java/org/winmerge/desktop/ui/hex/HexGridCanvasSnapshotTest.java`
  - Ensures JavaFX toolkit initialization is idempotent across full-suite execution (`Toolkit already initialized` no longer fails run order)

## Headless Browser Evidence
- Tool: Playwright (headless Chromium)
- Artifact index: `migration-reports/qa/AMP-16/headless-results.json`
- Screenshots:
  - `migration-reports/qa/AMP-16/pre-d32e60995-summary.png`
  - `migration-reports/qa/AMP-16/post-b25b10bb1-summary.png`
  - `migration-reports/qa/AMP-16/post-b25b10bb1-hex-doc-model.png`
  - `migration-reports/qa/AMP-16/post-b25b10bb1-hex-load-executor.png`
  - `migration-reports/qa/AMP-16/post-b25b10bb1-hex-controller-fx.png`
  - `migration-reports/qa/AMP-16/post-b25b10bb1-hex-snapshot.png`

## Performance Baseline Comparison (`/benchmark` equivalent)
- Artifact: `migration-reports/qa/AMP-16/benchmark-comparison.json`
- Compile time (`:winmerge-desktop:clean :winmerge-desktop:compileJava`):
  - Baseline: 4.72s
  - Candidate: 4.85s
  - Delta: **+2.75%** (within threshold)
- Desktop test time (`:winmerge-desktop:clean :winmerge-desktop:test --rerun-tasks`):
  - Baseline: 5.81s
  - Candidate: 5.98s
  - Delta: **+2.93%** (within threshold)
- Resource size delta:
  - `winmerge-desktop-0.1.1.jar`: +4.23%
- Regression threshold policy (>10%): **No violations**

## Migration-Specific Correctness Checks
- Data integrity: PASS (no database migration for AMP-16; binary file read/compare only)
- API contract preservation: PASS (no external API endpoint/interface changes in this phase)
- Authentication/session continuity: N/A (desktop local UI flow; no auth/session boundary introduced)
- Critical migrated flows validated:
  - side-by-side hex model loading from binary inputs
  - 8-digit offset formatting (`0x00000000` style)
  - diff-byte highlighting behavior
  - synchronized pane scrolling via linked scrollbars
  - bounded load behavior with explicit user-facing guard message

## Rollback Verification
- Forward validation completed on candidate state with passing compile/test and headless evidence capture.
- Rollback simulation executed in isolated worktree at `d32e60995` (`/tmp/winmerge-qa-amp16-pre-d32`) with matching compile/test/jar commands.
- Result: rollback target is reproducible and runnable; rollback path confirmed for this migration unit.
- Database rollback script: not applicable for AMP-16 (no schema/data migration surface provided by Database Migration Specialist for this phase).

## Final QA Gate Verdict
- No CRITICAL user-flow regressions in migrated AMP-16 functionality.
- No benchmark regressions above the 10% threshold.
- Rollback verification successful.
- **AMP-16 is cleared for Release Engineer handoff.**
