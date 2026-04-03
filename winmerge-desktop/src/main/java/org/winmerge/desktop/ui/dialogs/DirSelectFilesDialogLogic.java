package org.winmerge.desktop.ui.dialogs;

import java.util.ArrayList;
import java.util.List;

final class DirSelectFilesDialogLogic {
    private DirSelectFilesDialogLogic() {
    }

    /**
     * Determines whether a button should be visible for a given option slot.
     * Mirrors C++ ShowDlgItem(exists) semantics: unavailable options are hidden.
     */
    static boolean buttonVisible(boolean available) {
        return available;
    }

    /**
     * Resolves the file path for a given button id, returning null when the
     * option slot is unavailable or out of range.
     */
    static String resolvePath(DirSelectFilesRequest request, int buttonId) {
        if (buttonId < 0 || buttonId >= 9) {
            return null;
        }
        int paneIndex = buttonId / 3;
        int optionIndex = buttonId % 3;

        DirSelectFilesRequest.DirSelectFilesPane pane = switch (paneIndex) {
            case 0 -> request.pane1();
            case 1 -> request.pane2();
            case 2 -> request.pane3();
            default -> null;
        };
        if (pane == null) {
            return null;
        }

        String[] paths = pane.filePaths();
        boolean[] available = pane.available();
        if (optionIndex < 0 || optionIndex >= paths.length || !available[optionIndex]) {
            return null;
        }
        return paths[optionIndex];
    }

    /**
     * Builds the result paths from selected button ids, skipping unavailable slots.
     */
    static DirSelectFilesResult buildResult(DirSelectFilesRequest request, List<Integer> selectedButtons) {
        List<String> selectedPaths = new ArrayList<>();
        for (Integer buttonId : selectedButtons) {
            if (buttonId == null) continue;
            String path = resolvePath(request, buttonId);
            if (path != null && !path.isBlank()) {
                selectedPaths.add(path);
            }
        }
        return new DirSelectFilesResult(new org.winmerge.core.io.PathContext(selectedPaths), new ArrayList<>(selectedButtons));
    }
}
