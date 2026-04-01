package org.winmerge.desktop.ui.options;

import java.io.IOException;
import java.util.Objects;

import javafx.event.ActionEvent;
import javafx.fxml.FXMLLoader;
import javafx.scene.Node;
import javafx.scene.control.ButtonBar;
import javafx.scene.control.ButtonType;
import javafx.scene.control.Dialog;
import javafx.scene.control.DialogPane;
import javafx.stage.Window;

public final class OptionsDialog extends Dialog<ButtonType> {
    private static final ButtonType APPLY_BUTTON_TYPE = new ButtonType("Apply", ButtonBar.ButtonData.APPLY);

    public OptionsDialog(Window owner, AppSettings settings, Runnable onApply) {
        Objects.requireNonNull(settings, "settings");
        Objects.requireNonNull(onApply, "onApply");

        if (owner != null) {
            initOwner(owner);
        }

        DialogPane dialogPane = loadDialogPane(settings);
        setDialogPane(dialogPane);
        setTitle("Options");
        setResizable(true);

        dialogPane.getButtonTypes().setAll(ButtonType.OK, ButtonType.CANCEL, APPLY_BUTTON_TYPE);
        Node applyButton = dialogPane.lookupButton(APPLY_BUTTON_TYPE);
        if (applyButton != null) {
            applyButton.addEventFilter(ActionEvent.ACTION, event -> {
                onApply.run();
                event.consume();
            });
        }
    }

    private DialogPane loadDialogPane(AppSettings settings) {
        FXMLLoader loader = new FXMLLoader(getClass().getResource("/org/winmerge/desktop/ui/options/OptionsDialogPane.fxml"));
        final DialogPane dialogPane;
        try {
            dialogPane = loader.load();
        } catch (IOException ioException) {
            throw new IllegalStateException("Unable to load options dialog FXML", ioException);
        }

        OptionsDialogController controller = loader.getController();
        controller.bind(settings);
        return dialogPane;
    }
}
