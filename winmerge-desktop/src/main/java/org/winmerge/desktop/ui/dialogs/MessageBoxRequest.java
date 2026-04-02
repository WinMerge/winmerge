package org.winmerge.desktop.ui.dialogs;

import java.util.List;

import javafx.scene.control.Alert;
import javafx.scene.control.ButtonType;

public record MessageBoxRequest(
    Alert.AlertType alertType,
    String title,
    String headerText,
    String contentText,
    List<ButtonType> buttons
) {
    public MessageBoxRequest {
        alertType = alertType == null ? Alert.AlertType.INFORMATION : alertType;
        title = title == null ? "" : title;
        headerText = headerText == null ? "" : headerText;
        contentText = contentText == null ? "" : contentText;
        buttons = buttons == null ? List.of() : List.copyOf(buttons);
    }

    public static MessageBoxRequest information(String title, String contentText) {
        return new MessageBoxRequest(Alert.AlertType.INFORMATION, title, "", contentText, List.of(ButtonType.OK));
    }
}
