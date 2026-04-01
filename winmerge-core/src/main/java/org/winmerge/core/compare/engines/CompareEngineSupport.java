package org.winmerge.core.compare.engines;

import org.winmerge.core.compare.DiffCode;
import org.winmerge.core.compare.WhitespaceIgnoreChoice;
import org.winmerge.core.io.FileTextEncoding;
import org.winmerge.core.io.FileTextStats;
import org.winmerge.core.io.UnicodeEncoding;

import java.io.IOException;
import java.io.InputStream;
import java.nio.charset.Charset;
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
        FileTextEncoding leftEncoding = detectEncoding(leftBytes);
        FileTextEncoding rightEncoding = detectEncoding(rightBytes);
        String left = decodeUsingEncoding(leftBytes, leftEncoding);
        String right = decodeUsingEncoding(rightBytes, rightEncoding);

        String normalizedLeft = normalizeTextForComparison(left, context);
        String normalizedRight = normalizeTextForComparison(right, context);
        return normalizedLeft.equals(normalizedRight) ? DiffCode.SAME : DiffCode.DIFF;
    }

    static String normalizeTextForComparison(String value, CompareEngineContext context) {
        return normalizeText(value, context);
    }

    static FileTextEncoding detectEncoding(byte[] bytes) {
        FileTextEncoding encoding = new FileTextEncoding();
        if (bytes.length >= 3
                && (bytes[0] & 0xFF) == 0xEF
                && (bytes[1] & 0xFF) == 0xBB
                && (bytes[2] & 0xFF) == 0xBF) {
            encoding.setUnicoding(UnicodeEncoding.UTF8);
            encoding.setBom(true);
            return encoding;
        }
        if (bytes.length >= 2
                && (bytes[0] & 0xFF) == 0xFF
                && (bytes[1] & 0xFF) == 0xFE) {
            encoding.setUnicoding(UnicodeEncoding.UCS2LE);
            encoding.setBom(true);
            return encoding;
        }
        if (bytes.length >= 2
                && (bytes[0] & 0xFF) == 0xFE
                && (bytes[1] & 0xFF) == 0xFF) {
            encoding.setUnicoding(UnicodeEncoding.UCS2BE);
            encoding.setBom(true);
            return encoding;
        }
        if (isValidUtf8(bytes)) {
            encoding.setUnicoding(UnicodeEncoding.UTF8);
            encoding.setBom(false);
            return encoding;
        }
        encoding.setCodepage(systemDefaultCodepage());
        return encoding;
    }

    static String decodeUsingEncoding(byte[] bytes, FileTextEncoding encoding) {
        int offset = 0;
        if (encoding.hasBom()) {
            if (encoding.getUnicoding() == UnicodeEncoding.UTF8 && bytes.length >= 3) {
                offset = 3;
            } else if ((encoding.getUnicoding() == UnicodeEncoding.UCS2LE
                    || encoding.getUnicoding() == UnicodeEncoding.UCS2BE) && bytes.length >= 2) {
                offset = 2;
            }
        }
        Charset charset = encoding.toCharset();
        return new String(bytes, offset, bytes.length - offset, charset);
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
            normalized = normalized.replaceAll("[ \\t]+(?=\\r?\\n|$)", "");
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

    private static int systemDefaultCodepage() {
        String name = Charset.defaultCharset().name().toUpperCase(Locale.ROOT);
        if (name.equals("UTF-8")) {
            return FileTextEncoding.CP_UTF_8;
        }
        if (name.startsWith("WINDOWS-")) {
            return parsePositiveInt(name.substring("WINDOWS-".length()), FileTextEncoding.CP_UTF_8);
        }
        if (name.startsWith("CP")) {
            return parsePositiveInt(name.substring(2), FileTextEncoding.CP_UTF_8);
        }
        return FileTextEncoding.CP_UTF_8;
    }

    private static int parsePositiveInt(String value, int fallback) {
        try {
            int parsed = Integer.parseInt(value);
            return parsed > 0 ? parsed : fallback;
        } catch (NumberFormatException ignored) {
            return fallback;
        }
    }

    private static boolean isValidUtf8(byte[] bytes) {
        int i = 0;
        while (i < bytes.length) {
            int b = bytes[i] & 0xFF;
            if (b <= 0x7F) {
                i++;
                continue;
            }
            if ((b >> 5) == 0b110) {
                if (!isContinuation(bytes, i + 1)) {
                    return false;
                }
                i += 2;
                continue;
            }
            if ((b >> 4) == 0b1110) {
                if (!isContinuation(bytes, i + 1) || !isContinuation(bytes, i + 2)) {
                    return false;
                }
                i += 3;
                continue;
            }
            if ((b >> 3) == 0b11110) {
                if (!isContinuation(bytes, i + 1) || !isContinuation(bytes, i + 2) || !isContinuation(bytes, i + 3)) {
                    return false;
                }
                i += 4;
                continue;
            }
            return false;
        }
        return true;
    }

    private static boolean isContinuation(byte[] bytes, int index) {
        if (index >= bytes.length) {
            return false;
        }
        int b = bytes[index] & 0xFF;
        return (b >> 6) == 0b10;
    }
}
