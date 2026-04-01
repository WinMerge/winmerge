# QA Report — AMP-26 (Phase 2g Diff Corpus Wiring)

## Decision
- **QA Status:** PASS
- **Health Score:** **97/100**
- **Gate Tier Used:** **Standard** (effective migration risk is documented as MEDIUM in `migration-reports/pre-migration/plan/migration-plan.md`)
- **Release Recommendation:** Ready for handoff to Release Engineer for AMP-26 scope

## Scope Verified
- Commit under QA: `5162a4d73`
- Baseline for comparison and rollback validation: `a3dfd4d40`
- Migrated surface under test:
  - `winmerge-core/src/test/java/org/winmerge/core/diff/DiffBaselineParityTest.java`

## Regression Test Results
- Command: `./gradlew --no-daemon :winmerge-core:test --tests org.winmerge.core.diff.DiffBaselineParityTest --rerun-tasks`
  - Result: PASS
  - Candidate parity suite totals: 7 tests, 0 failures, 0 errors
- Command: `./gradlew --no-daemon :winmerge-core:test`
  - Result: PASS
- Command: `./gradlew --no-daemon test`
  - Result: PASS

### Full Regression Totals (Baseline vs Candidate)
- Baseline (`a3dfd4d40`): 128 tests, 0 failures, 0 errors, 1 skipped
- Candidate (`5162a4d73`): 135 tests, 0 failures, 0 errors, 1 skipped

### Regression Tests Covering Migrated Functionality (Committed)
- Regression tests are committed in `5162a4d73`:
  - `javaDiffEngineMatchesGoldenBaseline` (dynamic suite over all 7 corpus cases)
- File:
  - `winmerge-core/src/test/java/org/winmerge/core/diff/DiffBaselineParityTest.java`

## Headless Browser Evidence
- Tool: Playwright (headless Chromium)
- Artifact index: `migration-reports/qa/AMP-26/headless-results.json`
- Screenshots:
  - `migration-reports/qa/AMP-26/pre-a3dfd4d40-summary.png`
  - `migration-reports/qa/AMP-26/pre-a3dfd4d40-diff-engine.png`
  - `migration-reports/qa/AMP-26/post-5162a4d73-summary.png`
  - `migration-reports/qa/AMP-26/post-5162a4d73-diff-engine.png`
  - `migration-reports/qa/AMP-26/post-5162a4d73-diff-baseline-parity.png`

## Performance Baseline Comparison (`/benchmark` equivalent)
- Artifact: `migration-reports/qa/AMP-26/benchmark-comparison.json`
- Build time (`:winmerge-core:clean :winmerge-core:build --rerun-tasks`):
  - Baseline: 5.77s
  - Candidate: 4.97s
  - Delta: **-13.86%** (improvement)
- Core test time (`:winmerge-core:test --rerun-tasks`):
  - Baseline: 5.03s
  - Candidate: 5.06s
  - Delta: **+0.60%** (within 10% threshold)
- Resource size delta:
  - `winmerge-core-0.1.3.jar`: 0.00%
  - `winmerge-core-0.1.3-sources.jar`: 0.00%
- Corpus case timing (rollback guard 3x):
  - Max observed ratio: `2.54x` (`large_over_1mb`) vs threshold `3.0x`
- Regression threshold policy (>10%): **No violations**

## Migration-Specific Correctness Checks
- Data integrity: PASS (no database schema/data migration surface in AMP-26)
- API contract preservation: PASS (no public API/interface or integration behavior change; regression addition only)
- Authentication/session continuity: N/A (no auth/session boundary in winmerge-core test harness work)
- Functional corpus correctness: PASS (7/7 cases matched expected comparison and exit code; 0% failure rate)

## Rollback Verification
- Forward validation completed on candidate (`5162a4d73`) with passing targeted, module, and full-suite test runs.
- Rollback simulation executed in isolated worktree at baseline commit (`a3dfd4d40`) in `/tmp/winmerge-qa-amp26-pre-2RPToq` with `./gradlew --no-daemon test`.
- Result: rollback target is reproducible and cleanly runnable; rollback path confirmed for this migration unit.
- Database rollback script from Database Migration Specialist: not provided for AMP-26 (no database migration surface in this phase).

## Final QA Gate Verdict
- No CRITICAL flow regressions detected for migrated Phase 2g corpus-validation functionality.
- No benchmark regressions above the 10% threshold.
- Rollback verification successful.
- **AMP-26 is cleared for Release Engineer handoff.**
