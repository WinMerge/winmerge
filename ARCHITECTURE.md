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

## Latest Released Migration Impact (AMP-16, Phase 2c-4)

AMP-16 shipped the JavaFX hex merge view surface in `winmerge-desktop`:

- `org.winmerge.desktop.ui.hex.HexDocModel`
- `org.winmerge.desktop.ui.hex.HexController`
- `org.winmerge.desktop.ui.hex.HexGridCanvas`
- `org/winmerge/desktop/ui/hex/HexPane.fxml`

The release includes:

- async hex loading off the JavaFX UI thread
- bounded binary read guard (64 MiB per file) with user-visible feedback
- synchronized side-by-side pane scrolling and diff-byte highlighting

Release gates completed before release handoff:

- staff review approval
- security review clearance
- design review pass
- QA gate pass with health score 95/100

See `migration-reports/post-migration/releases/AMP-16-release-record.md` for
release-level verification evidence.
