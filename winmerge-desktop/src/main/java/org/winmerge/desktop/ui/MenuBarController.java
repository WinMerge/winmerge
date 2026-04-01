package org.winmerge.desktop.ui;

import java.util.Objects;

import javafx.fxml.FXML;
import javafx.scene.control.MenuItem;

public class MenuBarController {
    @FXML
    private MenuItem fileOpenMenuItem;

    @FXML
    private MenuItem fileExitMenuItem;

    @FXML
    private void initialize() {
        requireInjected(fileOpenMenuItem, "fileOpenMenuItem");
        requireInjected(fileExitMenuItem, "fileExitMenuItem");
    }

    public void bind(ActionDispatcher dispatcher) {
        Objects.requireNonNull(dispatcher, "dispatcher");
        fileOpenMenuItem.setOnAction(event -> dispatcher.dispatch(ActionId.FILE_OPEN));
        fileExitMenuItem.setOnAction(event -> dispatcher.dispatch(ActionId.FILE_EXIT));
    }

    private static void requireInjected(Object field, String fieldName) {
        Objects.requireNonNull(field, () -> "Missing @FXML injection for " + fieldName);
    }
}
