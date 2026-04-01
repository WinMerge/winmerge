package org.winmerge.desktop.ui;

import java.io.IOException;
import java.util.Objects;

import javafx.application.Platform;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.control.Button;
import javafx.scene.control.Tab;
import javafx.scene.control.TabPane;
import javafx.scene.control.ToolBar;
import javafx.stage.Stage;
import javafx.stage.WindowEvent;
import org.winmerge.desktop.ui.open.OpenController;

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
        actionDispatcher.register(ActionId.FILE_OPEN, this::openOpenPaneTab);
        actionDispatcher.register(ActionId.FILE_EXIT, this::requestExit);
    }

    private void openOpenPaneTab() {
        try {
            FXMLLoader loader = new FXMLLoader(MainController.class.getResource("/org/winmerge/desktop/ui/open/OpenPane.fxml"));
            Parent contentRoot = loader.load();
            OpenController openController = loader.getController();

            Tab openTab = tabManager.openTab("Open", contentRoot);
            openController.configure(
                tabManager,
                () -> tabManager.closeTab(openTab),
                statusBarViewController::setStatusText
            );
            statusBarViewController.setStatusText("Open pane ready.");
        } catch (IOException ioException) {
            statusBarViewController.setStatusText("Failed to open Open pane: " + ioException.getMessage());
        }
    }

    private void requestExit() {
        if (primaryStage == null) {
            Platform.exit();
            return;
        }
        primaryStage.close();
    }

    private static void requireInjected(Object field, String fieldName) {
        Objects.requireNonNull(field, () -> "Missing @FXML injection for " + fieldName);
    }
}
