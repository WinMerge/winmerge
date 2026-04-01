package org.winmerge.core.diff;

import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.io.TempDir;
import org.winmerge.core.io.FileTextStats;
import org.winmerge.core.io.NioFileSystemService;

import java.nio.ByteBuffer;
import java.nio.file.Files;
import java.nio.file.Path;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertNull;
import static org.junit.jupiter.api.Assertions.assertTrue;

class DiffFileDataTest {
    @TempDir
    Path tempDir;

    @Test
    void opensMappedBuffersAndCollectsTextStats() throws Exception {
        Path left = tempDir.resolve("left.txt");
        Path right = tempDir.resolve("right.txt");
        Files.write(left, new byte[] {'a', '\r', '\n', 'b', '\n', 0});
        Files.write(right, new byte[] {'x', '\r', 'z'});

        DiffFileData fileData = new DiffFileData(new NioFileSystemService());
        fileData.setDisplayFilepaths("left-display", "right-display");

        assertTrue(fileData.openFiles(left.toString(), right.toString()));
        assertTrue(fileData.isUsed());
        assertEquals("left-display", fileData.getDisplayFilepath(0));
        assertEquals("right-display", fileData.getDisplayFilepath(1));
        assertEquals(left.toString(), fileData.getFileLocation(0).getFilePath());
        assertEquals(right.toString(), fileData.getFileLocation(1).getFilePath());

        ByteBuffer leftBuffer = fileData.getMappedBuffer(0);
        ByteBuffer rightBuffer = fileData.getMappedBuffer(1);
        assertNotNull(leftBuffer);
        assertNotNull(rightBuffer);
        assertEquals(6, leftBuffer.remaining());
        assertEquals(3, rightBuffer.remaining());
        assertEquals(6, fileData.getFileSize(0));
        assertEquals(3, fileData.getFileSize(1));

        FileTextStats leftStats = fileData.getTextStats(0);
        assertEquals(0, leftStats.getCrCount());
        assertEquals(1, leftStats.getLfCount());
        assertEquals(1, leftStats.getCrlfCount());
        assertEquals(1, leftStats.getZeroCount());

        FileTextStats rightStats = fileData.getTextStats(1);
        assertEquals(1, rightStats.getCrCount());
        assertEquals(0, rightStats.getLfCount());
        assertEquals(0, rightStats.getCrlfCount());
        assertEquals(0, rightStats.getZeroCount());
    }

    @Test
    void handlesSameFileMappingAndReset() throws Exception {
        Path shared = tempDir.resolve("shared.txt");
        Files.writeString(shared, "same file");

        DiffFileData fileData = new DiffFileData(new NioFileSystemService());
        assertTrue(fileData.openFiles(shared.toString(), shared.toString()));
        assertTrue(fileData.isUsed());
        assertNotNull(fileData.getMappedBuffer(0));
        assertNotNull(fileData.getMappedBuffer(1));
        assertEquals(fileData.getFileSize(0), fileData.getFileSize(1));
        assertEquals(fileData.getTextStats(0).getLfCount(), fileData.getTextStats(1).getLfCount());

        fileData.reset();
        assertFalse(fileData.isUsed());
        assertNull(fileData.getMappedBuffer(0));
        assertNull(fileData.getMappedBuffer(1));
    }

    @Test
    void failsOpenWhenFileMissing() {
        DiffFileData fileData = new DiffFileData(new NioFileSystemService());
        assertFalse(fileData.openFiles(tempDir.resolve("missing-left.txt").toString(), tempDir.resolve("missing-right.txt").toString()));
        assertFalse(fileData.isUsed());
    }
}
