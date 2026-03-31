# Diff Algorithm Corpus

This corpus is the pre-migration safety net for WinMerge diff-equivalence validation.

## Coverage
- Empty files
- Single-line edits
- Whitespace-only changes
- Unicode content
- Binary content
- Large files (> 1 MiB)

## Usage
1. Generate large fixtures:
   - `./Testing/Data/DiffAlgorithmCorpus/generate-large-fixtures.sh`
2. Use `manifest.json` to drive baseline capture in C++ and parity checks in Java.
