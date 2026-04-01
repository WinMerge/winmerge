# Contributing (Migration Workflow)

This project uses gated migration phases with explicit handoffs.

## Branching

- Start from current `master`.
- Keep migration changes scoped to the assigned AMP issue.
- Avoid mixing unrelated refactors in migration phase branches.

## Required Gates

1. Staff review approval
2. Security review clearance
3. QA gate pass with report + health score
4. Release Engineer ship/deploy verification

## Release Requirements

- Run full Gradle verification before release handoff:
  - `./gradlew clean test build`
- Keep migration evidence in `migration-reports/`.
- Update:
  - `Docs/Users/ChangeLog.md`
  - `README.md`
  - `ARCHITECTURE.md`
  - release record under `migration-reports/post-migration/releases/`

## AMP-12 Reference

The latest completed release phase is `AMP-12` (Phase 2b Wave 3b compare engine
migration). See:

- `migration-reports/qa/AMP-12/qa-report.md`
- `migration-reports/post-migration/releases/AMP-12-release-record.md`
