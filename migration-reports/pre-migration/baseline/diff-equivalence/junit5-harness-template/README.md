# JUnit 5 Diff-Equivalence Harness Template

This template compares Java diff-engine outputs with the pre-migration C++ golden baseline.

## Intended location
Copy into the future `winmerge-core` Gradle module once phase 2b starts.

## Inputs
- Corpus manifest from `Testing/Data/DiffAlgorithmCorpus/manifest.json`
- Golden baseline from `migration-reports/pre-migration/baseline/diff-equivalence/golden-baseline.json`

## Wiring steps
1. Replace `PlaceholderDiffEngine` with an adapter that invokes the Java diff implementation.
2. Replace `golden-baseline.sample.json` with the captured `golden-baseline.json`.
3. Enable the disabled test and run `./gradlew test`.
