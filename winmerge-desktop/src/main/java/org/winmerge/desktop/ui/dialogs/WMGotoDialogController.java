package org.winmerge.desktop.ui.dialogs;

import java.util.Objects;
import java.util.Optional;
import java.util.OptionalInt;

import javafx.beans.property.BooleanProperty;
import javafx.beans.property.SimpleBooleanProperty;
import javafx.fxml.FXML;
import javafx.scene.control.Label;
import javafx.scene.control.RadioButton;
import javafx.scene.control.TextField;
import javafx.scene.control.ToggleGroup;

public final class WMGotoDialogController {
    @FXML
    private Label rangeLabel;

    @FXML
    private TextField parameterField;

    @FXML
    private RadioButton fileLeftRadio;

    @FXML
    private RadioButton fileMiddleRadio;

    @FXML
    private RadioButton fileRightRadio;

    @FXML
    private RadioButton toLineRadio;

    @FXML
    private RadioButton toDifferenceRadio;

    private final ToggleGroup fileGroup = new ToggleGroup();
    private final ToggleGroup gotoGroup = new ToggleGroup();
    private final BooleanProperty canSubmit = new SimpleBooleanProperty(false);

    private WMGotoDialogRequest request;

    @FXML
    private void initialize() {
        requireInjected(rangeLabel, "rangeLabel");
        requireInjected(parameterField, "parameterField");
        requireInjected(fileLeftRadio, "fileLeftRadio");
        requireInjected(fileMiddleRadio, "fileMiddleRadio");
        requireInjected(fileRightRadio, "fileRightRadio");
        requireInjected(toLineRadio, "toLineRadio");
        requireInjected(toDifferenceRadio, "toDifferenceRadio");

        fileLeftRadio.setToggleGroup(fileGroup);
        fileMiddleRadio.setToggleGroup(fileGroup);
        fileRightRadio.setToggleGroup(fileGroup);
        toLineRadio.setToggleGroup(gotoGroup);
        toDifferenceRadio.setToggleGroup(gotoGroup);

        parameterField.textProperty().addListener((obs, oldValue, newValue) -> updateRangeAndValidation());
        fileGroup.selectedToggleProperty().addListener((obs, oldValue, newValue) -> updateRangeAndValidation());
        gotoGroup.selectedToggleProperty().addListener((obs, oldValue, newValue) -> updateRangeAndValidation());
    }

    public void bind(WMGotoDialogRequest request) {
        this.request = Objects.requireNonNull(request, "request");

        fileMiddleRadio.setDisable(request.fileCount() < 3);
        toDifferenceRadio.setDisable(request.maxDifference() == 0);

        switch (request.selectedFileIndex()) {
            case 1 -> {
                if (request.fileCount() >= 3) {
                    fileMiddleRadio.setSelected(true);
                } else {
                    fileRightRadio.setSelected(true);
                }
            }
            case 2 -> fileRightRadio.setSelected(true);
            default -> fileLeftRadio.setSelected(true);
        }
        if (request.target() == WMGotoTarget.DIFFERENCE && request.maxDifference() > 0) {
            toDifferenceRadio.setSelected(true);
        } else {
            toLineRadio.setSelected(true);
        }

        parameterField.setText(request.initialInput());
        updateRangeAndValidation();
    }

    public BooleanProperty canSubmitProperty() {
        return canSubmit;
    }

    public Optional<WMGotoDialogResult> buildResult() {
        if (request == null) {
            return Optional.empty();
        }
        int selectedFile = selectedFileIndex();
        WMGotoTarget selectedTarget = selectedTarget();
        OptionalInt parsed = WMGotoDialogValidator.parsePositiveInt(parameterField.getText());
        if (!parsed.isPresent()) {
            return Optional.empty();
        }
        int value = parsed.getAsInt();
        int rangeMax = WMGotoDialogValidator.rangeMax(request, selectedFile, selectedTarget);
        if (value > rangeMax) {
            return Optional.empty();
        }
        return Optional.of(new WMGotoDialogResult(selectedFile, selectedTarget, value));
    }

    private void updateRangeAndValidation() {
        if (request == null) {
            rangeLabel.setText("");
            canSubmit.set(false);
            return;
        }

        int selectedFile = selectedFileIndex();
        WMGotoTarget selectedTarget = selectedTarget();
        int rangeMax = WMGotoDialogValidator.rangeMax(request, selectedFile, selectedTarget);
        rangeLabel.setText(rangeMax > 0 ? "(1-" + rangeMax + ")" : "");
        canSubmit.set(WMGotoDialogValidator.canSubmit(parameterField.getText(), request, selectedFile, selectedTarget));
    }

    private int selectedFileIndex() {
        if (fileMiddleRadio.isSelected() && !fileMiddleRadio.isDisable()) {
            return 1;
        }
        if (fileRightRadio.isSelected()) {
            return request != null && request.fileCount() >= 3 ? 2 : 1;
        }
        return 0;
    }

    private WMGotoTarget selectedTarget() {
        return toDifferenceRadio.isSelected() && !toDifferenceRadio.isDisable()
            ? WMGotoTarget.DIFFERENCE
            : WMGotoTarget.LINE;
    }

    private static void requireInjected(Object field, String fieldName) {
        Objects.requireNonNull(field, () -> "Missing @FXML injection for " + fieldName);
    }
}
