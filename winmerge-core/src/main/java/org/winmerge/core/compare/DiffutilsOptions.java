package org.winmerge.core.compare;

public class DiffutilsOptions extends CompareOptions {
    private DiffOutputType outputStyle;
    private DiffAlgorithm diffAlgorithm;
    private int contextLines;
    private boolean filterCommentsLines;
    private boolean indentHeuristic;
    private boolean completelyBlankOutIgnoredDifferences;

    public DiffutilsOptions() {
        outputStyle = DiffOutputType.NORMAL;
        diffAlgorithm = DiffAlgorithm.DEFAULT;
        contextLines = 0;
        filterCommentsLines = false;
        indentHeuristic = true;
        completelyBlankOutIgnoredDifferences = false;
    }

    public DiffutilsOptions(CompareOptions compareOptions) {
        super(compareOptions);
        outputStyle = DiffOutputType.NORMAL;
        diffAlgorithm = DiffAlgorithm.DEFAULT;
        contextLines = 0;
        filterCommentsLines = false;
        indentHeuristic = true;
        completelyBlankOutIgnoredDifferences = false;
    }

    @Override
    public void setFromDiffOptions(DiffOptions options) {
        super.setFromDiffOptions(options);
        completelyBlankOutIgnoredDifferences = options.completelyBlankOutIgnoredChanges;
        filterCommentsLines = options.filterCommentsLines;
        indentHeuristic = options.indentHeuristic;
        diffAlgorithm = DiffAlgorithm.fromLegacyValue(options.diffAlgorithm);
    }

    public void getAsDiffOptions(DiffOptions options) {
        options.completelyBlankOutIgnoredChanges = completelyBlankOutIgnoredDifferences;
        options.filterCommentsLines = filterCommentsLines;
        options.ignoreBlankLines = isIgnoreBlankLines();
        options.ignoreCase = isIgnoreCase();
        options.ignoreEol = isIgnoreEolDifference();
        options.ignoreNumbers = isIgnoreNumbers();
        options.diffAlgorithm = diffAlgorithm.toLegacyValue();
        options.ignoreMissingTrailingEol = isIgnoreMissingTrailingEol();
        options.ignoreLineBreaks = isIgnoreLineBreaks();
        options.ignoreWhitespace = getIgnoreWhitespace().toLegacyValue();
        options.indentHeuristic = indentHeuristic;
    }

    public DiffOutputType getOutputStyle() {
        return outputStyle;
    }

    public void setOutputStyle(DiffOutputType outputStyle) {
        this.outputStyle = outputStyle;
    }

    public DiffAlgorithm getDiffAlgorithm() {
        return diffAlgorithm;
    }

    public void setDiffAlgorithm(DiffAlgorithm diffAlgorithm) {
        this.diffAlgorithm = diffAlgorithm;
    }

    public int getContextLines() {
        return contextLines;
    }

    public void setContextLines(int contextLines) {
        this.contextLines = contextLines;
    }

    public boolean isFilterCommentsLines() {
        return filterCommentsLines;
    }

    public void setFilterCommentsLines(boolean filterCommentsLines) {
        this.filterCommentsLines = filterCommentsLines;
    }

    public boolean isIndentHeuristic() {
        return indentHeuristic;
    }

    public void setIndentHeuristic(boolean indentHeuristic) {
        this.indentHeuristic = indentHeuristic;
    }

    public boolean isCompletelyBlankOutIgnoredDifferences() {
        return completelyBlankOutIgnoredDifferences;
    }

    public void setCompletelyBlankOutIgnoredDifferences(boolean completelyBlankOutIgnoredDifferences) {
        this.completelyBlankOutIgnoredDifferences = completelyBlankOutIgnoredDifferences;
    }
}
