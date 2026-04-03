package org.winmerge.desktop.ui.dialogs;

import java.io.IOException;
import java.util.Objects;
import java.util.ResourceBundle;

import javafx.fxml.FXMLLoader;
import javafx.scene.control.ButtonType;
import javafx.scene.control.Dialog;
import javafx.scene.control.DialogPane;
import javafx.scene.layout.VBox;
import javafx.stage.Window;
import org.winmerge.desktop.i18n.I18n;

public final class SubstitutionFiltersDialog extends Dialog<ButtonType> {
    public SubstitutionFiltersDialog(Window owner, SubstitutionFiltersDialogModel model) {
        Objects.requireNonNull(model, "model");

        if (owner != null) {
            initOwner(owner);
        }

        LoadedContent content = loadContent(model);

        DialogPane dialogPane = new DialogPane();
        dialogPane.setContent(content.root());
        dialogPane.getButtonTypes().setAll(ButtonType.OK, ButtonType.CANCEL);

        setDialogPane(dialogPane);
        setTitle(I18n.tr("IDS_FILTER_TITLE"));
        setResultConverter(buttonType -> {
            if (buttonType == ButtonType.OK) {
                content.controller().flushToModel();
            }
            return buttonType;
        });
    }

    static LoadedContent loadContent(SubstitutionFiltersDialogModel model) {
        FXMLLoader loader = new FXMLLoader(SubstitutionFiltersDialog.class.getResource(
            "/org/winmerge/desktop/ui/dialogs/SubstitutionFiltersDialogPane.fxml"
        ), ResourceBundle.getBundle("i18n.WinMerge"));
        final VBox root;
        try {
            root = loader.load();
        } catch (IOException ioException) {
            throw new IllegalStateException("Unable to load substitution-filters dialog FXML", ioException);
        }

        SubstitutionFiltersDialogController controller = loader.getController();
        controller.bind(model);
        return new LoadedContent(root, controller);
    }

    record LoadedContent(VBox root, SubstitutionFiltersDialogController controller) {
    }
}
