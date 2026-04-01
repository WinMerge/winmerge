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

    public TabManager(TabPane tabPane) {
        this.tabPane = Objects.requireNonNull(tabPane, "tabPane");
    }

    public Tab openTab(String title, Node content) {
        Tab tab = new Tab(Objects.requireNonNull(title, "title"), Objects.requireNonNull(content, "content"));
        tab.setClosable(true);
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
            } else {
                MergeEditController mergeController = loader.getController();
                mergeController.setStatusListener(safeStatus);
                mergeController.loadFiles(request.leftPath(), request.rightPath());
            }

            String tabTitle = buildComparisonTabTitle(request.leftPath(), request.rightPath());
            Tab tab = openTab(tabTitle, contentRoot);
            safeStatus.accept("Opened " + (openHexView ? "hex" : "text") + " diff: " + tab.getText());
            return tab;
        } catch (IOException ioException) {
            throw new IllegalStateException("Unable to load comparison tab content", ioException);
        }
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
