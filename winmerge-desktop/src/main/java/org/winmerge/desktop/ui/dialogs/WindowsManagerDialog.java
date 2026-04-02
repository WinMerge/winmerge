package org.winmerge.desktop.ui.dialogs;

import java.io.IOException;
import java.util.ResourceBundle;

import javafx.fxml.FXMLLoader;
import javafx.scene.Node;
import javafx.scene.control.ButtonBar;
import javafx.scene.control.ButtonType;
import javafx.scene.control.Dialog;
import javafx.scene.control.DialogPane;
import javafx.stage.Window;
import org.winmerge.desktop.i18n.I18n;

public final class WindowsManagerDialog extends Dialog<Integer> {
    private final WindowsManagerDialogController controller;
    private final ButtonType closeSelectedButtonType;

    public WindowsManagerDialog(Window owner, WindowsManagerModel model) {
        if (owner != null) {
            initOwner(owner);
        }

        FXMLLoader loader = new FXMLLoader(
            getClass().getResource("/org/winmerge/desktop/ui/dialogs/WindowsManagerDialogPane.fxml"),
            ResourceBundle.getBundle("i18n.WinMerge")
        );
        final DialogPane dialogPane;
        try {
            dialogPane = loader.load();
        } catch (IOException ioException) {
            throw new IllegalStateException("Unable to load windows-manager dialog FXML", ioException);
        }

        controller = loader.getController();
        controller.bind(model);

        closeSelectedButtonType = new ButtonType(I18n.tr("IDS_WINDOWS_MANAGER_CLOSE_SELECTED"), ButtonBar.ButtonData.LEFT);

        setDialogPane(dialogPane);
        setTitle(I18n.tr("IDS_WINDOWS_MANAGER_TITLE"));
        dialogPane.getButtonTypes().setAll(ButtonType.OK, closeSelectedButtonType, ButtonType.CANCEL);

        Node okButton = dialogPane.lookupButton(ButtonType.OK);
        if (okButton != null) {
            okButton.disableProperty().bind(controller.canSubmitProperty().not());
        }

        setResultConverter(buttonType -> {
            if (buttonType == ButtonType.OK) {
                return controller.selectedIndex().isPresent() ? controller.selectedIndex().getAsInt() : null;
            }
            if (buttonType == closeSelectedButtonType) {
                return -1;
            }
            return null;
        });
    }
}
