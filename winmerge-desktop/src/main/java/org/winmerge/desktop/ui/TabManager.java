package org.winmerge.desktop.ui;

import java.util.Objects;
import java.util.concurrent.CountDownLatch;

import javafx.application.Platform;
import javafx.scene.Node;
import javafx.scene.control.Tab;
import javafx.scene.control.TabPane;

public class TabManager {
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
