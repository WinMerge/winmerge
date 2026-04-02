# QA Report — AMP-34 (Filter Dialog Migration)

## Decision
- **QA Status:** FAIL
- **Health Score:** **44/100**
- **Gate Tier Used:** **Exhaustive** (high-complexity migration surface: 7 new dialogs + DialogService contract changes)
- **Release Recommendation:** Do **not** hand off to Release Engineer. Return to Migration Engineer (code reproducibility failure).

## Scope Verified
- Issue under QA: `AMP-34`
- Baseline commit for reproducibility/rollback checks: `2d8222b20`
- Candidate under review: current workspace delta (`workspace-amp34`, uncommitted)
- Migrated surface under test:
  - `winmerge-desktop/src/main/java/org/winmerge/desktop/ui/dialogs/*Filter*`
  - `winmerge-desktop/src/main/resources/org/winmerge/desktop/ui/dialogs/*Filter*`
  - `DialogService` / `DefaultDialogService`
  - `winmerge-desktop/src/test/java/org/winmerge/desktop/ui/dialogs/*Filter*Test`
  - `winmerge-desktop/src/test/java/org/winmerge/desktop/ui/FilterSettingsDialogFxIntegrationTest.java`

## Blocking Finding (Release Gate)

1. **Committed-state reproducibility failure (CRITICAL)**
- Impact: The reviewed migration exists only in a dirty workspace; committed baseline fails compile/test/jar and therefore cannot be released or rolled back cleanly.
- Repro:
  1. `git worktree add --detach /tmp/winmerge-qa-amp34-pre-iI2l1x 2d8222b20`
  2. `cd /tmp/winmerge-qa-amp34-pre-iI2l1x`
  3. `./gradlew --no-daemon :winmerge-desktop:clean :winmerge-desktop:compileJava --rerun-tasks`
- Result:
  - Compile fails with missing symbols in `DefaultDialogService` for `FilterSettingsModel`, `FilterConditionRequest`, `FilterConditionResult`, `SharedFilterDialog`, `TestFilterDialog` (19 errors).
- Evidence:
  - `migration-reports/qa/AMP-34/pre-2d8222b20-compile-gate.png`
  - `migration-reports/qa/AMP-34/tmp/base_compile.log`

## Regression Test Results

### Candidate workspace forward validation (diagnostic only)
- `./gradlew --no-daemon :winmerge-desktop:clean :winmerge-desktop:compileJava --rerun-tasks` → PASS
- `./gradlew --no-daemon :winmerge-desktop:test --rerun-tasks --tests org.winmerge.desktop.ui.dialogs.FilterConditionDialogControllerTest --tests org.winmerge.desktop.ui.dialogs.FileFiltersDialogModelTest --tests org.winmerge.desktop.ui.dialogs.SharedFilterDialogTest --tests org.winmerge.desktop.ui.dialogs.LineFiltersDialogModelTest --tests org.winmerge.desktop.ui.dialogs.SubstitutionFiltersDialogModelTest --tests org.winmerge.desktop.ui.FilterSettingsDialogFxIntegrationTest` → PASS
- `./gradlew --no-daemon :winmerge-desktop:test --rerun-tasks` → PASS
- `./gradlew --no-daemon test --rerun-tasks` → PASS
- Candidate desktop totals: 84 tests, 0 failures, 0 errors, 1 skipped
- Candidate full-suite totals: core 63 + desktop 84 + plugins 7 + shell 18 = 172 tests, 0 failures, 0 errors, 1 skipped

### Baseline committed-state validation (authoritative for release)
- `./gradlew --no-daemon :winmerge-desktop:clean :winmerge-desktop:compileJava --rerun-tasks` → FAIL
- `./gradlew --no-daemon :winmerge-desktop:test --rerun-tasks` → FAIL
- `./gradlew --no-daemon test --rerun-tasks` → FAIL
- `./gradlew --no-daemon :winmerge-desktop:jar` → FAIL

### Regression Test Deliverable Requirement
- Requirement not met for release gating: migrated regression coverage currently exists only in uncommitted workspace files for AMP-34 scope.
- No additional QA-authored regression test commit was produced because the gate is blocked before releasable committed-state verification.

## Headless Browser Evidence
- Artifact index: `migration-reports/qa/AMP-34/headless-results.json`
- Screenshots:
  - `migration-reports/qa/AMP-34/pre-2d8222b20-compile-gate.png`
  - `migration-reports/qa/AMP-34/post-workspace-amp34-compile-gate.png`
  - `migration-reports/qa/AMP-34/post-workspace-amp34-targeted-tests-log.png`
  - `migration-reports/qa/AMP-34/post-workspace-amp34-summary.png`
  - `migration-reports/qa/AMP-34/post-workspace-amp34-filter-condition-dialog.png`
  - `migration-reports/qa/AMP-34/post-workspace-amp34-file-filters-model.png`
  - `migration-reports/qa/AMP-34/post-workspace-amp34-shared-filter.png`
  - `migration-reports/qa/AMP-34/post-workspace-amp34-line-filters.png`
  - `migration-reports/qa/AMP-34/post-workspace-amp34-substitution-filters.png`
  - `migration-reports/qa/AMP-34/post-workspace-amp34-filter-settings-integration.png`
  - `migration-reports/qa/AMP-34/post-workspace-amp34-tab-routing.png`
  - `migration-reports/qa/AMP-34/post-workspace-amp34-save-closing.png`

## Performance Baseline Comparison (`/benchmark` equivalent)
- Artifact: `migration-reports/qa/AMP-34/benchmark-comparison.json`
- Compile time:
  - Baseline: 5.96s (FAIL)
  - Candidate: 5.48s (PASS)
  - Delta: -8.05%
- Desktop test time:
  - Baseline: 4.67s (FAIL)
  - Candidate: 6.70s (PASS)
  - Delta: +43.47%
- Full-suite test time:
  - Baseline: 5.12s (FAIL)
  - Candidate: 7.85s (PASS)
  - Delta: +53.32%
- Jar artifact size:
  - Baseline: not produced (compile failed)
  - Candidate: 896,685 bytes
- **Policy status (>10% regression): FAIL**
  - Benchmark comparison is not release-valid while baseline committed-state reproducibility fails.

## Migration-Specific Correctness Checks
- Data integrity: N/A (no database/schema migration in AMP-34 scope)
- API contract preservation: BLOCKED (cannot clear committed-state contract behavior until symbols/tests are committed and reproducible)
- Authentication/session continuity: N/A (desktop dialog scope)
- Critical migrated flows: pass in workspace diagnostics, not releasable due committed-state failure

## Rollback Verification
- Forward validation succeeded only in dirty workspace candidate.
- Rollback/repro baseline (`2d8222b20`) fails compile/test/jar in isolated worktree; clean rollback acceptance criteria not met.
- Database rollback script from Database Migration Specialist: not provided for this phase (no DB migration surface).

## Required Remediation (Migration Engineer)
1. Commit the AMP-34 dialog implementation and associated regression tests (currently uncommitted workspace state).
2. Re-run clean-worktree commands against the exact committed candidate:
   - `./gradlew --no-daemon :winmerge-desktop:clean :winmerge-desktop:compileJava --rerun-tasks`
   - `./gradlew --no-daemon :winmerge-desktop:test --rerun-tasks`
   - `./gradlew --no-daemon test --rerun-tasks`
   - `./gradlew --no-daemon :winmerge-desktop:jar`
3. Re-submit to QA with commit hash(es) and clean-worktree evidence.

## Final QA Gate Verdict
- **AMP-34 is blocked.**
- Route back to **Migration Engineer** with repro above (code reproducibility failure).
