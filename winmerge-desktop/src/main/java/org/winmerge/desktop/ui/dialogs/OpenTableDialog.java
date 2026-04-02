package org.winmerge.desktop.ui.dialogs;

import java.io.IOException;
import java.util.Optional;
import java.util.ResourceBundle;

import javafx.fxml.FXMLLoader;
import javafx.scene.Node;
import javafx.scene.control.ButtonType;
import javafx.scene.control.Dialog;
import javafx.scene.control.DialogPane;
import javafx.stage.Window;
import org.winmerge.desktop.i18n.I18n;

public final class OpenTableDialog extends Dialog<OpenTableResult> {
    private final OpenTableDialogController controller;

    public OpenTableDialog(Window owner, OpenTableRequest request) {
        if (owner != null) {
            initOwner(owner);
        }

        FXMLLoader loader = new FXMLLoader(
            getClass().getResource("/org/winmerge/desktop/ui/dialogs/OpenTableDialogPane.fxml"),
            ResourceBundle.getBundle("i18n.WinMerge")
        );
        final DialogPane dialogPane;
        try {
            dialogPane = loader.load();
        } catch (IOException ioException) {
            throw new IllegalStateException("Unable to load open-table dialog FXML", ioException);
        }

        controller = loader.getController();
        controller.bind(request);

        setDialogPane(dialogPane);
        setTitle(I18n.tr("IDS_OPEN_TABLE_DIALOG_TITLE"));
        dialogPane.getButtonTypes().setAll(ButtonType.OK, ButtonType.CANCEL);

        Node okButton = dialogPane.lookupButton(ButtonType.OK);
        if (okButton != null) {
            okButton.disableProperty().bind(controller.canSubmitProperty().not());
        }

        setResultConverter(buttonType -> {
            if (buttonType != ButtonType.OK) {
                return null;
            }
            Optional<OpenTableResult> result = controller.buildResult();
            return result.orElse(null);
        });
    }
}
