package org.winmerge.desktop.ui.dialogs;

import java.io.File;
import java.io.IOException;
import java.util.Objects;
import java.util.Optional;
import java.util.ResourceBundle;

import javafx.fxml.FXMLLoader;
import javafx.scene.Node;
import javafx.scene.control.Alert;
import javafx.scene.control.ButtonType;
import javafx.scene.control.Dialog;
import javafx.scene.control.DialogPane;
import javafx.stage.Window;
import org.winmerge.desktop.i18n.I18n;

public final class DirCompareReportDialog extends Dialog<DirCompareReportResult> {
    private final DirCompareReportDialogController controller;

    public DirCompareReportDialog(Window owner, DirCompareReportRequest request) {
        Objects.requireNonNull(request, "request");

        if (owner != null) {
            initOwner(owner);
        }

        FXMLLoader loader = new FXMLLoader(
            getClass().getResource("/org/winmerge/desktop/ui/dialogs/DirCompareReportDialogPane.fxml"),
            ResourceBundle.getBundle("i18n.WinMerge")
        );
        final DialogPane dialogPane;
        try {
            dialogPane = loader.load();
        } catch (IOException ioException) {
            throw new IllegalStateException("Unable to load dir-compare-report dialog FXML", ioException);
        }

        controller = loader.getController();
        controller.bind(request);

        setDialogPane(dialogPane);
        setTitle(I18n.tr("IDS_DIR_COMPARE_REPORT_TITLE"));
        dialogPane.getButtonTypes().setAll(ButtonType.OK, ButtonType.CANCEL);

        Node okButton = dialogPane.lookupButton(ButtonType.OK);
        if (okButton != null) {
            okButton.disableProperty().bind(controller.canSubmitProperty().not());
        }

        setResultConverter(
            buttonType -> {
                if (buttonType != ButtonType.OK) {
                    return null;
                }
                Optional<DirCompareReportResult> result = controller.buildResult();
                if (result.isEmpty()) {
                    return null;
                }
                String reportFilePath = result.get().reportFilePath();
                if (reportFilePath != null && !reportFilePath.isBlank()) {
                    File reportFile = new File(reportFilePath);
                    if (reportFile.exists()) {
                        Alert confirm = new Alert(Alert.AlertType.WARNING,
                            I18n.tr("IDS_REPORT_FILEOVERWRITE"),
                            ButtonType.YES, ButtonType.NO);
                        confirm.setTitle(I18n.tr("IDS_DIR_COMPARE_REPORT_TITLE"));
                        confirm.setHeaderText(null);
                        if (owner != null) {
                            confirm.initOwner(owner);
                        }
                        Optional<ButtonType> choice = confirm.showAndWait();
                        if (choice.isEmpty() || choice.get() != ButtonType.YES) {
                            return null;
                        }
                    }
                }
                return result.get();
            }
        );
    }
}
