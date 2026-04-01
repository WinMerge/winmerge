package org.winmerge.desktop.ui;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.nio.file.Path;
import java.util.Objects;
import java.util.Optional;

import javafx.application.Platform;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.control.Button;
import javafx.scene.control.Tab;
import javafx.scene.control.TabPane;
import javafx.scene.control.ToolBar;
import javafx.stage.FileChooser;
import javafx.stage.Stage;
import javafx.stage.WindowEvent;
import org.winmerge.desktop.ui.hex.HexController;
import org.winmerge.desktop.ui.merge.MergeEditController;

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
        actionDispatcher.register(ActionId.FILE_OPEN, this::openFileDiffTab);
        actionDispatcher.register(ActionId.FILE_EXIT, this::requestExit);
    }

    private void openFileDiffTab() {
        if (primaryStage == null) {
            statusBarViewController.setStatusText("Window is not initialized yet.");
            return;
        }

        Optional<FilePair> selectedPair = promptForTwoFiles(primaryStage);
        if (selectedPair.isEmpty()) {
            statusBarViewController.setStatusText("Open cancelled.");
            return;
        }

        FilePair pair = selectedPair.get();
        try {
            boolean openHexView = isLikelyBinary(pair.leftPath()) || isLikelyBinary(pair.rightPath());
            String fxmlResource = openHexView
                ? "/org/winmerge/desktop/ui/hex/HexPane.fxml"
                : "/org/winmerge/desktop/ui/merge/MergeEditPane.fxml";

            FXMLLoader loader = new FXMLLoader(MainController.class.getResource(fxmlResource));
            Parent contentRoot = loader.load();

            if (openHexView) {
                HexController hexController = loader.getController();
                hexController.setStatusListener(statusBarViewController::setStatusText);
                hexController.loadFiles(pair.leftPath(), pair.rightPath());
            } else {
                MergeEditController mergeController = loader.getController();
                mergeController.setStatusListener(statusBarViewController::setStatusText);
                mergeController.loadFiles(pair.leftPath(), pair.rightPath());
            }

            String tabTitle = pair.leftPath().getFileName() + " ↔ " + pair.rightPath().getFileName();
            Tab tab = tabManager.openTab(tabTitle, contentRoot);
            String mode = openHexView ? "hex" : "text";
            statusBarViewController.setStatusText("Opened " + mode + " diff: " + tab.getText());
        } catch (IOException ioException) {
            statusBarViewController.setStatusText("Failed to open file diff: " + ioException.getMessage());
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

    private static Optional<FilePair> promptForTwoFiles(Stage owner) {
        FileChooser chooser = new FileChooser();
        chooser.setTitle("Select left file for comparison");
        File leftFile = chooser.showOpenDialog(owner);
        if (leftFile == null) {
            return Optional.empty();
        }

        chooser.setTitle("Select right file for comparison");
        File rightFile = chooser.showOpenDialog(owner);
        if (rightFile == null) {
            return Optional.empty();
        }
        return Optional.of(new FilePair(leftFile.toPath(), rightFile.toPath()));
    }

    private static boolean isLikelyBinary(Path path) {
        byte[] probe = new byte[2048];
        int bytesRead;
        try (InputStream stream = java.nio.file.Files.newInputStream(path)) {
            bytesRead = stream.read(probe);
        } catch (IOException ignored) {
            return false;
        }
        if (bytesRead <= 0) {
            return false;
        }

        int nonTextCount = 0;
        for (int i = 0; i < bytesRead; i++) {
            int value = probe[i] & 0xFF;
            if (value == 0) {
                return true;
            }
            if (value < 9 || (value > 13 && value < 32)) {
                nonTextCount++;
            }
        }
        return nonTextCount > (bytesRead / 5);
    }

    private record FilePair(Path leftPath, Path rightPath) {
    }
}
