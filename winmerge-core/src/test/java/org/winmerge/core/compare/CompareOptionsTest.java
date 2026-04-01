package org.winmerge.core.compare;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

class CompareOptionsTest {
    @Test
    void mapsDiffOptionsIntoCompareOptions() {
        DiffOptions options = new DiffOptions();
        options.ignoreWhitespace = 2;
        options.ignoreBlankLines = true;
        options.ignoreCase = true;
        options.ignoreNumbers = true;
        options.ignoreEol = true;
        options.ignoreMissingTrailingEol = true;
        options.ignoreLineBreaks = true;

        CompareOptions compareOptions = new CompareOptions();
        compareOptions.setFromDiffOptions(options);

        assertEquals(WhitespaceIgnoreChoice.IGNORE_ALL, compareOptions.getIgnoreWhitespace());
        assertTrue(compareOptions.isIgnoreBlankLines());
        assertTrue(compareOptions.isIgnoreCase());
        assertTrue(compareOptions.isIgnoreNumbers());
        assertTrue(compareOptions.isIgnoreEolDifference());
        assertTrue(compareOptions.isIgnoreMissingTrailingEol());
        assertTrue(compareOptions.isIgnoreLineBreaks());
    }

    @Test
    void roundTripsDiffutilsOptions() {
        DiffOptions input = new DiffOptions();
        input.ignoreWhitespace = 1;
        input.diffAlgorithm = 3;
        input.ignoreBlankLines = true;
        input.ignoreCase = true;
        input.ignoreNumbers = true;
        input.ignoreEol = true;
        input.filterCommentsLines = true;
        input.completelyBlankOutIgnoredChanges = true;
        input.ignoreMissingTrailingEol = true;
        input.ignoreLineBreaks = true;
        input.indentHeuristic = false;

        DiffutilsOptions options = new DiffutilsOptions();
        options.setFromDiffOptions(input);

        DiffOptions output = new DiffOptions();
        options.getAsDiffOptions(output);

        assertEquals(1, output.ignoreWhitespace);
        assertEquals(3, output.diffAlgorithm);
        assertTrue(output.ignoreBlankLines);
        assertTrue(output.ignoreCase);
        assertTrue(output.ignoreNumbers);
        assertTrue(output.ignoreEol);
        assertTrue(output.filterCommentsLines);
        assertTrue(output.completelyBlankOutIgnoredChanges);
        assertTrue(output.ignoreMissingTrailingEol);
        assertTrue(output.ignoreLineBreaks);
    }

    @Test
    void rejectsUnknownLegacyWhitespaceValue() {
        DiffOptions options = new DiffOptions();
        options.ignoreWhitespace = 99;

        CompareOptions compareOptions = new CompareOptions();
        assertThrows(IllegalArgumentException.class, () -> compareOptions.setFromDiffOptions(options));
    }
}
