package org.winmerge.desktop.ui;

import java.util.concurrent.Callable;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.FutureTask;
import java.util.concurrent.TimeUnit;

import javafx.application.Platform;

final class FxThreadTestSupport {
    private static final long FX_TIMEOUT_SECONDS = 10;
    private static volatile boolean toolkitInitialized = false;

    private FxThreadTestSupport() {
    }

    static synchronized void initializeToolkit() {
        if (toolkitInitialized) {
            return;
        }
        CountDownLatch startupLatch = new CountDownLatch(1);
        Platform.startup(startupLatch::countDown);
        await(startupLatch, "Failed to initialize JavaFX toolkit");
        toolkitInitialized = true;
    }

    static void runOnFxThread(Runnable runnable) {
        callOnFxThread(() -> {
            runnable.run();
            return null;
        });
    }

    static <T> T callOnFxThread(Callable<T> callable) {
        if (Platform.isFxApplicationThread()) {
            try {
                return callable.call();
            } catch (Exception exception) {
                throw new RuntimeException(exception);
            }
        }

        FutureTask<T> futureTask = new FutureTask<>(callable);
        Platform.runLater(futureTask);
        try {
            return futureTask.get(FX_TIMEOUT_SECONDS, TimeUnit.SECONDS);
        } catch (ExecutionException executionException) {
            Throwable cause = executionException.getCause();
            if (cause instanceof RuntimeException runtimeException) {
                throw runtimeException;
            }
            throw new RuntimeException(cause);
        } catch (Exception exception) {
            throw new RuntimeException(exception);
        }
    }

    private static void await(CountDownLatch latch, String message) {
        try {
            if (!latch.await(FX_TIMEOUT_SECONDS, TimeUnit.SECONDS)) {
                throw new IllegalStateException(message);
            }
        } catch (InterruptedException interruptedException) {
            Thread.currentThread().interrupt();
            throw new IllegalStateException(message, interruptedException);
        }
    }
}
