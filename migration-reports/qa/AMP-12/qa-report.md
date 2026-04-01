# QA Report — AMP-12 (Phase 2b Wave 3b)

## Decision
- **QA Status:** PASS
- **Health Score:** **96/100**
- **Gate Tier Used:** **Standard** (Phase 2b effective migration risk is documented as MEDIUM in `migration-reports/pre-migration/plan/migration-plan.md`)
- **Release Recommendation:** Ready for handoff to Release Engineer

## Scope Verified
- Commit under QA: `62ce3b0f7`
- Baseline for comparison: `e8abe70b3`
- Files changed in commit under QA:
  - `winmerge-core/src/main/java/org/winmerge/core/compare/engines/CompareEngineSupport.java`
  - `winmerge-core/src/test/java/org/winmerge/core/compare/engines/CompareEnginesTest.java`

## Regression Test Results
- Command: `./gradlew :winmerge-core:test --tests org.winmerge.core.compare.engines.CompareEnginesTest --rerun-tasks`
  - Result: PASS
  - Candidate suite count: 12 tests, 0 failures
- Command: `./gradlew :winmerge-core:test :winmerge-core:build`
  - Result: PASS
- Full `winmerge-core` test aggregation:
  - Baseline (`e8abe70b3`): 53 tests, 0 failures, 0 errors, 0 skipped
  - Candidate (`62ce3b0f7`): 55 tests, 0 failures, 0 errors, 0 skipped

### Regression Tests Covering Migrated Functionality
- Regression tests are committed in `62ce3b0f7` and validated in this QA run:
  - `quickCompareDoesNotCollapseDistinctInvalidUtf8BytesToSame`
  - `fullContentCompareDoesNotCollapseDistinctInvalidUtf8BytesToSame`
- File: `winmerge-core/src/test/java/org/winmerge/core/compare/engines/CompareEnginesTest.java`

## Headless Browser Evidence
- Tool: Playwright (headless Chromium)
- Artifact index: `migration-reports/qa/AMP-12/headless-results.json`
- Screenshots:
  - `migration-reports/qa/AMP-12/pre-e8abe70b3-summary.png`
  - `migration-reports/qa/AMP-12/pre-e8abe70b3-compare-engines.png`
  - `migration-reports/qa/AMP-12/post-62ce3b0f7-summary.png`
  - `migration-reports/qa/AMP-12/post-62ce3b0f7-compare-engines.png`

## Performance Baseline Comparison (`/benchmark` equivalent)
- Artifact: `migration-reports/qa/AMP-12/benchmark-comparison.json`
- Build time (`:winmerge-core:clean :winmerge-core:build --rerun-tasks`):
  - Baseline: 5.63s
  - Candidate: 5.04s
  - Delta: **-10.48%** (improvement)
- Compare-engine test run time (`CompareEnginesTest`):
  - Baseline: 5.69s
  - Candidate: 6.14s
  - Delta: **+7.91%** (within 10% threshold)
- Resource size deltas:
  - `winmerge-core-0.1.0-SNAPSHOT.jar`: +0.33%
  - `winmerge-core-0.1.0-SNAPSHOT-sources.jar`: +0.41%
- Regression threshold policy (>10%): **No violations**

## Migration-Specific Correctness Checks
- Data integrity: PASS (no DB/data migration surface in this commit)
- API contract preservation: PASS (no endpoint or external integration changes; scope limited to compare engine internals)
- Authentication/session continuity: N/A (no auth/session surface in `winmerge-core` compare engine patch)
- Pre-existing out-of-scope issue observed in both baseline and candidate:
  - Root `./gradlew -q tasks --all` fails due `:winmerge-desktop:smokeTestUi` task configuration (`SourceSetContainer` missing). This is not introduced by `62ce3b0f7`.

## Rollback Verification
- Forward validation completed on candidate commit (`62ce3b0f7`) with passing `winmerge-core` tests/build.
- Rollback simulation executed by validating baseline commit (`e8abe70b3`) in isolated worktree (`/tmp/winmerge-qa-pre-zijZBq`) with matching test/build commands.
- Result: baseline state is reproducible and cleanly runnable; rollback path confirmed for this migration unit.

## Final QA Gate Verdict
- No critical flow regressions detected for migrated compare-engine functionality.
- No benchmark regressions above the 10% threshold.
- Rollback validation successful for this code migration unit.
- **AMP-12 is cleared for Release Engineer handoff.**
