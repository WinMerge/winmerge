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
    private MenuItem toolsOptionsMenuItem;

    @FXML
    private void initialize() {
        requireInjected(fileOpenMenuItem, "fileOpenMenuItem");
        requireInjected(fileExitMenuItem, "fileExitMenuItem");
        requireInjected(toolsOptionsMenuItem, "toolsOptionsMenuItem");
    }

    public void bind(ActionDispatcher dispatcher) {
        Objects.requireNonNull(dispatcher, "dispatcher");
        fileOpenMenuItem.setOnAction(event -> dispatcher.dispatch(ActionId.FILE_OPEN));
        fileExitMenuItem.setOnAction(event -> dispatcher.dispatch(ActionId.FILE_EXIT));
        toolsOptionsMenuItem.setOnAction(event -> dispatcher.dispatch(ActionId.TOOLS_OPTIONS));
    }

    private static void requireInjected(Object field, String fieldName) {
        Objects.requireNonNull(field, () -> "Missing @FXML injection for " + fieldName);
    }
}
