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
    private MenuItem editGoToMenuItem;

    @FXML
    private MenuItem toolsOptionsMenuItem;

    @FXML
    private MenuItem helpAboutMenuItem;

    @FXML
    private void initialize() {
        requireInjected(fileOpenMenuItem, "fileOpenMenuItem");
        requireInjected(fileExitMenuItem, "fileExitMenuItem");
        requireInjected(editGoToMenuItem, "editGoToMenuItem");
        requireInjected(toolsOptionsMenuItem, "toolsOptionsMenuItem");
        requireInjected(helpAboutMenuItem, "helpAboutMenuItem");
    }

    public void bind(ActionDispatcher dispatcher) {
        Objects.requireNonNull(dispatcher, "dispatcher");
        fileOpenMenuItem.setOnAction(event -> dispatcher.dispatch(ActionId.FILE_OPEN));
        fileExitMenuItem.setOnAction(event -> dispatcher.dispatch(ActionId.FILE_EXIT));
        editGoToMenuItem.setOnAction(event -> dispatcher.dispatch(ActionId.EDIT_GO_TO));
        toolsOptionsMenuItem.setOnAction(event -> dispatcher.dispatch(ActionId.TOOLS_OPTIONS));
        helpAboutMenuItem.setOnAction(event -> dispatcher.dispatch(ActionId.HELP_ABOUT));
    }

    private static void requireInjected(Object field, String fieldName) {
        Objects.requireNonNull(field, () -> "Missing @FXML injection for " + fieldName);
    }
}
