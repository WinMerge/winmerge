package org.winmerge.desktop.ui.dialogs;

import java.io.File;
import java.util.Objects;
import java.util.Optional;

import javafx.beans.property.BooleanProperty;
import javafx.beans.property.SimpleBooleanProperty;
import javafx.fxml.FXML;
import javafx.scene.control.CheckBox;
import javafx.scene.control.ComboBox;
import javafx.scene.control.TextField;
import javafx.stage.FileChooser;
import javafx.stage.Window;
import org.winmerge.desktop.i18n.I18n;

public final class DirCompareReportDialogController {
    @FXML
    private TextField reportFileField;

    @FXML
    private ComboBox<ReportType> styleCombo;

    @FXML
    private CheckBox copyToClipboardCheckBox;

    @FXML
    private CheckBox includeFileCompareReportCheckBox;

    private final BooleanProperty canSubmit = new SimpleBooleanProperty(false);

    @FXML
    private void initialize() {
        requireInjected(reportFileField, "reportFileField");
        requireInjected(styleCombo, "styleCombo");
        requireInjected(copyToClipboardCheckBox, "copyToClipboardCheckBox");
        requireInjected(includeFileCompareReportCheckBox, "includeFileCompareReportCheckBox");

        styleCombo.getItems().setAll(ReportType.values());
        styleCombo.setValue(ReportType.TEXT);
        styleCombo.valueProperty().addListener((obs, oldValue, newValue) -> updateTypeState());

        reportFileField.textProperty().addListener((obs, oldValue, newValue) -> updateCanSubmit());
        copyToClipboardCheckBox.selectedProperty().addListener((obs, oldValue, newValue) -> updateCanSubmit());

        updateTypeState();
        updateCanSubmit();
    }

    public void bind(DirCompareReportRequest request) {
        Objects.requireNonNull(request, "request");

        reportFileField.setText(request.reportFilePath());
        styleCombo.setValue(request.reportType());
        copyToClipboardCheckBox.setSelected(request.copyToClipboard());
        includeFileCompareReportCheckBox.setSelected(request.includeFileCompareReport());

        updateTypeState();
        updateCanSubmit();
    }

    public BooleanProperty canSubmitProperty() {
        return canSubmit;
    }

    public Optional<DirCompareReportResult> buildResult() {
        return DirCompareReportDialogLogic.buildResult(
            reportFileField.getText(),
            styleCombo.getValue(),
            copyToClipboardCheckBox.isSelected(),
            includeFileCompareReportCheckBox.isSelected()
        );
    }

    @FXML
    private void onBrowse() {
        Window window = reportFileField.getScene() == null ? null : reportFileField.getScene().getWindow();
        FileChooser chooser = new FileChooser();
        chooser.setTitle(I18n.tr("IDS_DIR_COMPARE_REPORT_TITLE"));

        ReportType reportType = styleCombo.getValue() == null ? ReportType.TEXT : styleCombo.getValue();
        switch (reportType) {
            case HTML -> chooser.getExtensionFilters().add(new FileChooser.ExtensionFilter("HTML", "*.htm", "*.html"));
            case XML -> chooser.getExtensionFilters().add(new FileChooser.ExtensionFilter("XML", "*.xml"));
            case TEXT -> chooser.getExtensionFilters().add(new FileChooser.ExtensionFilter("Text", "*.csv", "*.asc", "*.rpt", "*.txt"));
            default -> {
            }
        }

        String currentPath = reportFileField.getText().trim();
        if (!currentPath.isEmpty()) {
            File currentFile = new File(currentPath);
            File parent = currentFile.getParentFile();
            if (parent != null && parent.isDirectory()) {
                chooser.setInitialDirectory(parent);
            }
            chooser.setInitialFileName(currentFile.getName());
        }

        File selected = chooser.showSaveDialog(window);
        if (selected != null) {
            reportFileField.setText(selected.getAbsolutePath());
        }
    }

    private void updateTypeState() {
        ReportType reportType = styleCombo.getValue() == null ? ReportType.TEXT : styleCombo.getValue();
        boolean includeEnabled = reportType == ReportType.HTML;
        includeFileCompareReportCheckBox.setDisable(!includeEnabled);
        if (!includeEnabled) {
            includeFileCompareReportCheckBox.setSelected(false);
        }
    }

    private void updateCanSubmit() {
        canSubmit.set(DirCompareReportDialogLogic.canSubmit(reportFileField.getText(), copyToClipboardCheckBox.isSelected()));
    }

    private static void requireInjected(Object field, String fieldName) {
        Objects.requireNonNull(field, () -> "Missing @FXML injection for " + fieldName);
    }
}
