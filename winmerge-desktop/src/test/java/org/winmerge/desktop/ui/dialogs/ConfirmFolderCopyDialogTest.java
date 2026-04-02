package org.winmerge.desktop.ui.dialogs;

import javafx.scene.control.ButtonType;
import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;

class ConfirmFolderCopyDialogTest {
    @Test
    void mapsYesButtonToYesChoice() {
        assertEquals(ConfirmFolderCopyChoice.YES, ConfirmFolderCopyDialog.resolveChoice(ButtonType.YES));
    }

    @Test
    void mapsNoButtonAndWindowCloseToNoChoice() {
        assertEquals(ConfirmFolderCopyChoice.NO, ConfirmFolderCopyDialog.resolveChoice(ButtonType.NO));
        assertEquals(ConfirmFolderCopyChoice.NO, ConfirmFolderCopyDialog.resolveChoice(null));
    }

    @Test
    void mapsDontAskAgainButtonToDedicatedChoice() {
        assertEquals(
            ConfirmFolderCopyChoice.DONT_ASK_AGAIN,
            ConfirmFolderCopyDialog.resolveChoice(ConfirmFolderCopyDialog.dontAskAgainButtonType())
        );
    }
}
