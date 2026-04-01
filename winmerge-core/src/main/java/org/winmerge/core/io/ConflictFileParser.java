package org.winmerge.core.io;

import java.io.IOException;
import java.nio.charset.Charset;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.List;

public final class ConflictFileParser {
    private static final String SEPARATOR = "=======";
    private static final String THEIRS_END = ">>>>>>> ";
    private static final String MINE_BEGIN = "<<<<<<< ";
    private static final String BASE_BEGIN = "||||||| ";

    private ConflictFileParser() {
    }

    public static boolean isConflictFile(Path conflictFilePath) {
        return isConflictFile(conflictFilePath, FileSystemService.createDefault());
    }

    public static boolean isConflictFile(Path conflictFilePath, FileSystemService fileSystem) {
        UnicodeFileReader reader = new UnicodeFileReader(fileSystem);
        try {
            UnicodeFileReader.ReadResult readResult = reader.readAll(conflictFilePath);
            for (LineToken token : splitLines(readResult.content())) {
                if (token.line().startsWith(MINE_BEGIN)) {
                    return true;
                }
            }
            return false;
        } catch (IOException ex) {
            return false;
        }
    }

    public static ParseResult parseConflictFile(
            Path conflictFilePath,
            Path workingCopyFilePath,
            Path newRevisionFilePath,
            Path baseRevisionFilePath,
            int guessEncodingType) throws IOException {
        return parseConflictFile(
                conflictFilePath,
                workingCopyFilePath,
                newRevisionFilePath,
                baseRevisionFilePath,
                guessEncodingType,
                FileSystemService.createDefault());
    }

    public static ParseResult parseConflictFile(
            Path conflictFilePath,
            Path workingCopyFilePath,
            Path newRevisionFilePath,
            Path baseRevisionFilePath,
            int guessEncodingType,
            FileSystemService fileSystem) throws IOException {
        UnicodeFileWriter writer = new UnicodeFileWriter(fileSystem);
        byte[] contentBytes = fileSystem.readAllBytes(conflictFilePath);
        FileTextEncoding encoding = guessEncoding(contentBytes, guessEncodingType);
        String content = decode(contentBytes, encoding);

        StringBuilder workingCopy = new StringBuilder();
        StringBuilder newRevision = new StringBuilder();
        StringBuilder baseRevision = new StringBuilder();

        int state = 0;
        int nestingLevel = 0;
        boolean nestedConflicts = false;
        boolean threeWay = false;
        boolean parsedConflict = false;
        String revision = "none";

        for (LineToken token : splitLines(content)) {
            String line = token.line();
            String eol = token.eol();
            int pos;

            switch (state) {
                case 0:
                    if (line.startsWith(MINE_BEGIN)) {
                        state = 1;
                        parsedConflict = true;
                    } else {
                        appendLine(newRevision, line, eol);
                        appendLine(baseRevision, line, eol);
                        appendLine(workingCopy, line, eol);
                    }
                    break;
                case 1:
                    if (line.startsWith(MINE_BEGIN)) {
                        state = 3;
                        appendLine(workingCopy, line, eol);
                        break;
                    }

                    pos = line.indexOf(BASE_BEGIN);
                    if (pos != -1) {
                        String head = line.substring(0, pos);
                        if (!head.isEmpty()) {
                            appendLine(baseRevision, head, eol);
                        }
                        state = 5;
                        threeWay = true;
                        break;
                    }

                    pos = line.indexOf(SEPARATOR);
                    if (pos != -1 && pos == line.length() - SEPARATOR.length()) {
                        String head = line.substring(0, pos);
                        if (!head.isEmpty()) {
                            appendLine(workingCopy, head, eol);
                        }
                        state = 2;
                    } else {
                        appendLine(workingCopy, line, eol);
                    }
                    break;
                case 2:
                    if (line.startsWith(MINE_BEGIN)) {
                        state = 4;
                        appendLine(newRevision, line, eol);
                        break;
                    }
                    pos = line.indexOf(THEIRS_END);
                    if (pos != -1) {
                        revision = line.substring(pos + THEIRS_END.length());
                        String head = line.substring(0, pos);
                        if (!head.isEmpty()) {
                            appendLine(newRevision, head, eol);
                        }
                        state = 0;
                    } else {
                        appendLine(newRevision, line, eol);
                    }
                    break;
                case 3:
                    nestedConflicts = true;
                    if (line.startsWith(MINE_BEGIN)) {
                        nestingLevel++;
                    } else if (line.contains(THEIRS_END)) {
                        if (nestingLevel == 0) {
                            state = 1;
                        } else {
                            nestingLevel--;
                        }
                    }
                    appendLine(workingCopy, line, eol);
                    break;
                case 4:
                    if (line.startsWith(MINE_BEGIN)) {
                        nestingLevel++;
                    } else if (line.contains(THEIRS_END)) {
                        if (nestingLevel == 0) {
                            state = 2;
                        } else {
                            nestingLevel--;
                        }
                    }
                    appendLine(newRevision, line, eol);
                    break;
                case 5:
                    pos = line.indexOf(SEPARATOR);
                    if (pos != -1 && pos == line.length() - SEPARATOR.length()) {
                        String head = line.substring(0, pos);
                        if (!head.isEmpty()) {
                            appendLine(baseRevision, head, eol);
                        }
                        state = 2;
                    } else {
                        appendLine(baseRevision, line, eol);
                    }
                    break;
                default:
                    throw new IllegalStateException("Unexpected parser state: " + state);
            }
        }

        writer.write(workingCopyFilePath, workingCopy.toString(), encoding, encoding.hasBom());
        writer.write(newRevisionFilePath, newRevision.toString(), encoding, encoding.hasBom());
        writer.write(baseRevisionFilePath, baseRevision.toString(), encoding, encoding.hasBom());

        return new ParseResult(parsedConflict, nestedConflicts, threeWay, revision);
    }

    private static FileTextEncoding guessEncoding(byte[] contentBytes, int guessEncodingType) {
        FileTextEncoding encoding = detectBomEncoding(contentBytes);
        if (encoding.getUnicoding() != UnicodeEncoding.NONE) {
            return encoding;
        }

        encoding.setUnicoding(UnicodeEncoding.UTF8);
        encoding.setBom(false);

        if (guessEncodingType == 0) {
            return encoding;
        }

        if (isValidUtf8(contentBytes)) {
            return encoding;
        }

        if ((guessEncodingType & 2) != 0) {
            encoding.setCodepage(1252);
        }
        return encoding;
    }

    private static FileTextEncoding detectBomEncoding(byte[] contentBytes) {
        FileTextEncoding encoding = new FileTextEncoding();
        if (contentBytes.length >= 3
                && (contentBytes[0] & 0xFF) == 0xEF
                && (contentBytes[1] & 0xFF) == 0xBB
                && (contentBytes[2] & 0xFF) == 0xBF) {
            encoding.setUnicoding(UnicodeEncoding.UTF8);
            encoding.setBom(true);
            return encoding;
        }
        if (contentBytes.length >= 2
                && (contentBytes[0] & 0xFF) == 0xFF
                && (contentBytes[1] & 0xFF) == 0xFE) {
            encoding.setUnicoding(UnicodeEncoding.UCS2LE);
            encoding.setBom(true);
            return encoding;
        }
        if (contentBytes.length >= 2
                && (contentBytes[0] & 0xFF) == 0xFE
                && (contentBytes[1] & 0xFF) == 0xFF) {
            encoding.setUnicoding(UnicodeEncoding.UCS2BE);
            encoding.setBom(true);
            return encoding;
        }
        return encoding;
    }

    private static String decode(byte[] contentBytes, FileTextEncoding encoding) {
        int offset = 0;
        if (encoding.hasBom()) {
            if (encoding.getUnicoding() == UnicodeEncoding.UTF8 && contentBytes.length >= 3) {
                offset = 3;
            } else if ((encoding.getUnicoding() == UnicodeEncoding.UCS2LE
                    || encoding.getUnicoding() == UnicodeEncoding.UCS2BE) && contentBytes.length >= 2) {
                offset = 2;
            }
        }
        Charset charset = encoding.toCharset();
        return new String(contentBytes, offset, contentBytes.length - offset, charset);
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

    private static void appendLine(StringBuilder builder, String line, String eol) {
        builder.append(line).append(eol);
    }

    private static List<LineToken> splitLines(String content) {
        List<LineToken> lines = new ArrayList<>();
        int index = 0;
        int start = 0;
        while (index < content.length()) {
            char c = content.charAt(index);
            if (c == '\n' || c == '\r') {
                String line = content.substring(start, index);
                String eol;
                if (c == '\r' && index + 1 < content.length() && content.charAt(index + 1) == '\n') {
                    eol = "\r\n";
                    index++;
                } else {
                    eol = String.valueOf(c);
                }
                lines.add(new LineToken(line, eol));
                index++;
                start = index;
                continue;
            }
            index++;
        }
        if (start < content.length()) {
            lines.add(new LineToken(content.substring(start), ""));
        }
        return lines;
    }

    private record LineToken(String line, String eol) {
    }

    public record ParseResult(boolean parsed, boolean nestedConflicts, boolean threeWay, String revision) {
    }
}
