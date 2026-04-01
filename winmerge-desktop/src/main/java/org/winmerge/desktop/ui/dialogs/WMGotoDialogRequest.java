package org.winmerge.desktop.ui.dialogs;

import java.util.Arrays;
import java.util.Objects;

public record WMGotoDialogRequest(
    String initialInput,
    int selectedFileIndex,
    WMGotoTarget target,
    int fileCount,
    int[] maxLinePerFile,
    int maxDifference
) {
    public WMGotoDialogRequest {
        initialInput = Objects.requireNonNullElse(initialInput, "");
        target = Objects.requireNonNull(target, "target");
        if (fileCount < 2 || fileCount > 3) {
            throw new IllegalArgumentException("fileCount must be 2 or 3");
        }
        if (selectedFileIndex < 0 || selectedFileIndex >= fileCount) {
            throw new IllegalArgumentException("selectedFileIndex out of range");
        }
        if (maxLinePerFile == null || maxLinePerFile.length < 3) {
            throw new IllegalArgumentException("maxLinePerFile must contain at least 3 values");
        }
        maxLinePerFile = Arrays.copyOf(maxLinePerFile, 3);
        for (int lineCount : maxLinePerFile) {
            if (lineCount < 0) {
                throw new IllegalArgumentException("maxLinePerFile values must be >= 0");
            }
        }
        if (maxDifference < 0) {
            throw new IllegalArgumentException("maxDifference must be >= 0");
        }
    }
}
