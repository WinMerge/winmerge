package org.winmerge.core.compare;

public class CompareOptions {
    private WhitespaceIgnoreChoice ignoreWhitespace;
    private boolean ignoreBlankLines;
    private boolean ignoreCase;
    private boolean ignoreNumbers;
    private boolean ignoreEolDifference;
    private boolean ignoreMissingTrailingEol;
    private boolean ignoreLineBreaks;

    public CompareOptions() {
        ignoreWhitespace = WhitespaceIgnoreChoice.COMPARE_ALL;
        ignoreBlankLines = false;
        ignoreCase = false;
        ignoreNumbers = false;
        ignoreEolDifference = false;
        ignoreMissingTrailingEol = false;
        ignoreLineBreaks = false;
    }

    public CompareOptions(CompareOptions other) {
        ignoreWhitespace = other.ignoreWhitespace;
        ignoreBlankLines = other.ignoreBlankLines;
        ignoreCase = other.ignoreCase;
        ignoreNumbers = other.ignoreNumbers;
        ignoreEolDifference = other.ignoreEolDifference;
        ignoreMissingTrailingEol = other.ignoreMissingTrailingEol;
        ignoreLineBreaks = other.ignoreLineBreaks;
    }

    public void setFromDiffOptions(DiffOptions options) {
        ignoreWhitespace = WhitespaceIgnoreChoice.fromLegacyValue(options.ignoreWhitespace);
        ignoreBlankLines = options.ignoreBlankLines;
        ignoreCase = options.ignoreCase;
        ignoreEolDifference = options.ignoreEol;
        ignoreNumbers = options.ignoreNumbers;
        ignoreMissingTrailingEol = options.ignoreMissingTrailingEol;
        ignoreLineBreaks = options.ignoreLineBreaks;
    }

    public WhitespaceIgnoreChoice getIgnoreWhitespace() {
        return ignoreWhitespace;
    }

    public void setIgnoreWhitespace(WhitespaceIgnoreChoice ignoreWhitespace) {
        this.ignoreWhitespace = ignoreWhitespace;
    }

    public boolean isIgnoreBlankLines() {
        return ignoreBlankLines;
    }

    public void setIgnoreBlankLines(boolean ignoreBlankLines) {
        this.ignoreBlankLines = ignoreBlankLines;
    }

    public boolean isIgnoreCase() {
        return ignoreCase;
    }

    public void setIgnoreCase(boolean ignoreCase) {
        this.ignoreCase = ignoreCase;
    }

    public boolean isIgnoreNumbers() {
        return ignoreNumbers;
    }

    public void setIgnoreNumbers(boolean ignoreNumbers) {
        this.ignoreNumbers = ignoreNumbers;
    }

    public boolean isIgnoreEolDifference() {
        return ignoreEolDifference;
    }

    public void setIgnoreEolDifference(boolean ignoreEolDifference) {
        this.ignoreEolDifference = ignoreEolDifference;
    }

    public boolean isIgnoreMissingTrailingEol() {
        return ignoreMissingTrailingEol;
    }

    public void setIgnoreMissingTrailingEol(boolean ignoreMissingTrailingEol) {
        this.ignoreMissingTrailingEol = ignoreMissingTrailingEol;
    }

    public boolean isIgnoreLineBreaks() {
        return ignoreLineBreaks;
    }

    public void setIgnoreLineBreaks(boolean ignoreLineBreaks) {
        this.ignoreLineBreaks = ignoreLineBreaks;
    }
}
