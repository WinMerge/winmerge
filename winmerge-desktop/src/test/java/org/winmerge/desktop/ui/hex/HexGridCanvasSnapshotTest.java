package org.winmerge.desktop.ui.hex;

import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.junit.jupiter.api.Assumptions.assumeTrue;

import java.io.IOException;
import java.io.OutputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicReference;

import javafx.application.Platform;
import javafx.scene.SnapshotParameters;
import javafx.scene.image.PixelReader;
import javafx.scene.image.WritableImage;
import org.junit.jupiter.api.Test;

class HexGridCanvasSnapshotTest {
    private static final int CANVAS_WIDTH = 560;
    private static final int CANVAS_HEIGHT = 220;
    private static volatile boolean toolkitReady;

    @Test
    void writesSnapshotToPpm() throws Exception {
        initToolkit();
        String output = System.getProperty("hex.snapshot.path");
        if (output == null || output.isBlank()) {
            output = System.getenv("HEX_SNAPSHOT_PATH");
        }
        assumeTrue(output != null && !output.isBlank(), "hex.snapshot.path system property is required");

        Path outputPath = Path.of(output);
        Files.createDirectories(outputPath.getParent());
        captureSnapshot(outputPath);
        assertTrue(Files.size(outputPath) > 0L);
    }

    private static void initToolkit() throws InterruptedException {
        if (toolkitReady) {
            return;
        }
        synchronized (HexGridCanvasSnapshotTest.class) {
            if (toolkitReady) {
                return;
            }
            CountDownLatch latch = new CountDownLatch(1);
            try {
                Platform.startup(latch::countDown);
                if (!latch.await(5, TimeUnit.SECONDS)) {
                    throw new IllegalStateException("Timed out starting JavaFX toolkit");
                }
            } catch (IllegalStateException exception) {
                // Other JavaFX tests may have already initialized the toolkit in this JVM.
                String message = exception.getMessage();
                if (message == null || !message.contains("already initialized")) {
                    throw exception;
                }
            }
            toolkitReady = true;
        }
    }

    private static void captureSnapshot(Path outputPath) throws Exception {
        CountDownLatch latch = new CountDownLatch(1);
        AtomicReference<Throwable> failure = new AtomicReference<>();
        Platform.runLater(() -> {
            try {
                HexGridCanvas canvas = new HexGridCanvas();
                canvas.setSide(HexDocModel.Side.LEFT);
                canvas.resize(CANVAS_WIDTH, CANVAS_HEIGHT);
                canvas.setWidth(CANVAS_WIDTH);
                canvas.setHeight(CANVAS_HEIGHT);
                canvas.setModel(sampleModel());

                SnapshotParameters snapshotParameters = new SnapshotParameters();
                WritableImage image = canvas.snapshot(snapshotParameters, null);
                writePpm(outputPath, image);
            } catch (Throwable throwable) {
                failure.set(throwable);
            } finally {
                latch.countDown();
            }
        });
        if (!latch.await(10, TimeUnit.SECONDS)) {
            throw new IllegalStateException("Timed out while capturing canvas snapshot");
        }
        if (failure.get() != null) {
            throw new RuntimeException("Snapshot generation failed", failure.get());
        }
    }

    private static HexDocModel sampleModel() {
        byte[] left = new byte[96];
        byte[] right = new byte[96];
        for (int i = 0; i < left.length; i++) {
            left[i] = (byte) i;
            right[i] = (byte) i;
        }
        right[10] = (byte) 0xFF;
        right[21] = (byte) 0xA0;
        right[47] = (byte) 0x3C;
        right[63] = (byte) 0x00;
        return HexDocModel.fromBytes(Path.of("left.bin"), Path.of("right.bin"), left, right);
    }

    private static void writePpm(Path outputPath, WritableImage image) throws IOException {
        int width = (int) image.getWidth();
        int height = (int) image.getHeight();
        PixelReader reader = image.getPixelReader();

        try (OutputStream outputStream = Files.newOutputStream(outputPath)) {
            outputStream.write(("P6\n" + width + " " + height + "\n255\n").getBytes());
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    int argb = reader.getArgb(x, y);
                    outputStream.write((argb >> 16) & 0xFF);
                    outputStream.write((argb >> 8) & 0xFF);
                    outputStream.write(argb & 0xFF);
                }
            }
        }
    }
}
