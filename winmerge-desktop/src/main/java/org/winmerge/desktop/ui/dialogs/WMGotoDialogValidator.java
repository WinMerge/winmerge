package org.winmerge.desktop.ui.dialogs;

import java.util.OptionalInt;

final class WMGotoDialogValidator {
    private WMGotoDialogValidator() {
    }

    static int rangeMax(WMGotoDialogRequest request, int selectedFileIndex, WMGotoTarget target) {
        if (selectedFileIndex < 0 || selectedFileIndex >= request.fileCount()) {
            return 0;
        }
        return target == WMGotoTarget.LINE
            ? request.maxLinePerFile()[selectedFileIndex]
            : request.maxDifference();
    }

    static OptionalInt parsePositiveInt(String text) {
        if (text == null || text.isBlank()) {
            return OptionalInt.empty();
        }
        try {
            int value = Integer.parseInt(text.trim());
            return value > 0 ? OptionalInt.of(value) : OptionalInt.empty();
        } catch (NumberFormatException ignored) {
            return OptionalInt.empty();
        }
    }

    static boolean canSubmit(String text, WMGotoDialogRequest request, int selectedFileIndex, WMGotoTarget target) {
        int rangeMax = rangeMax(request, selectedFileIndex, target);
        if (rangeMax <= 0) {
            return false;
        }
        OptionalInt parsed = parsePositiveInt(text);
        return parsed.isPresent() && parsed.getAsInt() <= rangeMax;
    }
}
