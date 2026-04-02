package org.winmerge.desktop.ui.dialogs;

import java.lang.reflect.Constructor;
import java.util.List;
import java.util.Optional;

import javafx.geometry.Insets;
import javafx.scene.control.ButtonType;
import javafx.scene.control.CheckBox;
import javafx.scene.control.ComboBox;
import javafx.scene.control.Dialog;
import javafx.scene.control.Label;
import javafx.scene.layout.GridPane;
import javafx.scene.text.Font;
import javafx.scene.text.FontPosture;
import javafx.scene.text.FontWeight;
import javafx.stage.Window;
import org.winmerge.desktop.i18n.I18n;

public final class FontChooserDialog {
    private static final List<String> CONTROLSFX_DIALOG_CLASSES = List.of(
        "org.controlsfx.dialog.FontSelectorDialog",
        "org.controlsfx.control.FontSelectorDialog"
    );

    private final Window owner;
    private final Font initialFont;

    public FontChooserDialog(Window owner, Font initialFont) {
        this.owner = owner;
        this.initialFont = initialFont == null ? Font.getDefault() : initialFont;
    }

    public Optional<Font> showAndWait() {
        if (isControlsFxPresent()) {
            ControlsFxOutcome outcome = showControlsFxDialog();
            if (outcome.displayed()) {
                return outcome.result();
            }
        }
        return showFallbackDialog();
    }

    private boolean isControlsFxPresent() {
        ClassLoader classLoader = getClass().getClassLoader();
        for (String className : CONTROLSFX_DIALOG_CLASSES) {
            try {
                Class.forName(className, false, classLoader);
                return true;
            } catch (ClassNotFoundException ex) {
                // continue checking candidates
            }
        }
        return false;
    }

    private ControlsFxOutcome showControlsFxDialog() {
        for (String className : CONTROLSFX_DIALOG_CLASSES) {
            try {
                Class<?> dialogClass = Class.forName(className, true, getClass().getClassLoader());
                if (!Dialog.class.isAssignableFrom(dialogClass)) {
                    continue;
                }
                Constructor<?> constructor = dialogClass.getConstructor(Font.class);
                @SuppressWarnings("unchecked")
                Dialog<Font> dialog = (Dialog<Font>) constructor.newInstance(initialFont);
                if (owner != null) {
                    dialog.initOwner(owner);
                }
                dialog.setTitle(I18n.tr("IDS_FONT_DIALOG_TITLE"));
                return ControlsFxOutcome.displayed(dialog.showAndWait());
            } catch (ReflectiveOperationException | RuntimeException ex) {
                // Try fallback dialog if ControlsFX API differs.
                return ControlsFxOutcome.notDisplayed();
            }
        }
        return ControlsFxOutcome.notDisplayed();
    }

    private Optional<Font> showFallbackDialog() {
        Dialog<Font> dialog = new Dialog<>();
        if (owner != null) {
            dialog.initOwner(owner);
        }
        dialog.setTitle(I18n.tr("IDS_FONT_DIALOG_TITLE"));

        ComboBox<String> familyCombo = new ComboBox<>();
        familyCombo.getItems().setAll(Font.getFamilies());
        familyCombo.setValue(initialFont.getFamily());

        ComboBox<Integer> sizeCombo = new ComboBox<>();
        sizeCombo.getItems().setAll(8, 9, 10, 11, 12, 14, 16, 18, 20, 24, 28, 32, 36, 48, 60, 72);
        sizeCombo.setEditable(true);
        sizeCombo.setValue((int) Math.round(initialFont.getSize()));

        CheckBox boldCheckBox = new CheckBox(I18n.tr("IDS_FONT_DIALOG_BOLD"));
        CheckBox italicCheckBox = new CheckBox(I18n.tr("IDS_FONT_DIALOG_ITALIC"));

        String style = initialFont.getStyle();
        boldCheckBox.setSelected(style != null && style.toLowerCase().contains("bold"));
        italicCheckBox.setSelected(style != null && style.toLowerCase().contains("italic"));

        Label previewLabel = new Label(I18n.tr("IDS_FONT_DIALOG_SAMPLE"));

        Runnable refreshPreview = () -> {
            String family = familyCombo.getValue() == null ? Font.getDefault().getFamily() : familyCombo.getValue();
            int size = parseInt(sizeCombo.getEditor().getText(), (int) Math.round(initialFont.getSize()));
            FontWeight weight = boldCheckBox.isSelected() ? FontWeight.BOLD : FontWeight.NORMAL;
            FontPosture posture = italicCheckBox.isSelected() ? FontPosture.ITALIC : FontPosture.REGULAR;
            previewLabel.setFont(Font.font(family, weight, posture, Math.max(1, size)));
        };

        familyCombo.valueProperty().addListener((obs, oldValue, newValue) -> refreshPreview.run());
        sizeCombo.getEditor().textProperty().addListener((obs, oldValue, newValue) -> refreshPreview.run());
        boldCheckBox.selectedProperty().addListener((obs, oldValue, newValue) -> refreshPreview.run());
        italicCheckBox.selectedProperty().addListener((obs, oldValue, newValue) -> refreshPreview.run());
        refreshPreview.run();

        GridPane grid = new GridPane();
        grid.setHgap(10.0);
        grid.setVgap(10.0);
        grid.setPadding(new Insets(12.0));

        grid.add(new Label(I18n.tr("IDS_FONT_DIALOG_FAMILY")), 0, 0);
        grid.add(familyCombo, 1, 0);
        grid.add(new Label(I18n.tr("IDS_FONT_DIALOG_SIZE")), 0, 1);
        grid.add(sizeCombo, 1, 1);
        grid.add(boldCheckBox, 0, 2);
        grid.add(italicCheckBox, 1, 2);
        grid.add(new Label(I18n.tr("IDS_FONT_DIALOG_PREVIEW")), 0, 3);
        grid.add(previewLabel, 1, 3);

        dialog.getDialogPane().setContent(grid);
        dialog.getDialogPane().getButtonTypes().setAll(ButtonType.OK, ButtonType.CANCEL);

        dialog.setResultConverter(buttonType -> {
            if (buttonType != ButtonType.OK) {
                return null;
            }
            String family = familyCombo.getValue() == null ? Font.getDefault().getFamily() : familyCombo.getValue();
            int size = parseInt(sizeCombo.getEditor().getText(), (int) Math.round(initialFont.getSize()));
            FontWeight weight = boldCheckBox.isSelected() ? FontWeight.BOLD : FontWeight.NORMAL;
            FontPosture posture = italicCheckBox.isSelected() ? FontPosture.ITALIC : FontPosture.REGULAR;
            return Font.font(family, weight, posture, Math.max(1, size));
        });

        return dialog.showAndWait();
    }

    private static int parseInt(String value, int fallback) {
        try {
            return Integer.parseInt(value.trim());
        } catch (RuntimeException ex) {
            return fallback;
        }
    }

    private record ControlsFxOutcome(boolean displayed, Optional<Font> result) {
        private static ControlsFxOutcome displayed(Optional<Font> result) {
            return new ControlsFxOutcome(true, result == null ? Optional.empty() : result);
        }

        private static ControlsFxOutcome notDisplayed() {
            return new ControlsFxOutcome(false, Optional.empty());
        }
    }
}
