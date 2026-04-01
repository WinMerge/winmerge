package org.winmerge.desktop.ui.dialogs;

import java.io.IOException;
import java.nio.file.Path;
import java.util.Objects;

import javafx.fxml.FXMLLoader;
import javafx.scene.control.ButtonBar;
import javafx.scene.control.ButtonType;
import javafx.scene.control.Dialog;
import javafx.scene.control.DialogPane;
import javafx.stage.Window;

public final class SaveClosingDialog extends Dialog<SaveClosingChoice> {
    private static final ButtonType SAVE_BUTTON = new ButtonType("Save", ButtonBar.ButtonData.YES);
    private static final ButtonType DISCARD_BUTTON = new ButtonType("Discard", ButtonBar.ButtonData.NO);

    public SaveClosingDialog(Window owner, Path filePath) {
        Objects.requireNonNull(filePath, "filePath");

        if (owner != null) {
            initOwner(owner);
        }

        FXMLLoader loader = new FXMLLoader(getClass().getResource("/org/winmerge/desktop/ui/dialogs/SaveClosingDialogPane.fxml"));
        final DialogPane dialogPane;
        try {
            dialogPane = loader.load();
        } catch (IOException ioException) {
            throw new IllegalStateException("Unable to load save-closing dialog FXML", ioException);
        }

        SaveClosingDialogController controller = loader.getController();
        controller.bind(filePath);

        setDialogPane(dialogPane);
        setTitle("Save Changes");
        dialogPane.getButtonTypes().setAll(SAVE_BUTTON, DISCARD_BUTTON, ButtonType.CANCEL);
        setResultConverter(buttonType -> {
            if (buttonType == SAVE_BUTTON) {
                return SaveClosingChoice.SAVE;
            }
            if (buttonType == DISCARD_BUTTON) {
                return SaveClosingChoice.DISCARD;
            }
            return SaveClosingChoice.CANCEL;
        });
    }
}
