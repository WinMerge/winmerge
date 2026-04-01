package org.winmerge.desktop.ui.dialogs;

import java.io.IOException;
import java.util.Optional;

import javafx.fxml.FXMLLoader;
import javafx.scene.Node;
import javafx.scene.control.ButtonType;
import javafx.scene.control.Dialog;
import javafx.scene.control.DialogPane;
import javafx.stage.Window;

public final class ComparisonResultFilterDialog extends Dialog<String> {
    private final ComparisonResultFilterDialogController controller;

    public ComparisonResultFilterDialog(Window owner, boolean threeWay) {
        if (owner != null) {
            initOwner(owner);
        }

        FXMLLoader loader = new FXMLLoader(
            getClass().getResource("/org/winmerge/desktop/ui/dialogs/ComparisonResultFilterDialogPane.fxml")
        );
        final DialogPane dialogPane;
        try {
            dialogPane = loader.load();
        } catch (IOException ioException) {
            throw new IllegalStateException("Unable to load comparison-result-filter dialog FXML", ioException);
        }

        controller = loader.getController();
        controller.bind(threeWay);

        setDialogPane(dialogPane);
        setTitle("Filter by Comparison Result");
        dialogPane.getButtonTypes().setAll(ButtonType.OK, ButtonType.CANCEL);

        Node okButton = dialogPane.lookupButton(ButtonType.OK);
        if (okButton != null) {
            okButton.disableProperty().bind(controller.canSubmitProperty().not());
        }

        setResultConverter(buttonType -> {
            if (buttonType != ButtonType.OK) {
                return null;
            }
            Optional<String> result = controller.buildExpression();
            return result.orElse(null);
        });
    }
}
