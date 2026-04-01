package org.winmerge.desktop.ui.merge;

/**
 * Zero-based, end-exclusive line ranges for a single diff hunk.
 */
public record DiffChunk(
    int leftStartLine,
    int leftEndLine,
    int rightStartLine,
    int rightEndLine
) {
    public int leftLineCount() {
        return leftEndLine - leftStartLine;
    }

    public int rightLineCount() {
        return rightEndLine - rightStartLine;
    }
}
