package org.winmerge.desktop.ui.dialogs;

import java.io.IOException;
import java.util.List;
import java.util.Objects;
import java.util.ResourceBundle;

import javafx.fxml.FXMLLoader;
import javafx.scene.control.ButtonType;
import javafx.scene.control.Dialog;
import javafx.scene.control.DialogPane;
import javafx.stage.Window;
import org.winmerge.desktop.i18n.I18n;

public final class DirAdditionalPropertiesDialog extends Dialog<List<String>> {
    private final DirAdditionalPropertiesController controller;

    public DirAdditionalPropertiesDialog(Window owner, List<DirPropertyNode> nodes) {
        Objects.requireNonNull(nodes, "nodes");

        if (owner != null) {
            initOwner(owner);
        }

        FXMLLoader loader = new FXMLLoader(
            getClass().getResource("/org/winmerge/desktop/ui/dialogs/DirAdditionalPropertiesDialogPane.fxml"),
            ResourceBundle.getBundle("i18n.WinMerge")
        );
        final DialogPane dialogPane;
        try {
            dialogPane = loader.load();
        } catch (IOException ioException) {
            throw new IllegalStateException("Unable to load dir-additional-properties dialog FXML", ioException);
        }

        controller = loader.getController();
        controller.bind(nodes);

        setDialogPane(dialogPane);
        setTitle(I18n.tr("IDS_DIR_ADDITIONAL_PROPERTIES_TITLE"));
        dialogPane.getButtonTypes().setAll(ButtonType.OK, ButtonType.CANCEL);

        setResultConverter(buttonType -> buttonType == ButtonType.OK ? controller.buildResult() : null);
    }
}
