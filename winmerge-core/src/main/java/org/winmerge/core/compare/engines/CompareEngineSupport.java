package org.winmerge.core.compare.engines;

import org.winmerge.core.compare.DiffCode;
import org.winmerge.core.compare.WhitespaceIgnoreChoice;
import org.winmerge.core.io.FileTextStats;

import java.io.IOException;
import java.io.InputStream;
import java.nio.charset.StandardCharsets;
import java.nio.file.Path;
import java.util.Arrays;
import java.util.Locale;
import java.util.function.IntSupplier;

final class CompareEngineSupport {
    private static final int BUFFER_SIZE = 256 * 1024;

    private CompareEngineSupport() {
    }

    static int resolveThreeWay(IntSupplier pair10, IntSupplier pair12, IntSupplier pair02) {
        int code10 = pair10.getAsInt();
        int code12 = pair12.getAsInt();
        if (code10 == DiffCode.CMPABORT || code12 == DiffCode.CMPABORT) {
            return DiffCode.CMPABORT;
        }

        int code02 = DiffCode.SAME;
        int result;
        if (code10 == DiffCode.SAME && code12 == DiffCode.SAME) {
            result = DiffCode.SAME;
        } else if (code10 == DiffCode.SAME) {
            result = DiffCode.DIFF | DiffCode.DIFF3RDONLY;
        } else if (code12 == DiffCode.SAME) {
            result = DiffCode.DIFF | DiffCode.DIFF1STONLY;
        } else {
            code02 = pair02.getAsInt();
            if (code02 == DiffCode.CMPABORT) {
                return DiffCode.CMPABORT;
            }
            result = (code02 == DiffCode.SAME) ? DiffCode.DIFF | DiffCode.DIFF2NDONLY : DiffCode.DIFF;
        }

        if (code10 == DiffCode.CMPERR || code12 == DiffCode.CMPERR || code02 == DiffCode.CMPERR) {
            return DiffCode.CMPERR;
        }
        return result;
    }

    static int compareBinaryStreams(
            Path left,
            Path right,
            CompareEngineContext context
    ) {
        try (InputStream leftIn = java.nio.file.Files.newInputStream(left);
             InputStream rightIn = java.nio.file.Files.newInputStream(right)) {
            byte[] leftBuffer = new byte[BUFFER_SIZE];
            byte[] rightBuffer = new byte[BUFFER_SIZE];

            while (true) {
                if (context.getAbortChecker().shouldAbort()) {
                    return DiffCode.CMPABORT;
                }
                int leftRead = leftIn.read(leftBuffer);
                int rightRead = rightIn.read(rightBuffer);
                if (leftRead < 0 || rightRead < 0) {
                    if (leftRead == rightRead) {
                        return DiffCode.SAME;
                    }
                    return DiffCode.DIFF;
                }
                if (leftRead != rightRead) {
                    return DiffCode.DIFF;
                }
                if (!Arrays.equals(
                        Arrays.copyOf(leftBuffer, leftRead),
                        Arrays.copyOf(rightBuffer, rightRead))) {
                    return DiffCode.DIFF;
                }
            }
        } catch (IOException ex) {
            return DiffCode.CMPERR;
        }
    }

    static int compareTextWithOptions(
            byte[] leftBytes,
            byte[] rightBytes,
            CompareEngineContext context
    ) {
        String left = new String(leftBytes, StandardCharsets.ISO_8859_1);
        String right = new String(rightBytes, StandardCharsets.ISO_8859_1);

        String normalizedLeft = normalizeText(left, context);
        String normalizedRight = normalizeText(right, context);
        return normalizedLeft.equals(normalizedRight) ? DiffCode.SAME : DiffCode.DIFF;
    }

    static void collectTextStats(FileTextStats stats, byte[] bytes) {
        stats.clear();
        for (int i = 0; i < bytes.length; i++) {
            byte value = bytes[i];
            if (value == 0) {
                stats.setZeroCount(stats.getZeroCount() + 1);
            } else if (value == '\r') {
                if (i + 1 < bytes.length && bytes[i + 1] == '\n') {
                    stats.setCrlfCount(stats.getCrlfCount() + 1);
                    i++;
                } else {
                    stats.setCrCount(stats.getCrCount() + 1);
                }
            } else if (value == '\n') {
                stats.setLfCount(stats.getLfCount() + 1);
            }
        }
    }

    static int binarySideFlagsFromStats(FileTextStats left, FileTextStats right) {
        boolean leftBinary = left.getZeroCount() > 0;
        boolean rightBinary = right.getZeroCount() > 0;
        if (leftBinary && rightBinary) {
            return DiffCode.BIN | DiffCode.BINSIDE1 | DiffCode.BINSIDE2;
        }
        if (leftBinary) {
            return DiffCode.BIN | DiffCode.BINSIDE1;
        }
        if (rightBinary) {
            return DiffCode.BIN | DiffCode.BINSIDE2;
        }
        return DiffCode.TEXT;
    }

    private static String normalizeText(String value, CompareEngineContext context) {
        String normalized = value;
        if (context.getQuickCompareOptions().isIgnoreLineBreaks()) {
            normalized = normalized.replace("\r\n", "").replace("\r", "").replace("\n", "");
        } else if (context.getQuickCompareOptions().isIgnoreEolDifference()) {
            normalized = normalized.replace("\r\n", "\n").replace('\r', '\n');
        }
        if (context.getQuickCompareOptions().isIgnoreMissingTrailingEol()) {
            normalized = trimSingleTrailingEol(normalized);
        }
        if (context.getQuickCompareOptions().isIgnoreBlankLines()) {
            normalized = Arrays.stream(normalized.split("\n", -1))
                    .filter(line -> !line.trim().isEmpty())
                    .reduce((a, b) -> a + "\n" + b)
                    .orElse("");
        }

        WhitespaceIgnoreChoice ws = context.getQuickCompareOptions().getIgnoreWhitespace();
        if (ws == WhitespaceIgnoreChoice.IGNORE_ALL) {
            normalized = normalized.replace(" ", "").replace("\t", "");
        } else if (ws == WhitespaceIgnoreChoice.IGNORE_CHANGE) {
            normalized = normalized.replaceAll("[ \\t]+", " ");
        }

        if (context.getQuickCompareOptions().isIgnoreNumbers()) {
            normalized = normalized.replaceAll("\\d+", "");
        }
        if (context.getQuickCompareOptions().isIgnoreCase()) {
            normalized = normalized.toLowerCase(Locale.ROOT);
        }
        return normalized;
    }

    private static String trimSingleTrailingEol(String value) {
        if (value.endsWith("\r\n")) {
            return value.substring(0, value.length() - 2);
        }
        if (value.endsWith("\n") || value.endsWith("\r")) {
            return value.substring(0, value.length() - 1);
        }
        return value;
    }
}
