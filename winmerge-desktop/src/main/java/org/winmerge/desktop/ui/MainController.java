package org.winmerge.desktop.ui;

import java.util.Objects;

import javafx.application.Platform;
import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.Tab;
import javafx.scene.control.TabPane;
import javafx.scene.control.ToolBar;
import javafx.scene.layout.StackPane;
import javafx.stage.Stage;
import javafx.stage.WindowEvent;

public class MainController {
    @FXML
    private ToolBar mainToolBar;

    @FXML
    private Button toolbarOpenButton;

    @FXML
    private Button toolbarExitButton;

    @FXML
    private TabPane mainTabPane;

    @FXML
    private MenuBarController menuBarViewController;

    @FXML
    private StatusBarController statusBarViewController;

    private final ActionDispatcher actionDispatcher = new ActionDispatcher();
    private TabManager tabManager;
    private Stage primaryStage;
    private int openCounter = 1;

    @FXML
    private void initialize() {
        requireInjected(mainToolBar, "mainToolBar");
        requireInjected(toolbarOpenButton, "toolbarOpenButton");
        requireInjected(toolbarExitButton, "toolbarExitButton");
        requireInjected(mainTabPane, "mainTabPane");
        requireInjected(menuBarViewController, "menuBarViewController");
        requireInjected(statusBarViewController, "statusBarViewController");

        tabManager = new TabManager(mainTabPane);
        registerActions();
        menuBarViewController.bind(actionDispatcher);
        toolbarOpenButton.setOnAction(event -> actionDispatcher.dispatch(ActionId.FILE_OPEN));
        toolbarExitButton.setOnAction(event -> actionDispatcher.dispatch(ActionId.FILE_EXIT));
        statusBarViewController.setStatusText("Ready");
    }

    public void setPrimaryStage(Stage primaryStage) {
        this.primaryStage = Objects.requireNonNull(primaryStage, "primaryStage");
    }

    public void handleWindowCloseRequest(WindowEvent event) {
        statusBarViewController.setStatusText("Exiting WinMerge...");
    }

    private void registerActions() {
        actionDispatcher.register(ActionId.FILE_OPEN, this::openPlaceholderTab);
        actionDispatcher.register(ActionId.FILE_EXIT, this::requestExit);
    }

    private void openPlaceholderTab() {
        String tabTitle = "Open " + openCounter++;
        Label placeholderText = new Label("Open dialog migration lands in phase 2c-5.");
        placeholderText.getStyleClass().add("placeholder-text");
        StackPane content = new StackPane(placeholderText);
        content.getStyleClass().add("placeholder-tab-content");

        Tab tab = tabManager.openTab(tabTitle, content);
        statusBarViewController.setStatusText("Opened tab: " + tab.getText());
    }

    private void requestExit() {
        if (primaryStage == null) {
            Platform.exit();
            return;
        }
        primaryStage.fireEvent(
            new WindowEvent(primaryStage, WindowEvent.WINDOW_CLOSE_REQUEST)
        );
        Platform.exit();
    }

    private static void requireInjected(Object field, String fieldName) {
        Objects.requireNonNull(field, () -> "Missing @FXML injection for " + fieldName);
    }
}
