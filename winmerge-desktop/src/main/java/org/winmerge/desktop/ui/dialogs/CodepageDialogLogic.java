package org.winmerge.desktop.ui.dialogs;

import java.util.Optional;

final class CodepageDialogLogic {
    private CodepageDialogLogic() {
    }

    static boolean canSubmit(String loadCodepage, String saveCodepage, boolean affectLeft, boolean affectMiddle, boolean affectRight) {
        return hasValue(loadCodepage) && hasValue(saveCodepage) && (affectLeft || affectMiddle || affectRight);
    }

    static Optional<CodepageResult> buildResult(
        String loadCodepage,
        String saveCodepage,
        boolean includeBom,
        boolean affectLeft,
        boolean affectMiddle,
        boolean affectRight
    ) {
        if (!canSubmit(loadCodepage, saveCodepage, affectLeft, affectMiddle, affectRight)) {
            return Optional.empty();
        }
        return Optional.of(
            new CodepageResult(
                loadCodepage.trim(),
                saveCodepage.trim(),
                includeBom,
                affectLeft,
                affectMiddle,
                affectRight
            )
        );
    }

    private static boolean hasValue(String value) {
        return value != null && !value.trim().isEmpty();
    }
}
