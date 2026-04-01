package org.winmerge.desktop.ui.options.tabs;

import java.util.Objects;

import javafx.beans.binding.Bindings;
import javafx.fxml.FXML;
import javafx.scene.control.CheckBox;
import javafx.scene.control.Spinner;
import javafx.scene.control.SpinnerValueFactory;
import javafx.scene.control.TextField;
import org.winmerge.desktop.ui.options.AppSettings;

public final class PropGeneralTab implements OptionsTabController {
    @FXML
    private CheckBox automaticRescanCheckBox;
    @FXML
    private CheckBox scrollToFirstCheckBox;
    @FXML
    private CheckBox scrollToFirstInlineDiffCheckBox;
    @FXML
    private CheckBox verifyOpenPathsCheckBox;
    @FXML
    private CheckBox preserveFileTimesCheckBox;
    @FXML
    private CheckBox showSelectFolderOnStartupCheckBox;
    @FXML
    private CheckBox closeWithOkCheckBox;
    @FXML
    private Spinner<Integer> singleInstanceModeSpinner;
    @FXML
    private Spinner<Integer> autoReloadModeSpinner;
    @FXML
    private TextField selectedLanguageField;

    @FXML
    private void initialize() {
        Objects.requireNonNull(automaticRescanCheckBox, "Missing @FXML injection for automaticRescanCheckBox");
        Objects.requireNonNull(scrollToFirstCheckBox, "Missing @FXML injection for scrollToFirstCheckBox");
        Objects.requireNonNull(scrollToFirstInlineDiffCheckBox, "Missing @FXML injection for scrollToFirstInlineDiffCheckBox");
        Objects.requireNonNull(verifyOpenPathsCheckBox, "Missing @FXML injection for verifyOpenPathsCheckBox");
        Objects.requireNonNull(preserveFileTimesCheckBox, "Missing @FXML injection for preserveFileTimesCheckBox");
        Objects.requireNonNull(showSelectFolderOnStartupCheckBox, "Missing @FXML injection for showSelectFolderOnStartupCheckBox");
        Objects.requireNonNull(closeWithOkCheckBox, "Missing @FXML injection for closeWithOkCheckBox");
        Objects.requireNonNull(singleInstanceModeSpinner, "Missing @FXML injection for singleInstanceModeSpinner");
        Objects.requireNonNull(autoReloadModeSpinner, "Missing @FXML injection for autoReloadModeSpinner");
        Objects.requireNonNull(selectedLanguageField, "Missing @FXML injection for selectedLanguageField");

        singleInstanceModeSpinner.setValueFactory(new SpinnerValueFactory.IntegerSpinnerValueFactory(0, 2, 0));
        autoReloadModeSpinner.setValueFactory(new SpinnerValueFactory.IntegerSpinnerValueFactory(0, 2, 0));
        singleInstanceModeSpinner.setEditable(true);
        autoReloadModeSpinner.setEditable(true);
    }

    @Override
    public void bind(AppSettings settings) {
        Objects.requireNonNull(settings, "settings");
        Bindings.bindBidirectional(automaticRescanCheckBox.selectedProperty(), settings.automaticRescanProperty());
        Bindings.bindBidirectional(
            scrollToFirstCheckBox.selectedProperty(),
            settings.booleanProperty("options.general.scrollToFirst", false)
        );
        Bindings.bindBidirectional(
            scrollToFirstInlineDiffCheckBox.selectedProperty(),
            settings.booleanProperty("options.general.scrollToFirstInlineDiff", false)
        );
        Bindings.bindBidirectional(
            verifyOpenPathsCheckBox.selectedProperty(),
            settings.booleanProperty("options.general.verifyOpenPaths", false)
        );
        Bindings.bindBidirectional(
            preserveFileTimesCheckBox.selectedProperty(),
            settings.booleanProperty("options.general.preserveFileTimes", false)
        );
        Bindings.bindBidirectional(
            showSelectFolderOnStartupCheckBox.selectedProperty(),
            settings.booleanProperty("options.general.showSelectFolderOnStartup", false)
        );
        Bindings.bindBidirectional(
            closeWithOkCheckBox.selectedProperty(),
            settings.booleanProperty("options.general.closeWithOk", true)
        );
        Bindings.bindBidirectional(
            selectedLanguageField.textProperty(),
            settings.stringProperty("options.general.selectedLanguage", "")
        );

        bindSpinner(singleInstanceModeSpinner, settings, "options.general.singleInstanceMode", 0);
        bindSpinner(autoReloadModeSpinner, settings, "options.general.autoReloadMode", 0);
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
