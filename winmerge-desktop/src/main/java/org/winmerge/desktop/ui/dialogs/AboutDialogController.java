package org.winmerge.desktop.ui.dialogs;

import java.awt.Desktop;
import java.net.URI;
import java.nio.file.Files;
import java.util.Objects;

import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.control.Hyperlink;
import javafx.scene.control.Label;
import javafx.scene.control.TextArea;

public final class AboutDialogController {
    @FXML
    private Label appNameLabel;

    @FXML
    private Label versionLabel;

    @FXML
    private Hyperlink websiteLink;

    @FXML
    private Label summaryLabel;

    @FXML
    private TextArea asciiArtTextArea;

    @FXML
    private Button openContributorsButton;

    @FXML
    private Label statusLabel;

    private AboutDialogModel model;

    @FXML
    private void initialize() {
        requireInjected(appNameLabel, "appNameLabel");
        requireInjected(versionLabel, "versionLabel");
        requireInjected(websiteLink, "websiteLink");
        requireInjected(summaryLabel, "summaryLabel");
        requireInjected(asciiArtTextArea, "asciiArtTextArea");
        requireInjected(openContributorsButton, "openContributorsButton");
        requireInjected(statusLabel, "statusLabel");
    }

    public void bind(AboutDialogModel model) {
        this.model = Objects.requireNonNull(model, "model");

        appNameLabel.setText(model.appName());
        versionLabel.setText(model.versionText());
        websiteLink.setText(model.websiteLabel());
        summaryLabel.setText(model.summaryText());
        asciiArtTextArea.setText(model.asciiArt());
        asciiArtTextArea.setEditable(false);
        asciiArtTextArea.setFocusTraversable(false);
        statusLabel.setText("");
    }

    @FXML
    private void onOpenWebsite() {
        if (model == null || model.websiteUrl().isBlank()) {
            return;
        }
        try {
            if (!Desktop.isDesktopSupported()) {
                statusLabel.setText("Desktop integration is not available.");
                return;
            }
            Desktop.getDesktop().browse(URI.create(model.websiteUrl()));
            statusLabel.setText("");
        } catch (Exception exception) {
            statusLabel.setText("Unable to open website: " + exception.getMessage());
        }
    }

    @FXML
    private void onOpenContributors() {
        if (model == null) {
            return;
        }
        try {
            if (!Files.exists(model.contributorsPath())) {
                statusLabel.setText("Contributors file not found: " + model.contributorsPath());
                return;
            }
            if (!Desktop.isDesktopSupported()) {
                statusLabel.setText("Desktop integration is not available.");
                return;
            }
            Desktop.getDesktop().open(model.contributorsPath().toFile());
            statusLabel.setText("");
        } catch (Exception exception) {
            statusLabel.setText("Unable to open contributors file: " + exception.getMessage());
        }
    }

    private static void requireInjected(Object field, String fieldName) {
        Objects.requireNonNull(field, () -> "Missing @FXML injection for " + fieldName);
    }
}
