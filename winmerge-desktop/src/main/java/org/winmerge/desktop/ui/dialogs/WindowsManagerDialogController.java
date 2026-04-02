package org.winmerge.desktop.ui.dialogs;

import java.util.Objects;
import java.util.OptionalInt;

import javafx.beans.property.BooleanProperty;
import javafx.beans.property.SimpleBooleanProperty;
import javafx.fxml.FXML;
import javafx.scene.control.ListView;

public final class WindowsManagerDialogController {
    @FXML
    private ListView<String> windowsListView;

    private final BooleanProperty canSubmit = new SimpleBooleanProperty(false);

    @FXML
    private void initialize() {
        requireInjected(windowsListView, "windowsListView");
        windowsListView.getSelectionModel().selectedIndexProperty().addListener((obs, oldValue, newValue) -> updateCanSubmit());
        updateCanSubmit();
    }

    public void bind(WindowsManagerModel model) {
        Objects.requireNonNull(model, "model");
        windowsListView.getItems().setAll(model.windowTitles());
        if (!model.windowTitles().isEmpty()) {
            windowsListView.getSelectionModel().select(model.activeIndex());
        }
        updateCanSubmit();
    }

    public BooleanProperty canSubmitProperty() {
        return canSubmit;
    }

    public OptionalInt selectedIndex() {
        int selected = windowsListView.getSelectionModel().getSelectedIndex();
        if (selected < 0) {
            return OptionalInt.empty();
        }
        return OptionalInt.of(selected);
    }

    private void updateCanSubmit() {
        canSubmit.set(windowsListView.getSelectionModel().getSelectedIndex() >= 0);
    }

    private static void requireInjected(Object field, String fieldName) {
        Objects.requireNonNull(field, () -> "Missing @FXML injection for " + fieldName);
    }
}
