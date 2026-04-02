package org.winmerge.desktop.ui.dialogs;

import java.util.LinkedHashSet;
import java.util.List;
import java.util.Objects;
import java.util.Optional;
import java.util.Set;

import javafx.beans.property.BooleanProperty;
import javafx.beans.property.SimpleBooleanProperty;
import javafx.fxml.FXML;
import javafx.scene.control.CheckBox;
import javafx.scene.control.ComboBox;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;
import org.winmerge.desktop.i18n.I18n;

public final class PatchDialogController {
    @FXML
    private ComboBox<String> leftFileCombo;

    @FXML
    private ComboBox<String> rightFileCombo;

    @FXML
    private ComboBox<String> resultFileCombo;

    @FXML
    private ComboBox<String> styleCombo;

    @FXML
    private TextField contextLinesField;

    @FXML
    private CheckBox copyToClipboardCheckBox;

    @FXML
    private CheckBox appendToFileCheckBox;

    @FXML
    private CheckBox openInEditorCheckBox;

    @FXML
    private CheckBox includeCommandLineCheckBox;

    @FXML
    private Label validationLabel;

    private final BooleanProperty canSubmit = new SimpleBooleanProperty(false);

    @FXML
    private void initialize() {
        requireInjected(leftFileCombo, "leftFileCombo");
        requireInjected(rightFileCombo, "rightFileCombo");
        requireInjected(resultFileCombo, "resultFileCombo");
        requireInjected(styleCombo, "styleCombo");
        requireInjected(contextLinesField, "contextLinesField");
        requireInjected(copyToClipboardCheckBox, "copyToClipboardCheckBox");
        requireInjected(appendToFileCheckBox, "appendToFileCheckBox");
        requireInjected(openInEditorCheckBox, "openInEditorCheckBox");
        requireInjected(includeCommandLineCheckBox, "includeCommandLineCheckBox");
        requireInjected(validationLabel, "validationLabel");

        leftFileCombo.setEditable(true);
        rightFileCombo.setEditable(true);
        resultFileCombo.setEditable(true);

        styleCombo.getItems().setAll("Normal", "Context", "Unified");
        styleCombo.setValue("Normal");

        resultFileCombo.getEditor().textProperty().addListener((obs, oldValue, newValue) -> updateCanSubmit());
        contextLinesField.textProperty().addListener((obs, oldValue, newValue) -> updateCanSubmit());

        updateCanSubmit();
    }

    public void bind(PatchDialogRequest request) {
        Objects.requireNonNull(request, "request");

        applyValues(leftFileCombo, collectPaths(request.files(), PatchFiles::leftPath), request.leftPath());
        applyValues(rightFileCombo, collectPaths(request.files(), PatchFiles::rightPath), request.rightPath());
        applyValues(resultFileCombo, collectPaths(request.files(), PatchFiles::resultPath), request.resultPath());

        styleCombo.setValue(request.patchStyle());
        contextLinesField.setText(Integer.toString(request.contextLines()));
        copyToClipboardCheckBox.setSelected(request.copyToClipboard());
        appendToFileCheckBox.setSelected(request.appendToFile());
        openInEditorCheckBox.setSelected(request.openInEditor());
        includeCommandLineCheckBox.setSelected(request.includeCommandLine());

        updateCanSubmit();
    }

    public BooleanProperty canSubmitProperty() {
        return canSubmit;
    }

    public Optional<PatchDialogResult> buildResult() {
        return PatchDialogLogic.buildResult(
            readComboValue(leftFileCombo),
            readComboValue(rightFileCombo),
            readComboValue(resultFileCombo),
            styleCombo.getValue(),
            contextLinesField.getText(),
            copyToClipboardCheckBox.isSelected(),
            appendToFileCheckBox.isSelected(),
            openInEditorCheckBox.isSelected(),
            includeCommandLineCheckBox.isSelected()
        );
    }

    private void updateCanSubmit() {
        String resultPath = readComboValue(resultFileCombo);
        String contextText = contextLinesField.getText();

        String validationKey = PatchDialogLogic.validationMessage(resultPath, contextText);
        if (validationKey.isEmpty()) {
            validationLabel.setVisible(false);
            validationLabel.setText("");
        } else {
            validationLabel.setVisible(true);
            validationLabel.setText(I18n.tr(validationKey));
        }

        canSubmit.set(PatchDialogLogic.canSubmit(resultPath, contextText));
    }

    private static void applyValues(ComboBox<String> combo, List<String> values, String selectedValue) {
        combo.getItems().setAll(values);
        if (selectedValue != null && !selectedValue.isBlank()) {
            combo.setValue(selectedValue);
            combo.getEditor().setText(selectedValue);
        } else if (!values.isEmpty()) {
            combo.setValue(values.get(0));
            combo.getEditor().setText(values.get(0));
        } else {
            combo.getEditor().setText("");
        }
    }

    private static List<String> collectPaths(List<PatchFiles> files, java.util.function.Function<PatchFiles, String> extractor) {
        Set<String> paths = new LinkedHashSet<>();
        if (files != null) {
            for (PatchFiles file : files) {
                if (file == null) {
                    continue;
                }
                String value = extractor.apply(file);
                if (value != null && !value.isBlank()) {
                    paths.add(value.trim());
                }
            }
        }
        return List.copyOf(paths);
    }

    private static String readComboValue(ComboBox<String> combo) {
        String editorValue = combo.getEditor().getText();
        if (editorValue != null && !editorValue.isBlank()) {
            return editorValue.trim();
        }
        String selected = combo.getValue();
        return selected == null ? "" : selected.trim();
    }

    private static void requireInjected(Object field, String fieldName) {
        Objects.requireNonNull(field, () -> "Missing @FXML injection for " + fieldName);
    }
}
