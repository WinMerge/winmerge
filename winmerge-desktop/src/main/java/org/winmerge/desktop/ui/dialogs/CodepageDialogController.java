package org.winmerge.desktop.ui.dialogs;

import java.util.List;
import java.util.Objects;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.CancellationException;
import java.util.concurrent.CompletionException;
import java.util.concurrent.atomic.AtomicBoolean;

import javafx.application.Platform;
import javafx.beans.property.BooleanProperty;
import javafx.beans.property.SimpleBooleanProperty;
import javafx.fxml.FXML;
import javafx.scene.control.CheckBox;
import javafx.scene.control.ComboBox;
import javafx.scene.control.Label;
import org.winmerge.desktop.i18n.I18n;

public final class CodepageDialogController {
    @FXML
    private ComboBox<String> loadCodepageCombo;

    @FXML
    private ComboBox<String> saveCodepageCombo;

    @FXML
    private CheckBox includeBomCheckBox;

    @FXML
    private CheckBox affectLeftCheckBox;

    @FXML
    private CheckBox affectMiddleCheckBox;

    @FXML
    private CheckBox affectRightCheckBox;

    @FXML
    private Label loadingLabel;

    @FXML
    private Label validationLabel;

    private final BooleanProperty canSubmit = new SimpleBooleanProperty(false);
    private final AtomicBoolean closed = new AtomicBoolean(false);
    private CompletableFuture<List<String>> codepagesFuture;

    @FXML
    private void initialize() {
        requireInjected(loadCodepageCombo, "loadCodepageCombo");
        requireInjected(saveCodepageCombo, "saveCodepageCombo");
        requireInjected(includeBomCheckBox, "includeBomCheckBox");
        requireInjected(affectLeftCheckBox, "affectLeftCheckBox");
        requireInjected(affectMiddleCheckBox, "affectMiddleCheckBox");
        requireInjected(affectRightCheckBox, "affectRightCheckBox");
        requireInjected(loadingLabel, "loadingLabel");
        requireInjected(validationLabel, "validationLabel");

        loadCodepageCombo.valueProperty().addListener((obs, oldValue, newValue) -> updateCanSubmit());
        saveCodepageCombo.valueProperty().addListener((obs, oldValue, newValue) -> updateCanSubmit());
        affectLeftCheckBox.selectedProperty().addListener((obs, oldValue, newValue) -> updateCanSubmit());
        affectMiddleCheckBox.selectedProperty().addListener((obs, oldValue, newValue) -> updateCanSubmit());
        affectRightCheckBox.selectedProperty().addListener((obs, oldValue, newValue) -> updateCanSubmit());

        validationLabel.setText("");
        validationLabel.setVisible(false);
        updateCanSubmit();
    }

    public void bind(CodepageRequest request) {
        Objects.requireNonNull(request, "request");

        includeBomCheckBox.setSelected(request.includeBom());
        affectLeftCheckBox.setSelected(request.affectLeft());
        affectMiddleCheckBox.setSelected(request.fileCount() > 2 && request.affectMiddle());
        affectMiddleCheckBox.setVisible(request.fileCount() > 2);
        affectManaged(affectMiddleCheckBox, request.fileCount() > 2);
        affectRightCheckBox.setSelected(request.affectRight());

        loadCodepageCombo.setDisable(true);
        saveCodepageCombo.setDisable(true);
        loadingLabel.setText(I18n.tr("IDS_CODEPAGE_DIALOG_LOADING"));
        loadingLabel.setVisible(true);

        CompletableFuture<List<String>> startedFuture = request.safeCodepagesLoader().get();
        codepagesFuture = startedFuture;
        startedFuture.whenComplete((values, throwable) -> Platform.runLater(() -> applyLoadedCodepages(request, values, throwable)));
    }

    public BooleanProperty canSubmitProperty() {
        return canSubmit;
    }

    public void onDialogClosed() {
        closed.set(true);
        if (codepagesFuture != null && !codepagesFuture.isDone()) {
            codepagesFuture.cancel(true);
        }
    }

    public java.util.Optional<CodepageResult> buildResult() {
        return CodepageDialogLogic.buildResult(
            loadCodepageCombo.getValue(),
            saveCodepageCombo.getValue(),
            includeBomCheckBox.isSelected(),
            affectLeftCheckBox.isSelected(),
            affectMiddleCheckBox.isVisible() && affectMiddleCheckBox.isSelected(),
            affectRightCheckBox.isSelected()
        );
    }

    private void applyLoadedCodepages(CodepageRequest request, List<String> values, Throwable throwable) {
        if (closed.get()) {
            return;
        }

        List<String> codepages = values == null || values.isEmpty() ? request.defaultCodepages() : values;
        Throwable root = unwrap(throwable);
        boolean cancelled = root instanceof CancellationException;

        if (!cancelled) {
            loadCodepageCombo.getItems().setAll(codepages);
            saveCodepageCombo.getItems().setAll(codepages);
            selectValue(loadCodepageCombo, request.loadCodepage());
            selectValue(saveCodepageCombo, request.saveCodepage());
            loadCodepageCombo.setDisable(false);
            saveCodepageCombo.setDisable(false);
            loadingLabel.setVisible(false);
        }

        if (root != null && !cancelled) {
            validationLabel.setText(I18n.tr("IDS_CODEPAGE_DIALOG_LOAD_FAILED"));
            validationLabel.setVisible(true);
        }

        updateCanSubmit();
    }

    private void updateCanSubmit() {
        boolean submit = CodepageDialogLogic.canSubmit(
            loadCodepageCombo.getValue(),
            saveCodepageCombo.getValue(),
            affectLeftCheckBox.isSelected(),
            affectMiddleCheckBox.isVisible() && affectMiddleCheckBox.isSelected(),
            affectRightCheckBox.isSelected()
        );
        canSubmit.set(submit);
        if (submit && !validationLabel.isVisible()) {
            validationLabel.setText("");
        }
    }

    private static void selectValue(ComboBox<String> combo, String preferredValue) {
        if (preferredValue != null && combo.getItems().contains(preferredValue)) {
            combo.setValue(preferredValue);
            return;
        }
        if (!combo.getItems().isEmpty()) {
            combo.setValue(combo.getItems().get(0));
        }
    }

    private static Throwable unwrap(Throwable throwable) {
        Throwable root = throwable;
        while (root instanceof CompletionException && root.getCause() != null) {
            root = root.getCause();
        }
        return root;
    }

    private static void affectManaged(CheckBox checkBox, boolean managed) {
        checkBox.setManaged(managed);
    }

    private static void requireInjected(Object field, String fieldName) {
        Objects.requireNonNull(field, () -> "Missing @FXML injection for " + fieldName);
    }
}
