# QA Report — AMP-36 (Platform & Utility Dialogs)

## Decision
- **QA Status:** FAIL
- **Health Score:** **41/100**
- **Gate Tier Used:** **Standard** (phase risk treated as MEDIUM)
- **Release Recommendation:** Do **not** hand off to Release Engineer. Return to Migration Engineer for reproducibility fix.

## Scope Verified
- Issue under QA: `AMP-36`
- Reviewed commits: `48a4e5f84`, `7c0910831`, `347f6a2bc`
- Baseline for rollback/compare: `24b276ea5`
- Migrated surface under test:
  - `winmerge-desktop/src/main/java/org/winmerge/desktop/ui/dialogs/*` (Codepage/OpenTable/Patch/WindowsManager, wrappers, DialogService wiring)

## Blocking Finding (Release Gate)

1. **Clean-worktree compile failure for reviewed commit (`347f6a2bc`)**
- Severity: CRITICAL
- Impact: The reviewed candidate is not reproducible from committed sources and cannot be released.
- Repro:
  1. `git worktree add --detach /tmp/winmerge-qa-amp36-cand-ft5tvl 347f6a2bc`
  2. `cd /tmp/winmerge-qa-amp36-cand-ft5tvl`
  3. `./gradlew --no-daemon :winmerge-desktop:compileJava`
- Result:
  - 19 compile errors in `DialogService` / `DefaultDialogService`
  - Missing symbols include `FilterSettingsModel`, `FilterConditionRequest`, `FilterConditionResult`, `SharedFilterDialog`, `TestFilterDialog`
- Evidence:
  - `migration-reports/qa/AMP-36/post-347f6a2bc-compile-gate.png`
  - `migration-reports/qa/AMP-36/tmp/post-347f6a2bc-compile-log.html`

## Regression Test Results

### Clean-worktree gate (authoritative)
- Command: `./gradlew --no-daemon :winmerge-desktop:compileJava` (baseline `24b276ea5`)
  - Result: FAIL (same missing filter-dialog symbol chain)
- Command: `./gradlew --no-daemon :winmerge-desktop:compileJava` (candidate `347f6a2bc`)
  - Result: FAIL (same class-missing chain)

### Workspace diagnostic runs (non-authoritative for release)
These pass only in the current dirty workspace where additional uncommitted files exist.

- `./gradlew --no-daemon :winmerge-desktop:test --rerun-tasks --tests org.winmerge.desktop.ui.dialogs.CodepageDialogTest --tests org.winmerge.desktop.ui.dialogs.PatchDialogTest --tests org.winmerge.desktop.ui.dialogs.OpenTableDialogTest`
  - Result: PASS (8 tests, 0 failures)
- `./gradlew --no-daemon :winmerge-desktop:test --rerun-tasks`
  - Result: PASS (84 tests, 0 failures, 1 skipped)
- `./gradlew --no-daemon test --rerun-tasks`
  - Result: PASS (172 tests, 0 failures, 1 skipped)

### Regression Coverage Present in Reviewed Commits
- `winmerge-desktop/src/test/java/org/winmerge/desktop/ui/dialogs/CodepageDialogTest.java`
- `winmerge-desktop/src/test/java/org/winmerge/desktop/ui/dialogs/OpenTableDialogTest.java`
- `winmerge-desktop/src/test/java/org/winmerge/desktop/ui/dialogs/PatchDialogTest.java`
- `winmerge-desktop/src/test/java/org/winmerge/desktop/ui/SaveClosingHandlerTest.java` (updated)
- `winmerge-desktop/src/test/java/org/winmerge/desktop/ui/TabManagerRoutingTest.java` (updated)

## Headless Browser Evidence
- Artifact index: `migration-reports/qa/AMP-36/headless-results.json`
- Screenshots:
  - `migration-reports/qa/AMP-36/pre-24b276ea5-compile-gate.png`
  - `migration-reports/qa/AMP-36/post-347f6a2bc-compile-gate.png`
  - `migration-reports/qa/AMP-36/post-347f6a2bc-summary.png`
  - `migration-reports/qa/AMP-36/post-347f6a2bc-codepage-dialog.png`
  - `migration-reports/qa/AMP-36/post-347f6a2bc-open-table-dialog.png`
  - `migration-reports/qa/AMP-36/post-347f6a2bc-patch-dialog.png`
  - `migration-reports/qa/AMP-36/post-347f6a2bc-save-closing.png`
  - `migration-reports/qa/AMP-36/post-347f6a2bc-tab-routing.png`

## Performance Baseline Comparison (`/benchmark` equivalent)
- Artifact: `migration-reports/qa/AMP-36/benchmark-comparison.json`
- Clean compile timings:
  - Baseline: 6.70s (FAIL)
  - Candidate: 5.91s (FAIL)
- Workspace diagnostic timings:
  - Compile: 5.52s
  - Targeted migrated-flow tests: 6.32s
  - Desktop tests: 6.66s
  - Full tests: 6.75s
- **Benchmark policy status:** FAIL (non-reproducible candidate build invalidates comparison)

## Migration-Specific Correctness Checks
- Data integrity: N/A (no database/schema migration in AMP-36 scope)
- API contract preservation: NOT CLEARABLE (cannot validate committed-state contract due compile gate failure)
- Authentication/session continuity: N/A (desktop dialog scope)
- End-to-end migrated flow continuity: BLOCKED at compile gate in clean worktree

## Rollback Verification
- Forward migration validation in clean candidate did **not** pass compile gate.
- Because forward gate failed, rollback equivalence cannot be accepted as successful for release purposes.
- Database rollback script from Database Migration Specialist: not provided (no DB surface).

## Required Remediation (Migration Engineer)
1. Commit or otherwise include the missing filter-dialog dependencies referenced by `DialogService`/`DefaultDialogService`, or remove cross-scope references from AMP-36 if they are not in this ticket scope.
2. Re-run clean-worktree verification on the exact reviewed commit chain.
3. Re-submit to QA with reproducible commands and clean-worktree passing evidence.

## Final QA Gate Verdict
- **AMP-36 is blocked.**
- Return to **Migration Engineer** with repro above (code failure).
