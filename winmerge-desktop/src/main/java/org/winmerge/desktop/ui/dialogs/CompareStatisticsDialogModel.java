package org.winmerge.desktop.ui.dialogs;

import java.util.Objects;

public record CompareStatisticsDialogModel(
    String leftRootPath,
    String rightRootPath,
    long onlyLeftFiles,
    long onlyRightFiles,
    long differentFiles,
    long identicalFiles,
    long totalFiles
) {
    public CompareStatisticsDialogModel {
        leftRootPath = Objects.requireNonNullElse(leftRootPath, "");
        rightRootPath = Objects.requireNonNullElse(rightRootPath, "");
        onlyLeftFiles = Math.max(0L, onlyLeftFiles);
        onlyRightFiles = Math.max(0L, onlyRightFiles);
        differentFiles = Math.max(0L, differentFiles);
        identicalFiles = Math.max(0L, identicalFiles);
        totalFiles = Math.max(0L, totalFiles);
    }
}
