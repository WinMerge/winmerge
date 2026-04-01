package org.winmerge.desktop.ui;

import java.util.Objects;

import javafx.application.Platform;
import javafx.fxml.FXML;
import javafx.scene.control.Label;

public class StatusBarController {
    @FXML
    private Label statusTextLabel;

    @FXML
    private void initialize() {
        requireInjected(statusTextLabel, "statusTextLabel");
    }

    public void setStatusText(String statusText) {
        String nextValue = statusText == null ? "" : statusText;
        if (Platform.isFxApplicationThread()) {
            statusTextLabel.setText(nextValue);
        } else {
            Platform.runLater(() -> statusTextLabel.setText(nextValue));
        }
    }

    private static void requireInjected(Object field, String fieldName) {
        Objects.requireNonNull(field, () -> "Missing @FXML injection for " + fieldName);
    }
}
