package org.winmerge.desktop.ui.dialogs;

import java.util.Objects;

import javafx.fxml.FXML;
import javafx.scene.control.CheckBox;
import javafx.scene.control.Label;
import javafx.scene.control.TextArea;
import javafx.scene.control.TextField;
import org.winmerge.desktop.i18n.I18n;

public final class TestFilterDialogController {
    @FXML
    private Label filterNameLabel;

    @FXML
    private TextField testTextField;

    @FXML
    private CheckBox isDirectoryCheck;

    @FXML
    private TextArea resultTextArea;

    private TestFilterModel model;

    @FXML
    private void initialize() {
        requireInjected(filterNameLabel, "filterNameLabel");
        requireInjected(testTextField, "testTextField");
        requireInjected(isDirectoryCheck, "isDirectoryCheck");
        requireInjected(resultTextArea, "resultTextArea");
    }

    public void bind(TestFilterModel model) {
        this.model = Objects.requireNonNull(model, "model");
        filterNameLabel.setText(model.filterName());
        resultTextArea.clear();
    }

    @FXML
    private void onTestClicked() {
        if (model == null) {
            return;
        }
        String input = testTextField.getText() == null ? "" : testTextField.getText();
        boolean passed = model.testInput(input, isDirectoryCheck.isSelected());
        String status = I18n.tr(passed ? "IDS_TEST_FILTER_STATUS_PASSED" : "IDS_TEST_FILTER_STATUS_FAILED");
        String output = I18n.tr("IDS_TEST_FILTER_RESULT_FMT", input, status);

        if (resultTextArea.getText().isBlank()) {
            resultTextArea.setText(output);
        } else {
            resultTextArea.appendText(System.lineSeparator() + output);
        }
    }

    private static void requireInjected(Object field, String fieldName) {
        Objects.requireNonNull(field, () -> "Missing @FXML injection for " + fieldName);
    }
}
