package org.winmerge.desktop.ui;

import java.nio.file.Path;
import java.util.Objects;
import java.util.function.Consumer;

import org.winmerge.desktop.ui.dialogs.DialogService;
import org.winmerge.desktop.ui.dialogs.SaveClosingChoice;

final class SaveClosingHandler {
    private SaveClosingHandler() {
    }

    static boolean confirmClose(DirtyTab tab, DialogService dialogService, Consumer<String> statusListener) {
        Objects.requireNonNull(tab, "tab");
        Objects.requireNonNull(dialogService, "dialogService");
        Objects.requireNonNull(statusListener, "statusListener");

        if (!tab.hasUnsavedChanges()) {
            return true;
        }

        Path filePath = tab.pathForSavePrompt();
        SaveClosingChoice choice = dialogService.showSaveClosingDialog(filePath);
        return switch (choice) {
            case SAVE -> saveAndContinue(tab, filePath, statusListener);
            case DISCARD -> {
                statusListener.accept("Discarded unsaved changes for " + filePath + ".");
                yield true;
            }
            case CANCEL -> {
                statusListener.accept("Close cancelled.");
                yield false;
            }
        };
    }

    private static boolean saveAndContinue(DirtyTab tab, Path filePath, Consumer<String> statusListener) {
        if (!tab.saveChanges()) {
            statusListener.accept("Save failed for " + filePath + "; close cancelled.");
            return false;
        }
        statusListener.accept("Saved changes to " + filePath + ".");
        return true;
    }
}
