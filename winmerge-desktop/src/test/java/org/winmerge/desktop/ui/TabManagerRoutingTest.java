package org.winmerge.desktop.ui;

import java.io.RandomAccessFile;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;

import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.io.TempDir;
import org.winmerge.desktop.ui.merge.MergeDocModel;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

class TabManagerRoutingTest {
    @TempDir
    Path tempDir;

    @Test
    void routesUtf16BomFilesToTextDiffInsteadOfHex() throws Exception {
        Path left = tempDir.resolve("left-utf16le.txt");
        Path right = tempDir.resolve("right-utf16le.txt");
        writeUtf16LeWithBom(left, "alpha\r\nbeta\r\n");
        writeUtf16LeWithBom(right, "alpha\r\ngamma\r\n");

        assertFalse(TabManager.shouldOpenHexView(left, right));
    }

    @Test
    void routesOversizedFilesToHexDiffForBoundedTextCompare() throws Exception {
        Path oversized = tempDir.resolve("oversized.txt");
        Path right = tempDir.resolve("right.txt");
        setLength(oversized, MergeDocModel.textCompareSizeLimitBytes() + 1);
        Files.writeString(right, "small\n", StandardCharsets.UTF_8);

        assertTrue(TabManager.shouldOpenHexView(oversized, right));
    }

    private static void writeUtf16LeWithBom(Path path, String text) throws Exception {
        byte[] payload = text.getBytes(StandardCharsets.UTF_16LE);
        byte[] bytes = new byte[payload.length + 2];
        bytes[0] = (byte) 0xFF;
        bytes[1] = (byte) 0xFE;
        System.arraycopy(payload, 0, bytes, 2, payload.length);
        Files.write(path, bytes);
    }

    private static void setLength(Path path, long length) throws Exception {
        try (RandomAccessFile file = new RandomAccessFile(path.toFile(), "rw")) {
            file.setLength(length);
        }
    }
}
