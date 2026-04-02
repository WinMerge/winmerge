package org.winmerge.desktop.ui.dialogs;

import java.util.Optional;

final class PatchDialogLogic {
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
        if (resultPath == null || resultPath.trim().isEmpty()) {
            return Optional.empty();
        }

        Optional<Integer> contextLines = parseContextLines(contextLinesText);
        if (contextLines.isEmpty()) {
            return Optional.empty();
        }

        String style = patchStyle == null || patchStyle.isBlank() ? "Normal" : patchStyle.trim();
        return Optional.of(
            new PatchDialogResult(
                normalize(leftPath),
                normalize(rightPath),
                resultPath.trim(),
                style,
                contextLines.get(),
                copyToClipboard,
                appendToFile,
                openInEditor,
                includeCommandLine
            )
        );
    }

    static boolean canSubmit(String resultPath, String contextLinesText) {
        return resultPath != null
            && !resultPath.trim().isEmpty()
            && parseContextLines(contextLinesText).isPresent();
    }

    static String validationMessage(String resultPath, String contextLinesText) {
        if (resultPath == null || resultPath.trim().isEmpty()) {
            return "IDS_PATCH_DIALOG_RESULT_REQUIRED";
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
}
