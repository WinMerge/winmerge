package org.winmerge.desktop.ui.options.tabs;

import java.util.Objects;

import javafx.beans.binding.Bindings;
import javafx.fxml.FXML;
import javafx.scene.control.CheckBox;
import javafx.scene.control.Spinner;
import javafx.scene.control.SpinnerValueFactory;
import org.winmerge.desktop.ui.options.AppSettings;

public final class PropEditorTab implements OptionsTabController {
    @FXML
    private Spinner<Integer> tabSizeSpinner;
    @FXML
    private Spinner<Integer> tabTypeSpinner;
    @FXML
    private Spinner<Integer> renderingModeSpinner;
    @FXML
    private CheckBox syntaxHighlightCheckBox;
    @FXML
    private CheckBox allowMixedEolCheckBox;

    @FXML
    private void initialize() {
        Objects.requireNonNull(tabSizeSpinner, "Missing @FXML injection for tabSizeSpinner");
        Objects.requireNonNull(tabTypeSpinner, "Missing @FXML injection for tabTypeSpinner");
        Objects.requireNonNull(renderingModeSpinner, "Missing @FXML injection for renderingModeSpinner");
        Objects.requireNonNull(syntaxHighlightCheckBox, "Missing @FXML injection for syntaxHighlightCheckBox");
        Objects.requireNonNull(allowMixedEolCheckBox, "Missing @FXML injection for allowMixedEolCheckBox");

        tabSizeSpinner.setValueFactory(new SpinnerValueFactory.IntegerSpinnerValueFactory(1, 16, 4));
        tabTypeSpinner.setValueFactory(new SpinnerValueFactory.IntegerSpinnerValueFactory(0, 1, 0));
        renderingModeSpinner.setValueFactory(new SpinnerValueFactory.IntegerSpinnerValueFactory(0, 6, 0));
        tabSizeSpinner.setEditable(true);
        tabTypeSpinner.setEditable(true);
        renderingModeSpinner.setEditable(true);
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

        bindSpinner(tabTypeSpinner, settings, "options.editor.tabType", 0);
        bindSpinner(renderingModeSpinner, settings, "options.editor.renderingMode", 0);

        Bindings.bindBidirectional(
            syntaxHighlightCheckBox.selectedProperty(),
            settings.booleanProperty("options.editor.syntaxHighlight", false)
        );
        Bindings.bindBidirectional(
            allowMixedEolCheckBox.selectedProperty(),
            settings.booleanProperty("options.editor.allowMixedEol", false)
        );
    }

    private static void bindSpinner(Spinner<Integer> spinner, AppSettings settings, String key, int defaultValue) {
        spinner.getValueFactory().setValue(settings.getInteger(key, defaultValue));
        settings.integerProperty(key, defaultValue).addListener((obs, oldValue, newValue) -> {
            if (spinner.getValue() == null || spinner.getValue() != newValue.intValue()) {
                spinner.getValueFactory().setValue(newValue.intValue());
            }
        });
        spinner.valueProperty().addListener((obs, oldValue, newValue) -> {
            if (newValue != null) {
                settings.setInteger(key, defaultValue, newValue);
            }
        });
    }
}
