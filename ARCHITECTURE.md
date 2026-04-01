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

## Latest Released Migration Impact (AMP-8, Phase 2d)

AMP-8 shipped the shell integration migration in `winmerge-shell`:

- `org.winmerge.shell.ShellLauncher`
- `org.winmerge.shell.ShellPreferencesStore`
- `org.winmerge.shell.ShellRegistrationManager`
- `org.winmerge.shell.WinMergeShellIntegration`

The release includes:

- platform-aware command launching with failure diagnostics propagation
- preferences-backed shell settings with persistence consistency on failures
- idempotent Windows unregister handling with missing-key convergence
- explicit unsupported rollback signaling for non-Windows file association
  unregister flows

Release gates completed before release handoff:

- staff review approval
- QA gate pass with health score 92/100
- Engineering Manager benchmark policy clearance for >10% deltas

See `migration-reports/post-migration/releases/AMP-8-release-record.md` for
release-level verification evidence.
