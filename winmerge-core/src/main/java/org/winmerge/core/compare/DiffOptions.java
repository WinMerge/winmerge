package org.winmerge.core.compare;

public final class DiffOptions {
    public int ignoreWhitespace;
    public int diffAlgorithm;
    public boolean ignoreCase;
    public boolean ignoreNumbers;
    public boolean ignoreBlankLines;
    public boolean ignoreEol;
    public boolean filterCommentsLines;
    public boolean indentHeuristic;
    public boolean completelyBlankOutIgnoredChanges;
    public boolean ignoreMissingTrailingEol;
    public boolean ignoreLineBreaks;

    public DiffOptions() {
        ignoreWhitespace = 0;
        diffAlgorithm = 0;
        ignoreCase = false;
        ignoreNumbers = false;
        ignoreBlankLines = false;
        ignoreEol = false;
        filterCommentsLines = false;
        indentHeuristic = true;
        completelyBlankOutIgnoredChanges = false;
        ignoreMissingTrailingEol = false;
        ignoreLineBreaks = false;
    }
}
