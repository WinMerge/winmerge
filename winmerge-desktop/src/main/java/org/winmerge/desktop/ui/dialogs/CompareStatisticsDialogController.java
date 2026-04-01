package org.winmerge.desktop.ui.dialogs;

import java.util.Objects;

import javafx.fxml.FXML;
import javafx.scene.control.Label;

public final class CompareStatisticsDialogController {
    @FXML
    private Label leftRootLabel;

    @FXML
    private Label rightRootLabel;

    @FXML
    private Label onlyLeftFilesLabel;

    @FXML
    private Label onlyRightFilesLabel;

    @FXML
    private Label differentFilesLabel;

    @FXML
    private Label identicalFilesLabel;

    @FXML
    private Label totalFilesLabel;

    @FXML
    private void initialize() {
        requireInjected(leftRootLabel, "leftRootLabel");
        requireInjected(rightRootLabel, "rightRootLabel");
        requireInjected(onlyLeftFilesLabel, "onlyLeftFilesLabel");
        requireInjected(onlyRightFilesLabel, "onlyRightFilesLabel");
        requireInjected(differentFilesLabel, "differentFilesLabel");
        requireInjected(identicalFilesLabel, "identicalFilesLabel");
        requireInjected(totalFilesLabel, "totalFilesLabel");
    }

    public void bind(CompareStatisticsDialogModel model) {
        Objects.requireNonNull(model, "model");
        leftRootLabel.setText(model.leftRootPath());
        rightRootLabel.setText(model.rightRootPath());
        onlyLeftFilesLabel.setText(Long.toString(model.onlyLeftFiles()));
        onlyRightFilesLabel.setText(Long.toString(model.onlyRightFiles()));
        differentFilesLabel.setText(Long.toString(model.differentFiles()));
        identicalFilesLabel.setText(Long.toString(model.identicalFiles()));
        totalFilesLabel.setText(Long.toString(model.totalFiles()));
    }

    private static void requireInjected(Object field, String fieldName) {
        Objects.requireNonNull(field, () -> "Missing @FXML injection for " + fieldName);
    }
}
