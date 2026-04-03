package org.winmerge.desktop.ui.dialogs;

import java.io.IOException;
import java.util.List;
import java.util.Objects;
import java.util.ResourceBundle;

import javafx.fxml.FXMLLoader;
import javafx.scene.Node;
import javafx.scene.control.ButtonType;
import javafx.scene.control.Dialog;
import javafx.scene.control.DialogPane;
import javafx.stage.Window;
import org.winmerge.desktop.i18n.I18n;

public final class DirColumnsDialog extends Dialog<List<DirColumn>> {
    private final DirColumnsDialogController controller;

    public DirColumnsDialog(Window owner, List<DirColumn> columns) {
        Objects.requireNonNull(columns, "columns");

        if (owner != null) {
            initOwner(owner);
        }

        FXMLLoader loader = new FXMLLoader(
            getClass().getResource("/org/winmerge/desktop/ui/dialogs/DirColumnsDialogPane.fxml"),
            ResourceBundle.getBundle("i18n.WinMerge")
        );
        final DialogPane dialogPane;
        try {
            dialogPane = loader.load();
        } catch (IOException ioException) {
            throw new IllegalStateException("Unable to load dir-columns dialog FXML", ioException);
        }

        controller = loader.getController();
        controller.bind(columns);

        setDialogPane(dialogPane);
        setTitle(I18n.tr("IDS_DIR_COLUMNS_TITLE"));
        dialogPane.getButtonTypes().setAll(ButtonType.OK, ButtonType.CANCEL);

        Node okButton = dialogPane.lookupButton(ButtonType.OK);
        if (okButton != null) {
            okButton.disableProperty().bind(controller.canSubmitProperty().not());
        }

        setResultConverter(buttonType -> buttonType == ButtonType.OK ? controller.buildResult() : null);
    }
}
