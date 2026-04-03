package org.winmerge.desktop.ui.dialogs;

import java.io.IOException;
import java.util.Objects;
import java.util.ResourceBundle;

import javafx.fxml.FXMLLoader;
import javafx.scene.Node;
import javafx.scene.control.ButtonType;
import javafx.scene.control.Dialog;
import javafx.scene.control.DialogPane;
import javafx.scene.layout.StackPane;
import javafx.stage.Window;
import org.winmerge.desktop.i18n.I18n;

public final class FilterSettingsDialog extends Dialog<ButtonType> {
    public FilterSettingsDialog(Window owner, FilterSettingsModel model) {
        Objects.requireNonNull(model, "model");

        if (owner != null) {
            initOwner(owner);
        }

        FXMLLoader loader = new FXMLLoader(
            getClass().getResource("/org/winmerge/desktop/ui/dialogs/FilterSettingsDialogPane.fxml"),
            ResourceBundle.getBundle("i18n.WinMerge")
        );
        final DialogPane dialogPane;
        try {
            dialogPane = loader.load();
        } catch (IOException ioException) {
            throw new IllegalStateException("Unable to load filter-settings dialog FXML", ioException);
        }

        StackPane fileTabContainer = lookupRequired(dialogPane, "#fileTabContainer", StackPane.class);
        StackPane lineTabContainer = lookupRequired(dialogPane, "#lineTabContainer", StackPane.class);
        StackPane substitutionTabContainer = lookupRequired(dialogPane, "#substitutionTabContainer", StackPane.class);

        FileFiltersDialog.LoadedContent fileContent = FileFiltersDialog.loadContent(model.fileFiltersModel(), owner);
        LineFiltersDialog.LoadedContent lineContent = LineFiltersDialog.loadContent(model.lineFiltersModel());
        SubstitutionFiltersDialog.LoadedContent substitutionContent =
            SubstitutionFiltersDialog.loadContent(model.substitutionFiltersModel());

        fileTabContainer.getChildren().setAll(fileContent.root());
        lineTabContainer.getChildren().setAll(lineContent.root());
        substitutionTabContainer.getChildren().setAll(substitutionContent.root());

        dialogPane.getButtonTypes().setAll(ButtonType.OK, ButtonType.CANCEL);
        setDialogPane(dialogPane);
        setTitle(I18n.tr("IDS_FILTER_TITLE"));

        setResultConverter(buttonType -> {
            if (buttonType == ButtonType.OK) {
                fileContent.controller().flushToModel();
                lineContent.controller().flushToModel();
                substitutionContent.controller().flushToModel();
            }
            return buttonType;
        });

        setOnHidden(event -> model.fileFiltersModel().cancelDirWatcher());
    }

    private static <T> T lookupRequired(DialogPane root, String selector, Class<T> type) {
        Node node = root.lookup(selector);
        if (node == null || !type.isInstance(node)) {
            throw new IllegalStateException("Missing node for selector: " + selector);
        }
        return type.cast(node);
    }
}
