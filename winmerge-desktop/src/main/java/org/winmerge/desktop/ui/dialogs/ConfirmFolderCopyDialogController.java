package org.winmerge.desktop.ui.dialogs;

import java.util.Objects;

import javafx.fxml.FXML;
import javafx.scene.control.Label;
import org.winmerge.desktop.i18n.I18n;

public final class ConfirmFolderCopyDialogController {
    @FXML
    private Label questionLabel;

    @FXML
    private Label fromTextLabel;

    @FXML
    private Label fromPathLabel;

    @FXML
    private Label toTextLabel;

    @FXML
    private Label toPathLabel;

    @FXML
    private void initialize() {
        requireInjected(questionLabel, "questionLabel");
        requireInjected(fromTextLabel, "fromTextLabel");
        requireInjected(fromPathLabel, "fromPathLabel");
        requireInjected(toTextLabel, "toTextLabel");
        requireInjected(toPathLabel, "toPathLabel");
    }

    public void bind(ConfirmFolderCopyRequest request) {
        Objects.requireNonNull(request, "request");

        questionLabel.setText(request.question().isBlank() ? I18n.tr("IDS_CONFIRM_SINGLE_COPY") : request.question());
        fromTextLabel.setText(request.fromText().isBlank() ? I18n.tr("IDS_FROM_LEFT") : request.fromText());
        fromPathLabel.setText(request.fromPath());
        toTextLabel.setText(request.toText().isBlank() ? I18n.tr("IDS_TO_RIGHT") : request.toText());
        toPathLabel.setText(request.toPath());
    }

    private static void requireInjected(Object field, String fieldName) {
        Objects.requireNonNull(field, () -> "Missing @FXML injection for " + fieldName);
    }
}
