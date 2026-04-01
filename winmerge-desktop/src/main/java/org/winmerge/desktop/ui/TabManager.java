package org.winmerge.desktop.ui;

import java.io.IOException;
import java.io.InputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Objects;
import java.util.Optional;
import java.util.concurrent.CountDownLatch;
import java.util.function.Consumer;

import javafx.application.Platform;
import javafx.fxml.FXMLLoader;
import javafx.scene.Node;
import javafx.scene.Parent;
import javafx.scene.control.Tab;
import javafx.scene.control.TabPane;
import org.winmerge.desktop.ui.dialogs.DialogService;
import org.winmerge.desktop.ui.dialogs.WMGotoDialogRequest;
import org.winmerge.desktop.ui.dialogs.WMGotoDialogResult;
import org.winmerge.desktop.ui.dialogs.WMGotoTarget;
import org.winmerge.desktop.ui.dir.DirController;
import org.winmerge.desktop.ui.hex.HexController;
import org.winmerge.desktop.ui.merge.MergeEditController;

public class TabManager {
    public enum CompareTarget {
        FILES,
        FOLDERS
    }

    public record ComparisonRequest(
        Path leftPath,
        Path rightPath,
        Optional<Path> middlePath,
        CompareTarget target,
        String filter
    ) {
        public ComparisonRequest {
            Objects.requireNonNull(leftPath, "leftPath");
            Objects.requireNonNull(rightPath, "rightPath");
            middlePath = middlePath == null ? Optional.empty() : middlePath;
            Objects.requireNonNull(target, "target");
            filter = filter == null ? "*.*" : filter;
        }
    }

    private final TabPane tabPane;
    private final DialogService dialogService;

    public TabManager(TabPane tabPane, DialogService dialogService) {
        this.tabPane = Objects.requireNonNull(tabPane, "tabPane");
        this.dialogService = Objects.requireNonNull(dialogService, "dialogService");
    }

    public Tab openTab(String title, Node content) {
        Tab tab = new Tab(Objects.requireNonNull(title, "title"), Objects.requireNonNull(content, "content"));
        tab.setClosable(true);
        tab.setOnClosed(event -> disposeController(tab.getUserData()));
        runOnFxThreadAndWait(() -> {
            tabPane.getTabs().add(tab);
            tabPane.getSelectionModel().select(tab);
        });
        return tab;
    }

    public boolean closeTab(Tab tab) {
        if (tab == null) {
            return false;
        }
        final boolean[] removed = new boolean[1];
        runOnFxThreadAndWait(() -> removed[0] = tabPane.getTabs().remove(tab));
        if (removed[0]) {
            disposeController(tab.getUserData());
        }
        return removed[0];
    }

    public Tab openComparison(ComparisonRequest request, Consumer<String> statusListener) {
        Objects.requireNonNull(request, "request");
        Consumer<String> safeStatus = statusListener == null ? message -> { } : statusListener;

        if (request.target() == CompareTarget.FOLDERS) {
            if (request.middlePath().isPresent()) {
                safeStatus.accept("3-way folder compare is pending; opening a left/right comparison.");
            }
            try {
                FXMLLoader loader = new FXMLLoader(TabManager.class.getResource("/org/winmerge/desktop/ui/dir/DirPane.fxml"));
                Parent contentRoot = loader.load();

                DirController dirController = loader.getController();
                dirController.configure(this, safeStatus);
                dirController.loadDirectories(request.leftPath(), request.rightPath(), request.filter());

                String tabTitle = buildComparisonTabTitle(request.leftPath(), request.rightPath());
                Tab tab = openTab(tabTitle, contentRoot);
                tab.setUserData(dirController);
                safeStatus.accept("Opened directory diff: " + tab.getText());
                return tab;
            } catch (IOException ioException) {
                throw new IllegalStateException("Unable to load directory comparison tab content", ioException);
            }
        }
        if (request.middlePath().isPresent()) {
            safeStatus.accept("3-way mode is pending; opening a left/right comparison.");
        }

        boolean openHexView = isLikelyBinary(request.leftPath()) || isLikelyBinary(request.rightPath());
        String fxmlResource = openHexView
            ? "/org/winmerge/desktop/ui/hex/HexPane.fxml"
            : "/org/winmerge/desktop/ui/merge/MergeEditPane.fxml";

        try {
            FXMLLoader loader = new FXMLLoader(TabManager.class.getResource(fxmlResource));
            Parent contentRoot = loader.load();

            if (openHexView) {
                HexController hexController = loader.getController();
                hexController.setStatusListener(safeStatus);
                hexController.loadFiles(request.leftPath(), request.rightPath());
                Tab tab = openTab(buildComparisonTabTitle(request.leftPath(), request.rightPath()), contentRoot);
                tab.setUserData(hexController);
                safeStatus.accept("Opened hex diff: " + tab.getText());
                return tab;
            } else {
                MergeEditController mergeController = loader.getController();
                mergeController.setStatusListener(safeStatus);
                mergeController.loadFiles(request.leftPath(), request.rightPath());
                Tab tab = openTab(buildComparisonTabTitle(request.leftPath(), request.rightPath()), contentRoot);
                tab.setUserData(mergeController);
                installDirtyCloseGuard(tab, mergeController, safeStatus);
                safeStatus.accept("Opened text diff: " + tab.getText());
                return tab;
            }
        } catch (IOException ioException) {
            throw new IllegalStateException("Unable to load comparison tab content", ioException);
        }
    }

    public Optional<WMGotoDialogRequest> createGotoRequestForActiveTab() {
        Tab selectedTab = tabPane.getSelectionModel().getSelectedItem();
        if (selectedTab == null || !(selectedTab.getUserData() instanceof MergeEditController mergeController)) {
            return Optional.empty();
        }
        if (!mergeController.isNavigationReady()) {
            return Optional.empty();
        }

        int leftLineCount = Math.max(1, mergeController.lineCountForFile(0));
        int rightLineCount = Math.max(1, mergeController.lineCountForFile(1));
        int[] maxLinePerFile = new int[] {leftLineCount, rightLineCount, rightLineCount};

        return Optional.of(
            new WMGotoDialogRequest(
                "1",
                0,
                WMGotoTarget.LINE,
                mergeController.fileCount(),
                maxLinePerFile,
                mergeController.diffCount()
            )
        );
    }

    public boolean applyGotoSelection(WMGotoDialogResult selection, Consumer<String> statusListener) {
        Objects.requireNonNull(selection, "selection");
        Consumer<String> safeStatus = statusListener == null ? message -> { } : statusListener;

        Tab selectedTab = tabPane.getSelectionModel().getSelectedItem();
        if (selectedTab == null || !(selectedTab.getUserData() instanceof MergeEditController mergeController)) {
            return false;
        }
        if (!mergeController.isNavigationReady()) {
            safeStatus.accept("Go to is not available until the comparison is loaded.");
            return false;
        }

        if (selection.target() == WMGotoTarget.LINE) {
            int targetFile = selection.fileIndex() <= 0 ? 0 : 1;
            mergeController.navigateToLine(targetFile, selection.value());
        } else {
            mergeController.navigateToDiff(selection.value());
        }
        return true;
    }

    private static String buildComparisonTabTitle(Path leftPath, Path rightPath) {
        String leftName = leftPath.getFileName() == null ? leftPath.toString() : leftPath.getFileName().toString();
        String rightName = rightPath.getFileName() == null ? rightPath.toString() : rightPath.getFileName().toString();
        return leftName + " ↔ " + rightName;
    }

    private static boolean isLikelyBinary(Path path) {
        byte[] probe = new byte[2048];
        int bytesRead;
        try (InputStream stream = Files.newInputStream(path)) {
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

    static void disposeController(Object controller) {
        if (!(controller instanceof AutoCloseable closeable)) {
            return;
        }
        try {
            closeable.close();
        } catch (Exception ignored) {
            // Best-effort cleanup on tab close.
        }
    }

    private void installDirtyCloseGuard(Tab tab, DirtyTab dirtyTab, Consumer<String> statusListener) {
        tab.setOnCloseRequest(event -> {
            if (!SaveClosingHandler.confirmClose(dirtyTab, dialogService, statusListener)) {
                event.consume();
            }
        });
    }

    private void runOnFxThreadAndWait(Runnable action) {
        if (Platform.isFxApplicationThread()) {
            action.run();
            return;
        }

        CountDownLatch latch = new CountDownLatch(1);
        Platform.runLater(() -> {
            try {
                action.run();
            } finally {
                latch.countDown();
            }
        });

        try {
            latch.await();
        } catch (InterruptedException interruptedException) {
            Thread.currentThread().interrupt();
            throw new IllegalStateException("Interrupted while waiting for JavaFX thread", interruptedException);
        }
    }
}
