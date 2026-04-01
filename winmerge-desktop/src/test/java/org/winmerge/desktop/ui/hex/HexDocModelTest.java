package org.winmerge.desktop.ui.hex;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;

import org.junit.jupiter.api.Test;

class HexDocModelTest {
    @Test
    void formatsOffsetsAsEightDigitHexValues() {
        HexDocModel model = HexDocModel.fromBytes(Path.of("left.bin"), Path.of("right.bin"), new byte[0], new byte[0]);

        assertEquals("0x00000000", model.formatOffsetForRow(0));
        assertEquals("0x00000010", model.formatOffsetForRow(1));
        assertEquals("0x00000100", model.formatOffsetForRow(16));
    }

    @Test
    void marksChangedBytesAndReportsByteValuesBySide() {
        HexDocModel model = HexDocModel.fromBytes(
            Path.of("left.bin"),
            Path.of("right.bin"),
            new byte[] { 0x00, 0x10, 0x20, 0x30 },
            new byte[] { 0x00, 0x10, 0x2F, 0x30 }
        );

        assertFalse(model.isDifferentByte(1));
        assertTrue(model.isDifferentByte(2));
        assertEquals(0x20, model.unsignedByte(HexDocModel.Side.LEFT, 2));
        assertEquals(0x2F, model.unsignedByte(HexDocModel.Side.RIGHT, 2));
    }

    @Test
    void handlesLengthMismatchesAsDifferences() {
        HexDocModel model = HexDocModel.fromBytes(
            Path.of("left.bin"),
            Path.of("right.bin"),
            new byte[] { 0x01, 0x02 },
            new byte[] { 0x01, 0x02, 0x03 }
        );

        assertTrue(model.hasDiffs());
        assertTrue(model.isDifferentByte(2));
        assertFalse(model.hasByte(HexDocModel.Side.LEFT, 2));
        assertTrue(model.hasByte(HexDocModel.Side.RIGHT, 2));
        assertEquals(1, model.rowCount());
    }

    @Test
    void readBytesWithLimitReadsFileWhenWithinLimit() throws IOException {
        Path file = Files.createTempFile("hex-doc-model", ".bin");
        try {
            Files.write(file, new byte[] { 0x01, 0x02, 0x03 });
            byte[] bytes = HexDocModel.readBytesWithLimit(file, 3);
            assertEquals(3, bytes.length);
            assertEquals(0x03, bytes[2]);
        } finally {
            Files.deleteIfExists(file);
        }
    }

    @Test
    void readBytesWithLimitRejectsFilesAboveLimit() throws IOException {
        Path file = Files.createTempFile("hex-doc-model", ".bin");
        try {
            Files.write(file, new byte[] { 0x01, 0x02, 0x03, 0x04 });
            HexDocModel.HexLoadLimitExceededException exception = assertThrows(
                HexDocModel.HexLoadLimitExceededException.class,
                () -> HexDocModel.readBytesWithLimit(file, 3)
            );
            assertEquals(file, exception.path());
            assertEquals(4, exception.sizeBytes());
            assertEquals(3, exception.limitBytes());
        } finally {
            Files.deleteIfExists(file);
        }
    }
}
