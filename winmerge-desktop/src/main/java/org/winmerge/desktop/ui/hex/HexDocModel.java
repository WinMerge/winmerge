package org.winmerge.desktop.ui.hex;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.BitSet;
import java.util.Objects;

public final class HexDocModel {
    public static final int BYTES_PER_ROW = 16;

    private final Path leftPath;
    private final Path rightPath;
    private final byte[] leftBytes;
    private final byte[] rightBytes;
    private final BitSet diffByteIndexes;
    private final int maxLength;

    private HexDocModel(Path leftPath, Path rightPath, byte[] leftBytes, byte[] rightBytes) {
        this.leftPath = Objects.requireNonNull(leftPath, "leftPath");
        this.rightPath = Objects.requireNonNull(rightPath, "rightPath");
        this.leftBytes = leftBytes;
        this.rightBytes = rightBytes;
        this.maxLength = Math.max(leftBytes.length, rightBytes.length);
        this.diffByteIndexes = computeDiffByteIndexes(leftBytes, rightBytes, maxLength);
    }

    public static HexDocModel load(Path leftPath, Path rightPath) throws IOException {
        Objects.requireNonNull(leftPath, "leftPath");
        Objects.requireNonNull(rightPath, "rightPath");
        return fromBytes(leftPath, rightPath, Files.readAllBytes(leftPath), Files.readAllBytes(rightPath));
    }

    public static HexDocModel fromBytes(Path leftPath, Path rightPath, byte[] leftBytes, byte[] rightBytes) {
        Objects.requireNonNull(leftPath, "leftPath");
        Objects.requireNonNull(rightPath, "rightPath");
        Objects.requireNonNull(leftBytes, "leftBytes");
        Objects.requireNonNull(rightBytes, "rightBytes");
        return new HexDocModel(leftPath, rightPath, leftBytes.clone(), rightBytes.clone());
    }

    public Path leftPath() {
        return leftPath;
    }

    public Path rightPath() {
        return rightPath;
    }

    public int maxLength() {
        return maxLength;
    }

    public int rowCount() {
        if (maxLength == 0) {
            return 1;
        }
        return (maxLength + BYTES_PER_ROW - 1) / BYTES_PER_ROW;
    }

    public boolean hasByte(Side side, int byteIndex) {
        if (byteIndex < 0) {
            return false;
        }
        byte[] bytes = bytesFor(side);
        return byteIndex < bytes.length;
    }

    public int unsignedByte(Side side, int byteIndex) {
        byte[] bytes = bytesFor(side);
        if (byteIndex < 0 || byteIndex >= bytes.length) {
            return -1;
        }
        return bytes[byteIndex] & 0xFF;
    }

    public boolean isDifferentByte(int byteIndex) {
        if (byteIndex < 0 || byteIndex >= maxLength) {
            return false;
        }
        return diffByteIndexes.get(byteIndex);
    }

    public boolean hasDiffs() {
        return !diffByteIndexes.isEmpty();
    }

    public String formatOffsetForRow(int rowIndex) {
        if (rowIndex < 0) {
            throw new IllegalArgumentException("rowIndex must be >= 0");
        }
        long offset = ((long) rowIndex) * BYTES_PER_ROW;
        return String.format("0x%08X", offset & 0xFFFFFFFFL);
    }

    private byte[] bytesFor(Side side) {
        return side == Side.LEFT ? leftBytes : rightBytes;
    }

    private static BitSet computeDiffByteIndexes(byte[] left, byte[] right, int maxLength) {
        BitSet bitSet = new BitSet(maxLength);
        for (int index = 0; index < maxLength; index++) {
            boolean leftPresent = index < left.length;
            boolean rightPresent = index < right.length;
            if (leftPresent != rightPresent) {
                bitSet.set(index);
                continue;
            }
            if (leftPresent && left[index] != right[index]) {
                bitSet.set(index);
            }
        }
        return bitSet;
    }

    public enum Side {
        LEFT,
        RIGHT
    }
}
