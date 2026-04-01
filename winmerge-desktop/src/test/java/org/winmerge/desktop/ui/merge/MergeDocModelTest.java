package org.winmerge.desktop.ui.merge;

import java.io.IOException;
import java.io.RandomAccessFile;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.List;

import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.io.TempDir;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.junit.jupiter.api.Assertions.assertThrows;

class MergeDocModelTest {
    @TempDir
    Path tempDir;

    @Test
    void computesDiffChunksForLineReplacement() {
        MergeDocModel model = MergeDocModel.fromTexts(
            Path.of("left.java"),
            Path.of("right.java"),
            "a\nb\nc\n",
            "a\nx\nc\n"
        );

        assertTrue(model.hasDiffs());
        assertEquals(List.of(new DiffChunk(1, 2, 1, 2)), model.diffChunks());
    }

    @Test
    void supportsMergeLeftChunkToRight() {
        MergeDocModel model = MergeDocModel.fromTexts(
            Path.of("left.java"),
            Path.of("right.java"),
            "a\nleft-only\nc\n",
            "a\nright-only\nc\n"
        );

        MergeDocModel merged = model.mergeLeftChunkToRight(0);
        assertFalse(merged.hasDiffs());
        assertEquals(model.leftText(), merged.rightText());
    }

    @Test
    void convergesMergeWhenRightHasNoEolStyle() {
        MergeDocModel model = MergeDocModel.fromTexts(
            Path.of("left.txt"),
            Path.of("right.txt"),
            "alpha\r\nbeta\r\n",
            ""
        );

        MergeDocModel merged = model.mergeLeftChunkToRight(0);
        assertFalse(merged.hasDiffs());
        assertEquals("alpha\r\nbeta\r\n", merged.rightText());
    }

    @Test
    void navigatesDiffIndicesWithWraparound() {
        MergeDocModel model = MergeDocModel.fromTexts(
            Path.of("left.java"),
            Path.of("right.java"),
            "a\nb\nc\nd\n",
            "x\nb\ny\nd\n"
        );

        assertEquals(0, model.nextDiffIndex(-1));
        assertEquals(1, model.nextDiffIndex(0));
        assertEquals(0, model.nextDiffIndex(1));
        assertEquals(1, model.previousDiffIndex(0));
        assertEquals(0, model.previousDiffIndex(1));
    }

    @Test
    void detectsEolOnlyDifferences() {
        MergeDocModel model = MergeDocModel.fromTexts(
            Path.of("left.txt"),
            Path.of("right.txt"),
            "line1\r\nline2\r\n",
            "line1\nline2\n"
        );

        assertTrue(model.hasDiffs());
        assertEquals(List.of(new DiffChunk(0, 2, 0, 2)), model.diffChunks());
    }

    @Test
    void capturesInsertionAndDeletionEdgeChunks() {
        MergeDocModel deletionModel = MergeDocModel.fromTexts(
            Path.of("left.txt"),
            Path.of("right.txt"),
            "a\nb\nc\n",
            "a\nc\n"
        );
        assertEquals(List.of(new DiffChunk(1, 2, 1, 1)), deletionModel.diffChunks());

        MergeDocModel insertionModel = MergeDocModel.fromTexts(
            Path.of("left.txt"),
            Path.of("right.txt"),
            "a\nc\n",
            "a\nb\nc\n"
        );
        assertEquals(List.of(new DiffChunk(1, 1, 1, 2)), insertionModel.diffChunks());
    }

    @Test
    void usesGuardedFallbackForOversizedDiffs() {
        String leftText = buildLineSeries("left-", 3_000);
        String rightText = buildLineSeries("right-", 3_000);

        MergeDocModel model = MergeDocModel.fromTexts(
            Path.of("left.txt"),
            Path.of("right.txt"),
            leftText,
            rightText
        );

        assertTrue(model.usedGuardedDiffFallback());
        assertEquals(List.of(new DiffChunk(0, 3_000, 0, 3_000)), model.diffChunks());
    }

    @Test
    void loadsUtf16WithBomWithoutUtf8DecodeFailure() throws Exception {
        Path left = tempDir.resolve("left-utf16le.txt");
        Path right = tempDir.resolve("right-utf16le.txt");

        writeUtf16LeWithBom(left, "alpha\r\nbeta\r\n");
        writeUtf16LeWithBom(right, "alpha\r\nbeta\r\n");

        MergeDocModel model = MergeDocModel.load(left, right);
        assertFalse(model.hasDiffs());
        assertEquals("alpha\r\nbeta\r\n", model.leftText());
        assertEquals("alpha\r\nbeta\r\n", model.rightText());
    }

    @Test
    void rejectsOversizedFilesBeforeDecode() throws Exception {
        Path oversized = tempDir.resolve("oversized.txt");
        Path right = tempDir.resolve("right.txt");
        setLength(oversized, MergeDocModel.textCompareSizeLimitBytes() + 1);
        Files.writeString(right, "alpha\n", StandardCharsets.UTF_8);

        IOException error = assertThrows(IOException.class, () -> MergeDocModel.load(oversized, right));
        assertTrue(error.getMessage().contains("text compare size limit"));
    }

    private static String buildLineSeries(String prefix, int count) {
        StringBuilder builder = new StringBuilder(count * 8);
        for (int i = 0; i < count; i++) {
            if (i > 0) {
                builder.append('\n');
            }
            builder.append(prefix).append(i);
        }
        return builder.toString();
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
