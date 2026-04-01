package org.winmerge.desktop.ui.dialogs;

import java.io.IOException;
import java.util.Optional;

import javafx.fxml.FXMLLoader;
import javafx.scene.Node;
import javafx.scene.control.ButtonType;
import javafx.scene.control.Dialog;
import javafx.scene.control.DialogPane;
import javafx.stage.Window;

public final class WMGotoDialog extends Dialog<WMGotoDialogResult> {
    private final WMGotoDialogController controller;

    public WMGotoDialog(Window owner, WMGotoDialogRequest request) {
        if (owner != null) {
            initOwner(owner);
        }

        FXMLLoader loader = new FXMLLoader(getClass().getResource("/org/winmerge/desktop/ui/dialogs/WMGotoDialogPane.fxml"));
        final DialogPane dialogPane;
        try {
            dialogPane = loader.load();
        } catch (IOException ioException) {
            throw new IllegalStateException("Unable to load goto dialog FXML", ioException);
        }

        controller = loader.getController();
        controller.bind(request);

        setDialogPane(dialogPane);
        setTitle("Go to");
        dialogPane.getButtonTypes().setAll(ButtonType.OK, ButtonType.CANCEL);

        Node okButton = dialogPane.lookupButton(ButtonType.OK);
        if (okButton != null) {
            okButton.disableProperty().bind(controller.canSubmitProperty().not());
        }

        setResultConverter(buttonType -> {
            if (buttonType != ButtonType.OK) {
                return null;
            }
            Optional<WMGotoDialogResult> result = controller.buildResult();
            return result.orElse(null);
        });
    }
}
