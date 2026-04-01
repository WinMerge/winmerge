package org.winmerge.core.diff;

import org.junit.jupiter.api.DynamicTest;
import org.junit.jupiter.api.TestFactory;
import org.winmerge.core.compare.DiffCode;
import org.winmerge.core.compare.engines.CompareEngineContext;
import org.winmerge.core.compare.engines.CompareEngineResult;
import org.winmerge.core.compare.engines.FullQuickCompare;
import org.winmerge.core.io.NioFileSystemService;
import org.winmerge.core.io.PathContext;

import java.nio.file.Files;
import java.nio.file.Path;
import java.util.List;
import java.util.Locale;
import java.util.stream.Stream;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

class DiffBaselineParityTest {
    private static final List<CorpusCase> CASES = List.of(
            new CorpusCase("empty_vs_empty", "cases/empty_vs_empty.left.txt", "cases/empty_vs_empty.right.txt", "identical", 0, 0, 5.32d),
            new CorpusCase("empty_vs_single_line", "cases/empty_vs_single_line.left.txt", "cases/empty_vs_single_line.right.txt", "different", 1, 1, 5.97d),
            new CorpusCase("single_line_edit", "cases/single_line_edit.left.txt", "cases/single_line_edit.right.txt", "different", 1, 1, 6.11d),
            new CorpusCase("whitespace_only", "cases/whitespace_only.left.txt", "cases/whitespace_only.right.txt", "different", 1, 1, 5.44d),
            new CorpusCase("unicode_mutation", "cases/unicode.left.txt", "cases/unicode.right.txt", "different", 1, 1, 5.48d),
            new CorpusCase("binary_mutation", "cases/binary.left.bin", "cases/binary.right.bin", "different", 1, -1, 6.10d),
            new CorpusCase("large_over_1mb", "cases/large_over_1mb.left.txt", "cases/large_over_1mb.right.txt", "different", 1, -1, 28.94d)
    );

    @TestFactory
    Stream<DynamicTest> javaDiffEngineMatchesGoldenBaseline() {
        Path corpusRoot = findCorpusRoot();
        assertTrue(Files.exists(corpusRoot.resolve("manifest.json")),
                "Diff corpus manifest is missing: " + corpusRoot.resolve("manifest.json"));

        return CASES.stream().map(corpusCase ->
                DynamicTest.dynamicTest("parity:" + corpusCase.id(), () -> {
                    Path left = corpusRoot.resolve(corpusCase.leftPath());
                    Path right = corpusRoot.resolve(corpusCase.rightPath());
                    assertTrue(Files.exists(left), "Missing corpus input: " + left);
                    assertTrue(Files.exists(right), "Missing corpus input: " + right);

                    CompareEngineContext context = new CompareEngineContext(
                            new PathContext(left.toString(), right.toString()),
                            new NioFileSystemService());
                    context.setCompareType(DiffEngine.CompareType.CONTENT);

                    long startedAt = System.nanoTime();
                    CompareEngineResult result = new FullQuickCompare().compare(context);
                    double elapsedMs = (System.nanoTime() - startedAt) / 1_000_000.0d;

                    CaseResult caseResult = mapResult(result);
                    assertEquals(corpusCase.expectedComparison(), caseResult.comparison(),
                            "Comparison mismatch for corpus case: " + corpusCase.id());
                    assertEquals(corpusCase.expectedExitCode(), caseResult.exitCode(),
                            "Exit-code mismatch for corpus case: " + corpusCase.id());
                    if (corpusCase.expectedHunks() >= 0) {
                        assertEquals(corpusCase.expectedHunks(), caseResult.hunks(),
                                "Hunk-count mismatch for corpus case: " + corpusCase.id());
                    }

                    // Keep per-case timing visible in test output for QA benchmark artifacts.
                    System.out.printf(
                            Locale.ROOT,
                            "AMP-26 parity case=%s elapsed_ms=%.2f baseline_ms=%.2f ratio=%.2f%n",
                            corpusCase.id(),
                            elapsedMs,
                            corpusCase.baselineElapsedMs(),
                            elapsedMs / corpusCase.baselineElapsedMs());
                }));
    }

    private static Path findCorpusRoot() {
        Path cursor = Path.of("").toAbsolutePath().normalize();
        while (cursor != null) {
            Path candidate = cursor.resolve("Testing/Data/DiffAlgorithmCorpus");
            if (Files.exists(candidate.resolve("manifest.json"))) {
                return candidate;
            }
            cursor = cursor.getParent();
        }
        throw new IllegalStateException("Unable to locate Testing/Data/DiffAlgorithmCorpus from working directory.");
    }

    private static CaseResult mapResult(CompareEngineResult result) {
        int compareResult = result.diffCode() & DiffCode.COMPAREFLAGS;
        String comparison;
        int exitCode;
        if (compareResult == DiffCode.SAME) {
            comparison = "identical";
            exitCode = 0;
        } else if (compareResult == DiffCode.DIFF) {
            comparison = "different";
            exitCode = 1;
        } else {
            comparison = "error";
            exitCode = 2;
        }

        int hunks = result.significantDiffs() >= 0 && result.trivialDiffs() >= 0
                ? result.significantDiffs() + result.trivialDiffs()
                : -1;
        return new CaseResult(comparison, exitCode, hunks);
    }

    private record CorpusCase(
            String id,
            String leftPath,
            String rightPath,
            String expectedComparison,
            int expectedExitCode,
            int expectedHunks,
            double baselineElapsedMs) {
    }

    private record CaseResult(String comparison, int exitCode, int hunks) {
    }
}
