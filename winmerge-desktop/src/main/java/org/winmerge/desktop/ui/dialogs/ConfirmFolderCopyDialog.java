package org.winmerge.desktop.ui.dialogs;

import java.io.IOException;
import java.util.Objects;

import javafx.fxml.FXMLLoader;
import javafx.scene.control.ButtonBar;
import javafx.scene.control.ButtonType;
import javafx.scene.control.Dialog;
import javafx.scene.control.DialogPane;
import javafx.stage.Window;
import org.winmerge.desktop.i18n.I18n;

public final class ConfirmFolderCopyDialog extends Dialog<ConfirmFolderCopyChoice> {
    private static final ButtonType DONT_ASK_AGAIN_BUTTON =
        new ButtonType(I18n.tr("IDS_MESSAGEBOX_DONT_ASK_AGAIN"), ButtonBar.ButtonData.OTHER);

    public ConfirmFolderCopyDialog(Window owner, ConfirmFolderCopyRequest request) {
        Objects.requireNonNull(request, "request");

        if (owner != null) {
            initOwner(owner);
        }

        FXMLLoader loader = new FXMLLoader(getClass().getResource("/org/winmerge/desktop/ui/dialogs/ConfirmFolderCopyDialogPane.fxml"));
        final DialogPane dialogPane;
        try {
            dialogPane = loader.load();
        } catch (IOException ioException) {
            throw new IllegalStateException("Unable to load confirm-folder-copy dialog FXML", ioException);
        }

        ConfirmFolderCopyDialogController controller = loader.getController();
        controller.bind(request);

        setDialogPane(dialogPane);
        setTitle(request.caption().isBlank() ? I18n.tr("IDS_CONFIRM_COPY_CAPTION") : request.caption());
        dialogPane.getButtonTypes().setAll(ButtonType.YES, ButtonType.NO, DONT_ASK_AGAIN_BUTTON);

        setResultConverter(ConfirmFolderCopyDialog::resolveChoice);
    }

    static ConfirmFolderCopyChoice resolveChoice(ButtonType buttonType) {
        if (buttonType == ButtonType.YES) {
            return ConfirmFolderCopyChoice.YES;
        }
        if (buttonType == DONT_ASK_AGAIN_BUTTON) {
            return ConfirmFolderCopyChoice.DONT_ASK_AGAIN;
        }
        return ConfirmFolderCopyChoice.NO;
    }

    static ButtonType dontAskAgainButtonType() {
        return DONT_ASK_AGAIN_BUTTON;
    }
}
