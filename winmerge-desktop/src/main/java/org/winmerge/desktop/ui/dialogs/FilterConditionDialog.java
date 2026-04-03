package org.winmerge.desktop.ui.dialogs;

import java.io.IOException;
import java.util.Objects;
import java.util.Optional;
import java.util.ResourceBundle;

import javafx.fxml.FXMLLoader;
import javafx.scene.Node;
import javafx.scene.control.ButtonType;
import javafx.scene.control.Dialog;
import javafx.scene.control.DialogPane;
import javafx.scene.layout.VBox;
import javafx.stage.Window;
import org.winmerge.desktop.i18n.I18n;

public final class FilterConditionDialog extends Dialog<FilterConditionResult> {
    public FilterConditionDialog(Window owner, FilterConditionRequest request) {
        Objects.requireNonNull(request, "request");

        if (owner != null) {
            initOwner(owner);
        }

        FXMLLoader loader = new FXMLLoader(
            getClass().getResource("/org/winmerge/desktop/ui/dialogs/FilterConditionDialogPane.fxml"),
            ResourceBundle.getBundle("i18n.WinMerge")
        );
        final VBox content;
        try {
            content = loader.load();
        } catch (IOException ioException) {
            throw new IllegalStateException("Unable to load filter-condition dialog FXML", ioException);
        }

        FilterConditionDialogController controller = loader.getController();
        controller.bind(request);

        DialogPane dialogPane = new DialogPane();
        dialogPane.setContent(content);
        dialogPane.getButtonTypes().setAll(ButtonType.OK, ButtonType.CANCEL);

        setDialogPane(dialogPane);
        setTitle(I18n.tr("IDS_FILTER_TITLE"));

        Node okButton = dialogPane.lookupButton(ButtonType.OK);
        if (okButton != null) {
            okButton.disableProperty().bind(controller.canSubmitProperty().not());
        }

        setResultConverter(buttonType -> {
            if (buttonType != ButtonType.OK) {
                return null;
            }
            Optional<FilterConditionResult> result = controller.buildResult();
            return result.orElse(null);
        });
    }
}
