package org.winmerge.desktop.ui.dialogs;

import java.nio.file.Path;
import java.util.Objects;

import javafx.fxml.FXML;
import javafx.scene.control.Label;
import org.winmerge.desktop.i18n.I18n;

public final class SaveClosingDialogController {
    @FXML
    private Label promptLabel;

    @FXML
    private Label filePathLabel;

    @FXML
    private void initialize() {
        requireInjected(promptLabel, "promptLabel");
        requireInjected(filePathLabel, "filePathLabel");
    }

    public void bind(Path filePath) {
        Objects.requireNonNull(filePath, "filePath");

        String pathText = filePath.toAbsolutePath().normalize().toString();
        String localizedPrompt = I18n.tr("IDS_SAVE_FMT", pathText);
        if ("IDS_SAVE_FMT".equals(localizedPrompt)) {
            localizedPrompt = "Save changes to " + pathText + "?";
        }

        promptLabel.setText(localizedPrompt);
        filePathLabel.setText(pathText);
    }

    private static void requireInjected(Object field, String fieldName) {
        Objects.requireNonNull(field, () -> "Missing @FXML injection for " + fieldName);
    }
}
