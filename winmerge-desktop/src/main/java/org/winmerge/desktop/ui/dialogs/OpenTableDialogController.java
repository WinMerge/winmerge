package org.winmerge.desktop.ui.dialogs;

import java.util.Objects;
import java.util.Optional;

import javafx.beans.property.BooleanProperty;
import javafx.beans.property.SimpleBooleanProperty;
import javafx.fxml.FXML;
import javafx.scene.control.CheckBox;
import javafx.scene.control.Label;
import javafx.scene.control.RadioButton;
import javafx.scene.control.TextField;
import javafx.scene.control.ToggleGroup;
import org.winmerge.desktop.i18n.I18n;

public final class OpenTableDialogController {
    @FXML
    private ToggleGroup fileTypeToggleGroup;

    @FXML
    private RadioButton csvRadioButton;

    @FXML
    private RadioButton dsvRadioButton;

    @FXML
    private RadioButton customRadioButton;

    @FXML
    private TextField csvDelimiterField;

    @FXML
    private TextField dsvDelimiterField;

    @FXML
    private TextField customDelimiterField;

    @FXML
    private TextField quoteCharacterField;

    @FXML
    private CheckBox allowNewlinesInQuotesCheckBox;

    @FXML
    private Label validationLabel;

    private final BooleanProperty canSubmit = new SimpleBooleanProperty(false);

    @FXML
    private void initialize() {
        requireInjected(fileTypeToggleGroup, "fileTypeToggleGroup");
        requireInjected(csvRadioButton, "csvRadioButton");
        requireInjected(dsvRadioButton, "dsvRadioButton");
        requireInjected(customRadioButton, "customRadioButton");
        requireInjected(csvDelimiterField, "csvDelimiterField");
        requireInjected(dsvDelimiterField, "dsvDelimiterField");
        requireInjected(customDelimiterField, "customDelimiterField");
        requireInjected(quoteCharacterField, "quoteCharacterField");
        requireInjected(allowNewlinesInQuotesCheckBox, "allowNewlinesInQuotesCheckBox");
        requireInjected(validationLabel, "validationLabel");

        csvRadioButton.setUserData(OpenTableDialogLogic.FILE_TYPE_CSV);
        dsvRadioButton.setUserData(OpenTableDialogLogic.FILE_TYPE_DSV);
        customRadioButton.setUserData(OpenTableDialogLogic.FILE_TYPE_CUSTOM);

        fileTypeToggleGroup.selectedToggleProperty().addListener((obs, oldValue, newValue) -> updateViewState());
        csvDelimiterField.textProperty().addListener((obs, oldValue, newValue) -> updateCanSubmit());
        dsvDelimiterField.textProperty().addListener((obs, oldValue, newValue) -> updateCanSubmit());
        customDelimiterField.textProperty().addListener((obs, oldValue, newValue) -> updateCanSubmit());
        quoteCharacterField.textProperty().addListener((obs, oldValue, newValue) -> updateCanSubmit());

        validationLabel.setText("");
        validationLabel.setVisible(false);
        if (fileTypeToggleGroup.getSelectedToggle() == null) {
            fileTypeToggleGroup.selectToggle(csvRadioButton);
        }
        updateViewState();
    }

    public void bind(OpenTableRequest request) {
        Objects.requireNonNull(request, "request");

        String fileType = OpenTableDialogLogic.normalizeFileType(request.fileType());
        switch (fileType) {
            case OpenTableDialogLogic.FILE_TYPE_DSV -> fileTypeToggleGroup.selectToggle(dsvRadioButton);
            case OpenTableDialogLogic.FILE_TYPE_CUSTOM -> fileTypeToggleGroup.selectToggle(customRadioButton);
            default -> fileTypeToggleGroup.selectToggle(csvRadioButton);
        }

        csvDelimiterField.setText(request.csvDelimiter());
        dsvDelimiterField.setText(request.dsvDelimiter());
        customDelimiterField.setText(request.customDelimiter());
        quoteCharacterField.setText(request.quoteCharacter());
        allowNewlinesInQuotesCheckBox.setSelected(request.allowNewlinesInQuotes());

        updateViewState();
    }

    public BooleanProperty canSubmitProperty() {
        return canSubmit;
    }

    public Optional<OpenTableResult> buildResult() {
        return OpenTableDialogLogic.buildResult(
            selectedFileType(),
            csvDelimiterField.getText(),
            dsvDelimiterField.getText(),
            customDelimiterField.getText(),
            quoteCharacterField.getText(),
            allowNewlinesInQuotesCheckBox.isSelected()
        );
    }

    private void updateViewState() {
        String fileType = selectedFileType();
        csvDelimiterField.setDisable(!OpenTableDialogLogic.FILE_TYPE_CSV.equals(fileType));
        dsvDelimiterField.setDisable(!OpenTableDialogLogic.FILE_TYPE_DSV.equals(fileType));
        customDelimiterField.setDisable(!OpenTableDialogLogic.FILE_TYPE_CUSTOM.equals(fileType));
        updateCanSubmit();
    }

    private void updateCanSubmit() {
        String validationKey = OpenTableDialogLogic.validationMessage(
            selectedFileType(),
            csvDelimiterField.getText(),
            dsvDelimiterField.getText(),
            customDelimiterField.getText(),
            quoteCharacterField.getText()
        );

        if (validationKey.isEmpty()) {
            validationLabel.setVisible(false);
            validationLabel.setText("");
        } else {
            validationLabel.setVisible(true);
            validationLabel.setText(I18n.tr(validationKey));
        }

        canSubmit.set(
            OpenTableDialogLogic.canSubmit(
                selectedFileType(),
                csvDelimiterField.getText(),
                dsvDelimiterField.getText(),
                customDelimiterField.getText(),
                quoteCharacterField.getText()
            )
        );
    }

    private String selectedFileType() {
        if (fileTypeToggleGroup.getSelectedToggle() == null) {
            return OpenTableDialogLogic.FILE_TYPE_CSV;
        }
        Object value = fileTypeToggleGroup.getSelectedToggle().getUserData();
        return value == null ? OpenTableDialogLogic.FILE_TYPE_CSV : value.toString();
    }

    private static void requireInjected(Object field, String fieldName) {
        Objects.requireNonNull(field, () -> "Missing @FXML injection for " + fieldName);
    }
}
