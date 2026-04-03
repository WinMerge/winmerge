package org.winmerge.desktop.ui.dialogs;

import java.io.IOException;
import java.util.Objects;
import java.util.Optional;
import java.util.ResourceBundle;

import javafx.fxml.FXMLLoader;
import javafx.scene.control.ButtonType;
import javafx.scene.control.Dialog;
import javafx.scene.control.DialogPane;
import javafx.stage.Window;
import org.winmerge.desktop.i18n.I18n;

public final class DirSelectFilesDialog extends Dialog<DirSelectFilesResult> {
    private final DirSelectFilesDialogController controller;

    public DirSelectFilesDialog(Window owner, DirSelectFilesRequest request) {
        Objects.requireNonNull(request, "request");

        if (owner != null) {
            initOwner(owner);
        }

        FXMLLoader loader = new FXMLLoader(
            getClass().getResource("/org/winmerge/desktop/ui/dialogs/DirSelectFilesDialogPane.fxml"),
            ResourceBundle.getBundle("i18n.WinMerge")
        );
        final DialogPane dialogPane;
        try {
            dialogPane = loader.load();
        } catch (IOException ioException) {
            throw new IllegalStateException("Unable to load dir-select-files dialog FXML", ioException);
        }

        controller = loader.getController();
        controller.bind(request);

        setDialogPane(dialogPane);
        setTitle(I18n.tr("IDS_DIR_SELECT_FILES_TITLE"));
        dialogPane.getButtonTypes().setAll(ButtonType.OK, ButtonType.CANCEL);

        setResultConverter(
            buttonType -> {
                if (buttonType != ButtonType.OK) {
                    return null;
                }
                Optional<DirSelectFilesResult> result = controller.buildResult();
                return result.orElse(null);
            }
        );
    }
}
