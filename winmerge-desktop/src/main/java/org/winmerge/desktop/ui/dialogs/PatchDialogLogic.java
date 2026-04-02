package org.winmerge.desktop.ui.dialogs;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.InvalidPathException;
import java.nio.file.Path;
import java.util.List;
import java.util.Optional;

final class PatchDialogLogic {
    private static final String DEFAULT_STYLE = "Normal";
    private static final List<String> SUPPORTED_STYLES = List.of("Normal", "Context", "Unified", "HTML");

    private PatchDialogLogic() {
    }

    static Optional<PatchDialogResult> buildResult(
        String leftPath,
        String rightPath,
        String resultPath,
        String patchStyle,
        String contextLinesText,
        boolean copyToClipboard,
        boolean appendToFile,
        boolean openInEditor,
        boolean includeCommandLine
    ) {
        if (!canSubmit(leftPath, rightPath, resultPath, contextLinesText)) {
            return Optional.empty();
        }

        Optional<Integer> contextLines = parseContextLines(contextLinesText);
        if (contextLines.isEmpty()) {
            return Optional.empty();
        }

        Optional<String> normalizedResultPath = normalizeResultPath(resultPath);
        if (normalizedResultPath.isEmpty()) {
            return Optional.empty();
        }

        String style = normalizeStyle(patchStyle);
        return Optional.of(
            new PatchDialogResult(
                normalize(leftPath),
                normalize(rightPath),
                normalizedResultPath.get(),
                style,
                contextLines.get(),
                copyToClipboard,
                appendToFile,
                openInEditor,
                includeCommandLine
            )
        );
    }

    static boolean canSubmit(String leftPath, String rightPath, String resultPath, String contextLinesText) {
        return validationMessage(leftPath, rightPath, resultPath, contextLinesText).isEmpty();
    }

    static String validationMessage(String leftPath, String rightPath, String resultPath, String contextLinesText) {
        if (!isExistingPathOrNullDevice(leftPath)) {
            return "IDS_DIFF_ITEM1NOTFOUND";
        }
        if (!isExistingPathOrNullDevice(rightPath)) {
            return "IDS_DIFF_ITEM2NOTFOUND";
        }
        if (isRelativeResultPath(resultPath)) {
            return "IDS_PATH_NOT_ABSOLUTE";
        }
        if (parseContextLines(contextLinesText).isEmpty()) {
            return "IDS_PATCH_DIALOG_CONTEXT_INVALID";
        }
        return "";
    }

    private static Optional<Integer> parseContextLines(String value) {
        if (value == null || value.trim().isEmpty()) {
            return Optional.empty();
        }
        try {
            int parsed = Integer.parseInt(value.trim());
            return parsed < 0 ? Optional.empty() : Optional.of(parsed);
        } catch (NumberFormatException ex) {
            return Optional.empty();
        }
    }

    private static String normalize(String value) {
        return value == null ? "" : value.trim();
    }

    private static String normalizeStyle(String value) {
        String normalized = normalize(value);
        return SUPPORTED_STYLES.contains(normalized) ? normalized : DEFAULT_STYLE;
    }

    private static boolean isExistingPathOrNullDevice(String value) {
        String normalized = normalize(value);
        if (normalized.isEmpty()) {
            return false;
        }
        if (isNullDeviceName(normalized)) {
            return true;
        }
        try {
            return Files.exists(Path.of(normalized));
        } catch (InvalidPathException ignored) {
            return false;
        }
    }

    private static boolean isRelativeResultPath(String value) {
        String normalized = normalize(value);
        if (normalized.isEmpty()) {
            return false;
        }
        try {
            return !Path.of(normalized).isAbsolute();
        } catch (InvalidPathException ignored) {
            return true;
        }
    }

    private static Optional<String> normalizeResultPath(String value) {
        String normalized = normalize(value);
        if (!normalized.isEmpty()) {
            return Optional.of(normalized);
        }
        try {
            Path tempPath = Files.createTempFile("pat", ".tmp").toAbsolutePath();
            Files.deleteIfExists(tempPath);
            return Optional.of(tempPath.toString());
        } catch (IOException ex) {
            return Optional.empty();
        }
    }

    private static boolean isNullDeviceName(String value) {
        return "nul".equalsIgnoreCase(value)
            || "nul:".equalsIgnoreCase(value)
            || "/dev/null".equalsIgnoreCase(value);
    }
}
