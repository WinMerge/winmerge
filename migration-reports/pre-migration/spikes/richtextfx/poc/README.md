# RichTextFX Syntax Highlighting Spike PoC

This PoC demonstrates a JavaFX + RichTextFX `CodeArea` editor with:

- syntax highlighting for C++, Java, and Python
- line numbers
- basic undo/redo controls

## Quick Start

```bash
cd migration-reports/pre-migration/spikes/richtextfx/poc
./scripts/build.sh
./scripts/run.sh
```

## Notes

- The scripts download dependencies directly from Maven Central into `lib/`.
- JavaFX native classifier is selected from host OS/arch automatically.
- This PoC intentionally focuses on highlighting foundation and editor capability checks, not full WinMerge editor parity.
