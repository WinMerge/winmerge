package org.winmerge.desktop.ui.dialogs;

public record PatchFiles(
    String leftPath,
    String rightPath,
    String resultPath
) {
    public PatchFiles {
        leftPath = leftPath == null ? "" : leftPath;
        rightPath = rightPath == null ? "" : rightPath;
        resultPath = resultPath == null ? "" : resultPath;
    }
}
