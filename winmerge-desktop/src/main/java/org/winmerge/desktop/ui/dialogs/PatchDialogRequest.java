package org.winmerge.desktop.ui.dialogs;

import java.util.List;
import java.util.Set;

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
    private static final Set<String> SUPPORTED_STYLES = Set.of("Normal", "Context", "Unified", "HTML");

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
        return new PatchDialogRequest(files, "", "", "", "Normal", 3, false, false, false, false);
    }

    private static String normalize(String value) {
        return value == null ? "" : value.trim();
    }

    private static String normalizeStyle(String value) {
        String normalized = normalize(value);
        if (normalized.isEmpty()) {
            return "Normal";
        }
        return SUPPORTED_STYLES.contains(normalized) ? normalized : "Normal";
    }
}
