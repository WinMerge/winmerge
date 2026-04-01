package org.winmerge.desktop.ui.dialogs;

import java.io.IOException;
import java.util.Objects;

import javafx.fxml.FXMLLoader;
import javafx.scene.control.ButtonType;
import javafx.scene.control.Dialog;
import javafx.scene.control.DialogPane;
import javafx.stage.Window;

public final class CompareStatisticsDialog extends Dialog<ButtonType> {
    public CompareStatisticsDialog(Window owner, CompareStatisticsDialogModel model) {
        Objects.requireNonNull(model, "model");

        if (owner != null) {
            initOwner(owner);
        }

        FXMLLoader loader = new FXMLLoader(
            getClass().getResource("/org/winmerge/desktop/ui/dialogs/CompareStatisticsDialogPane.fxml")
        );
        final DialogPane dialogPane;
        try {
            dialogPane = loader.load();
        } catch (IOException ioException) {
            throw new IllegalStateException("Unable to load compare-statistics dialog FXML", ioException);
        }

        CompareStatisticsDialogController controller = loader.getController();
        controller.bind(model);

        setDialogPane(dialogPane);
        setTitle("Compare Statistics");
        dialogPane.getButtonTypes().setAll(ButtonType.OK);
    }
}
