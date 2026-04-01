package org.winmerge.desktop.ui;

import java.io.IOException;
import java.nio.file.Path;
import java.util.Optional;
import java.util.Objects;

import javafx.application.Platform;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.control.Button;
import javafx.scene.control.ButtonType;
import javafx.scene.control.Tab;
import javafx.scene.control.TabPane;
import javafx.scene.control.ToolBar;
import javafx.stage.Stage;
import javafx.stage.WindowEvent;
import org.winmerge.core.WinMergeCore;
import org.winmerge.desktop.i18n.I18n;
import org.winmerge.desktop.ui.dialogs.AboutDialogModel;
import org.winmerge.desktop.ui.dialogs.DefaultDialogService;
import org.winmerge.desktop.ui.dialogs.DialogService;
import org.winmerge.desktop.ui.dialogs.WMGotoDialogRequest;
import org.winmerge.desktop.ui.dialogs.WMGotoDialogResult;
import org.winmerge.desktop.ui.open.OpenController;
import org.winmerge.desktop.ui.options.AppSettings;
import org.winmerge.desktop.ui.options.AppSettingsStore;
import org.winmerge.desktop.ui.options.OptionsDialog;
import org.winmerge.desktop.ui.options.OptionsDialogSession;

public class MainController {
    private static final String GNU_ASCII = """
          ,           ,
         /             \\
        ((__-^^-,-^^-__))
         `-_---' `---_-'
          `--|o` 'o|--'
             \\  `  /
              ): :(
              :o_o:
               "-"
        """;

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

    private Stage primaryStage;
    private final ActionDispatcher actionDispatcher = new ActionDispatcher();
    private final DialogService dialogService = new DefaultDialogService(() -> primaryStage);
    private final AppSettingsStore appSettingsStore = new AppSettingsStore(WinMergeCore.defaultConfigurationStore());
    private final AppSettings appSettings = appSettingsStore.load();
    private TabManager tabManager;

    @FXML
    private void initialize() {
        requireInjected(mainToolBar, "mainToolBar");
        requireInjected(toolbarOpenButton, "toolbarOpenButton");
        requireInjected(toolbarExitButton, "toolbarExitButton");
        requireInjected(mainTabPane, "mainTabPane");
        requireInjected(menuBarViewController, "menuBarViewController");
        requireInjected(statusBarViewController, "statusBarViewController");

        tabManager = new TabManager(mainTabPane, dialogService);
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
        actionDispatcher.register(ActionId.EDIT_GO_TO, this::openGotoDialog);
        actionDispatcher.register(ActionId.TOOLS_OPTIONS, this::openOptionsDialog);
        actionDispatcher.register(ActionId.HELP_ABOUT, this::openAboutDialog);
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

    private void openOptionsDialog() {
        OptionsDialogSession session = new OptionsDialogSession(appSettings, appSettingsStore);

        OptionsDialog optionsDialog = new OptionsDialog(primaryStage, session.draft(), () -> {
            session.applyPreview();
            statusBarViewController.setStatusText("Options applied (preview).");
        });

        if (session.finish(optionsDialog.showAndWait())) {
            statusBarViewController.setStatusText("Options saved.");
        } else {
            statusBarViewController.setStatusText("Options changes discarded.");
        }
    }

    private void openGotoDialog() {
        if (tabManager == null) {
            statusBarViewController.setStatusText("Go to is not available yet.");
            return;
        }

        Optional<WMGotoDialogRequest> request = tabManager.createGotoRequestForActiveTab();
        if (request.isEmpty()) {
            statusBarViewController.setStatusText("Go to is available for text comparison tabs.");
            return;
        }

        Optional<WMGotoDialogResult> result = dialogService.showGotoDialog(request.get());
        if (result.isEmpty()) {
            statusBarViewController.setStatusText("Go to cancelled.");
            return;
        }

        boolean applied = tabManager.applyGotoSelection(result.get(), statusBarViewController::setStatusText);
        if (!applied) {
            statusBarViewController.setStatusText("Go to failed: active tab does not support navigation.");
        }
    }

    private void openAboutDialog() {
        AboutDialogModel model = new AboutDialogModel(
            I18n.tr("AFX_IDS_APP_TITLE"),
            I18n.tr("IDS_VERSION_FMT", WinMergeCore.version()),
            "https://winmerge.org",
            "https://winmerge.org",
            I18n.tr("IDS_SPLASH_GPLTEXT"),
            GNU_ASCII,
            Path.of("Docs", "Users", "Contributors.txt").toAbsolutePath().normalize()
        );
        dialogService.showAboutDialog(model);
        statusBarViewController.setStatusText("About dialog closed.");
    }

    private static void requireInjected(Object field, String fieldName) {
        Objects.requireNonNull(field, () -> "Missing @FXML injection for " + fieldName);
    }
}
