package org.winmerge.desktop.ui.hex;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.io.IOException;
import java.lang.reflect.Field;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardOpenOption;
import java.util.concurrent.Callable;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.FutureTask;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicReference;

import javafx.application.Platform;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.control.ScrollBar;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Test;

class HexControllerFxIntegrationTest {
    private static final long FX_TIMEOUT_SECONDS = 10;
    private static volatile boolean toolkitInitialized;

    @BeforeAll
    static void setupToolkit() throws InterruptedException {
        initToolkit();
    }

    @Test
    void loadFilesPublishesDiffStatusForBinaryPair() throws Exception {
        Path left = Files.createTempFile("hex-controller-left", ".bin");
        Path right = Files.createTempFile("hex-controller-right", ".bin");
        try {
            Files.write(left, new byte[] { 0x01, 0x02, 0x03, 0x04 });
            Files.write(right, new byte[] { 0x01, 0x02, 0x05, 0x04 });

            CountDownLatch loaded = new CountDownLatch(1);
            AtomicReference<String> status = new AtomicReference<>();
            AtomicReference<HexController> controllerRef = new AtomicReference<>();

            runOnFxThread(() -> {
                HexController controller = loadController();
                controllerRef.set(controller);
                controller.setStatusListener(message -> {
                    status.set(message);
                    if (message.startsWith("Opened binary hex view.")) {
                        loaded.countDown();
                    }
                });
                controller.loadFiles(left, right);
            });

            assertTrue(loaded.await(5, TimeUnit.SECONDS), "Hex files did not finish loading");
            assertNotNull(status.get());
            assertTrue(status.get().contains("Differences highlighted."));

            runOnFxThread(() -> controllerRef.get().close());
        } finally {
            Files.deleteIfExists(left);
            Files.deleteIfExists(right);
        }
    }

    @Test
    void loadFilesReportsSizeGuardFailureWhenLimitExceeded() throws Exception {
        Path left = Files.createTempFile("hex-controller-left", ".bin");
        Path right = Files.createTempFile("hex-controller-right", ".bin");
        try {
            Files.write(left, new byte[] { 0x01, 0x02, 0x03 });
            createSparseFileBeyondLimit(right, HexDocModel.MAX_LOAD_BYTES_PER_FILE + 1);

            CountDownLatch failed = new CountDownLatch(1);
            AtomicReference<String> status = new AtomicReference<>();
            AtomicReference<HexController> controllerRef = new AtomicReference<>();

            runOnFxThread(() -> {
                HexController controller = loadController();
                controllerRef.set(controller);
                controller.setStatusListener(message -> {
                    status.set(message);
                    if (message.startsWith("Hex view size guard:")) {
                        failed.countDown();
                    }
                });
                controller.loadFiles(left, right);
            });

            assertTrue(failed.await(5, TimeUnit.SECONDS), "Size-guard status was not emitted");
            assertNotNull(status.get());
            assertTrue(status.get().contains("limit"));

            runOnFxThread(() -> controllerRef.get().close());
        } finally {
            Files.deleteIfExists(left);
            Files.deleteIfExists(right);
        }
    }

    @Test
    void leftAndRightScrollBarsStaySynchronized() throws Exception {
        AtomicReference<HexController> controllerRef = new AtomicReference<>();

        try {
            runOnFxThread(() -> {
                HexController controller = loadController();
                controllerRef.set(controller);

                ScrollBar leftScrollBar = readField(controller, "leftScrollBar");
                ScrollBar rightScrollBar = readField(controller, "rightScrollBar");

                leftScrollBar.setMax(100.0);
                rightScrollBar.setMax(100.0);

                leftScrollBar.setValue(27.0);
                assertEquals(27.0, rightScrollBar.getValue(), 0.0001);

                rightScrollBar.setValue(12.0);
                assertEquals(12.0, leftScrollBar.getValue(), 0.0001);
            });
        } finally {
            if (controllerRef.get() != null) {
                runOnFxThread(() -> controllerRef.get().close());
            }
        }
    }

    private static HexController loadController() {
        try {
            FXMLLoader loader = new FXMLLoader(
                HexControllerFxIntegrationTest.class.getResource("/org/winmerge/desktop/ui/hex/HexPane.fxml")
            );
            Parent unused = loader.load();
            return loader.getController();
        } catch (IOException exception) {
            throw new RuntimeException(exception);
        }
    }

    private static void createSparseFileBeyondLimit(Path path, long sizeBytes) throws IOException {
        try (
            FileChannel channel = FileChannel.open(
                path,
                StandardOpenOption.CREATE,
                StandardOpenOption.TRUNCATE_EXISTING,
                StandardOpenOption.WRITE
            )
        ) {
            channel.position(sizeBytes - 1);
            channel.write(ByteBuffer.wrap(new byte[] { 0x00 }));
        }
    }

    private static synchronized void initToolkit() throws InterruptedException {
        if (toolkitInitialized) {
            return;
        }
        CountDownLatch startupLatch = new CountDownLatch(1);
        try {
            Platform.startup(startupLatch::countDown);
            if (!startupLatch.await(FX_TIMEOUT_SECONDS, TimeUnit.SECONDS)) {
                throw new IllegalStateException("Failed to initialize JavaFX toolkit");
            }
        } catch (IllegalStateException exception) {
            String message = exception.getMessage();
            if (message == null || !message.contains("already initialized")) {
                throw exception;
            }
        }
        toolkitInitialized = true;
    }

    private static void runOnFxThread(Runnable runnable) {
        callOnFxThread(() -> {
            runnable.run();
            return null;
        });
    }

    private static <T> T callOnFxThread(Callable<T> callable) {
        if (Platform.isFxApplicationThread()) {
            try {
                return callable.call();
            } catch (Exception exception) {
                throw new RuntimeException(exception);
            }
        }

        FutureTask<T> task = new FutureTask<>(callable);
        Platform.runLater(task);
        try {
            return task.get(FX_TIMEOUT_SECONDS, TimeUnit.SECONDS);
        } catch (Exception exception) {
            throw new RuntimeException(exception);
        }
    }

    @SuppressWarnings("unchecked")
    private static <T> T readField(Object target, String name) {
        try {
            Field field = target.getClass().getDeclaredField(name);
            field.setAccessible(true);
            return (T) field.get(target);
        } catch (ReflectiveOperationException exception) {
            throw new RuntimeException(exception);
        }
    }
}
