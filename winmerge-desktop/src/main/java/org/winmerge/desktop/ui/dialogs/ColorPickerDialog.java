package org.winmerge.desktop.ui.dialogs;

import javafx.geometry.Insets;
import javafx.scene.control.ButtonType;
import javafx.scene.control.ColorPicker;
import javafx.scene.control.Dialog;
import javafx.scene.control.Label;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;
import javafx.stage.Window;
import org.winmerge.desktop.i18n.I18n;

public final class ColorPickerDialog extends Dialog<Color> {
    public ColorPickerDialog(Window owner, Color initialColor) {
        if (owner != null) {
            initOwner(owner);
        }

        ColorPicker colorPicker = new ColorPicker(initialColor == null ? Color.WHITE : initialColor);
        Label prompt = new Label(I18n.tr("IDS_COLOR_DIALOG_LABEL"));
        VBox content = new VBox(10.0, prompt, colorPicker);
        content.setPadding(new Insets(12.0));

        getDialogPane().setContent(content);
        getDialogPane().getButtonTypes().setAll(ButtonType.OK, ButtonType.CANCEL);
        setTitle(I18n.tr("IDS_COLOR_DIALOG_TITLE"));

        setResultConverter(buttonType -> {
            if (buttonType != ButtonType.OK) {
                return null;
            }
            Color value = colorPicker.getValue();
            return value == null ? null : value;
        });
    }
}
