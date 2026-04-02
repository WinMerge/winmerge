package org.winmerge.desktop.ui.dialogs;

import java.util.List;

public record PatchDialogRequest(
    List<PatchFiles> files,
    String leftPath,
    String rightPath,
    String resultPath,
    String patchStyle,
    int contextLines,
    boolean copyToClipboard,
    boolean appendToFile,
    boolean openInEditor,
    boolean includeCommandLine
) {
    public PatchDialogRequest {
        files = files == null ? List.of() : List.copyOf(files);
        leftPath = normalize(leftPath);
        rightPath = normalize(rightPath);
        resultPath = normalize(resultPath);
        patchStyle = normalizeStyle(patchStyle);
        contextLines = Math.max(0, contextLines);

        if (files.size() > 0) {
            PatchFiles first = files.get(0);
            if (leftPath.isBlank()) {
                leftPath = first.leftPath();
            }
            if (rightPath.isBlank()) {
                rightPath = first.rightPath();
            }
            if (resultPath.isBlank()) {
                resultPath = first.resultPath();
            }
        }
    }

    public static PatchDialogRequest defaults(List<PatchFiles> files) {
        return new PatchDialogRequest(files, "", "", "", "Normal", 3, false, false, false, true);
    }

    private static String normalize(String value) {
        return value == null ? "" : value.trim();
    }

    private static String normalizeStyle(String value) {
        String normalized = normalize(value);
        return normalized.isEmpty() ? "Normal" : normalized;
    }
}
