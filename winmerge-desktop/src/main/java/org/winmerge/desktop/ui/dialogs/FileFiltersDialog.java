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

public final class FileFiltersDialog extends Dialog<ButtonType> {
    public FileFiltersDialog(Window owner, FileFiltersDialogModel model) {
        Objects.requireNonNull(model, "model");

        if (owner != null) {
            initOwner(owner);
        }

        LoadedContent content = loadContent(model, owner);

        DialogPane dialogPane = new DialogPane();
        dialogPane.setContent(content.root());
        dialogPane.getButtonTypes().setAll(ButtonType.OK, ButtonType.CANCEL);

        setDialogPane(dialogPane);
        setTitle(I18n.tr("IDS_FILTER_TITLE"));
        setOnHidden(event -> model.cancelDirWatcher());

        setResultConverter(buttonType -> {
            if (buttonType == ButtonType.OK) {
                content.controller().flushToModel();
            }
            return buttonType;
        });
    }

    static LoadedContent loadContent(FileFiltersDialogModel model, Window owner) {
        FXMLLoader loader = new FXMLLoader(FileFiltersDialog.class.getResource(
            "/org/winmerge/desktop/ui/dialogs/FileFiltersDialogPane.fxml"
        ), ResourceBundle.getBundle("i18n.WinMerge"));
        final VBox root;
        try {
            root = loader.load();
        } catch (IOException ioException) {
            throw new IllegalStateException("Unable to load file-filters dialog FXML", ioException);
        }

        FileFiltersDialogController controller = loader.getController();
        controller.bind(model);
        controller.setSharedFilterChooser(() -> new SharedFilterDialog(owner, SharedFilterDialog.FilterType.PRIVATE).showAndWait());
        controller.setTestFilterAction(entry -> {
            TestFilterModel testModel = model.testFilterModel();
            testModel.setFilterName(entry.getName());
            new TestFilterDialog(owner, testModel).showAndWait();
        });
        return new LoadedContent(root, controller);
    }

    record LoadedContent(VBox root, FileFiltersDialogController controller) {
    }
}
