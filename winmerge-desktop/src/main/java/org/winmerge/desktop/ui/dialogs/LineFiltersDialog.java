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

public final class LineFiltersDialog extends Dialog<ButtonType> {
    public LineFiltersDialog(Window owner, LineFiltersDialogModel model) {
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

    static LoadedContent loadContent(LineFiltersDialogModel model) {
        FXMLLoader loader = new FXMLLoader(
            getClassLoaderResource("/org/winmerge/desktop/ui/dialogs/LineFiltersDialogPane.fxml"),
            ResourceBundle.getBundle("i18n.WinMerge")
        );
        final VBox root;
        try {
            root = loader.load();
        } catch (IOException ioException) {
            throw new IllegalStateException("Unable to load line-filters dialog FXML", ioException);
        }

        LineFiltersDialogController controller = loader.getController();
        controller.bind(model);
        return new LoadedContent(root, controller);
    }

    private static java.net.URL getClassLoaderResource(String path) {
        java.net.URL resource = LineFiltersDialog.class.getResource(path);
        if (resource == null) {
            throw new IllegalStateException("Missing FXML resource: " + path);
        }
        return resource;
    }

    record LoadedContent(VBox root, LineFiltersDialogController controller) {
    }
}
