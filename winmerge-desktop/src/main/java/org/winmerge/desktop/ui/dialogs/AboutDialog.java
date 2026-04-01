package org.winmerge.desktop.ui.dialogs;

import java.io.IOException;
import java.util.Objects;

import javafx.fxml.FXMLLoader;
import javafx.scene.control.ButtonType;
import javafx.scene.control.Dialog;
import javafx.scene.control.DialogPane;
import javafx.stage.Window;

public final class AboutDialog extends Dialog<ButtonType> {
    public AboutDialog(Window owner, AboutDialogModel model) {
        Objects.requireNonNull(model, "model");

        if (owner != null) {
            initOwner(owner);
        }

        DialogPane dialogPane = loadDialogPane(model);
        setDialogPane(dialogPane);
        setTitle("About WinMerge");
        setResizable(true);
        dialogPane.getButtonTypes().setAll(ButtonType.OK);
    }

    private DialogPane loadDialogPane(AboutDialogModel model) {
        FXMLLoader loader = new FXMLLoader(getClass().getResource("/org/winmerge/desktop/ui/dialogs/AboutDialogPane.fxml"));
        final DialogPane dialogPane;
        try {
            dialogPane = loader.load();
        } catch (IOException ioException) {
            throw new IllegalStateException("Unable to load about dialog FXML", ioException);
        }
        AboutDialogController controller = loader.getController();
        controller.bind(model);
        return dialogPane;
    }
}
