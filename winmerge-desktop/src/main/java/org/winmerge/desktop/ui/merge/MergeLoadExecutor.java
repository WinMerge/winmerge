package org.winmerge.desktop.ui.merge;

import java.util.Objects;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

final class MergeLoadExecutor implements AutoCloseable {
    private final ExecutorService executor;
    private final Object lock = new Object();
    private Future<?> activeFuture;

    MergeLoadExecutor(String threadName) {
        this(
            Executors.newSingleThreadExecutor(runnable -> {
                Thread thread = new Thread(runnable, threadName);
                thread.setDaemon(true);
                return thread;
            })
        );
    }

    MergeLoadExecutor(ExecutorService executor) {
        this.executor = Objects.requireNonNull(executor, "executor");
    }

    void submit(Runnable runnable) {
        Objects.requireNonNull(runnable, "runnable");
        synchronized (lock) {
            cancelActiveLocked();
            activeFuture = executor.submit(runnable);
        }
    }

    void cancelActive() {
        synchronized (lock) {
            cancelActiveLocked();
        }
    }

    boolean isShutdown() {
        return executor.isShutdown();
    }

    @Override
    public void close() {
        synchronized (lock) {
            cancelActiveLocked();
        }
        executor.shutdownNow();
    }

    private void cancelActiveLocked() {
        if (activeFuture == null) {
            return;
        }
        activeFuture.cancel(true);
        activeFuture = null;
    }
}
