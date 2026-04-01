package org.winmerge.desktop.ui.options.tabs;

import java.util.Objects;

import javafx.fxml.FXML;
import javafx.scene.control.Spinner;
import javafx.scene.control.SpinnerValueFactory;
import org.winmerge.desktop.ui.options.AppSettings;

public final class PropEditorTab implements OptionsTabController {
    @FXML
    private Spinner<Integer> tabSizeSpinner;

    @FXML
    private void initialize() {
        Objects.requireNonNull(tabSizeSpinner, "Missing @FXML injection for tabSizeSpinner");
        tabSizeSpinner.setValueFactory(new SpinnerValueFactory.IntegerSpinnerValueFactory(1, 16, 4));
        tabSizeSpinner.setEditable(true);
    }

    @Override
    public void bind(AppSettings settings) {
        Objects.requireNonNull(settings, "settings");
        tabSizeSpinner.getValueFactory().setValue(settings.getEditorTabSize());
        tabSizeSpinner.valueProperty().addListener((obs, oldValue, newValue) -> {
            if (newValue != null) {
                settings.setEditorTabSize(newValue);
            }
        });
    }
}
