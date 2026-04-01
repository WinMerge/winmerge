package org.winmerge.desktop.ui.options.tabs;

import java.util.Objects;

import javafx.beans.binding.Bindings;
import javafx.fxml.FXML;
import javafx.scene.control.CheckBox;
import org.winmerge.desktop.ui.options.AppSettings;

public final class PropGeneralTab implements OptionsTabController {
    @FXML
    private CheckBox automaticRescanCheckBox;

    @FXML
    private void initialize() {
        Objects.requireNonNull(automaticRescanCheckBox, "Missing @FXML injection for automaticRescanCheckBox");
    }

    @Override
    public void bind(AppSettings settings) {
        Objects.requireNonNull(settings, "settings");
        Bindings.bindBidirectional(automaticRescanCheckBox.selectedProperty(), settings.automaticRescanProperty());
    }
}
