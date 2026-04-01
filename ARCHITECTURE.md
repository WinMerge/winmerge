# Architecture (Migration Program)

This repository currently contains both the legacy WinMerge C++ codebase and the
migrated Java module structure used by the migration program.

## Module Layout

- `winmerge-core`  
  Pure Java compare/merge engine logic and tests.
- `winmerge-desktop`  
  JavaFX desktop application shell.
- `winmerge-shell`  
  Shell integration module with core-facing APIs.
- `winmerge-plugins`  
  Plugin bridge module.

## Legacy Sources

- `Src`, `ShellExtension`, `Plugins`, and related directories remain present for
  parity validation and phased migration checks.

## Phase 2b Wave 3b (AMP-12) Impact

Phase 2b Wave 3b shipped compare engine ports in `winmerge-core` under
`org.winmerge.core.compare.engines`:

- ExistenceCompare
- TimeSizeCompare
- BinaryCompare
- ImageCompare
- DiffUtilsEngine
- ByteCompare
- FullQuickCompare
- ByteComparator

Quality gates were enforced before release handoff:

- Staff review gate (request-changes resolved)
- Security review clearance
- QA release gate pass with health score 96/100

See `migration-reports/post-migration/releases/AMP-12-release-record.md` for
release-level verification evidence.
