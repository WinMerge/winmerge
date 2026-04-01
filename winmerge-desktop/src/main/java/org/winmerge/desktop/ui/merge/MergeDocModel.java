package org.winmerge.desktop.ui.merge;

import java.io.IOException;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Objects;

import org.winmerge.core.io.FileTextEncoding;
import org.winmerge.core.io.NioFileSystemService;
import org.winmerge.core.io.UnicodeFileReader;

public final class MergeDocModel {
    private static final long MAX_LCS_MATRIX_CELLS = 8_000_000L;
    private static final String DEFAULT_EOL = "\n";
    private static final UnicodeFileReader FILE_READER = new UnicodeFileReader(new NioFileSystemService());

    private final Path leftPath;
    private final Path rightPath;
    private final String leftText;
    private final String rightText;
    private final List<TextLine> leftLines;
    private final List<TextLine> rightLines;
    private final List<DiffChunk> diffChunks;
    private final boolean guardedDiffFallbackUsed;
    private final String preferredRightEol;
    private final FileTextEncoding leftEncoding;
    private final FileTextEncoding rightEncoding;

    private MergeDocModel(
        Path leftPath,
        Path rightPath,
        String leftText,
        String rightText,
        List<TextLine> leftLines,
        List<TextLine> rightLines,
        List<DiffChunk> diffChunks,
        boolean guardedDiffFallbackUsed,
        String preferredRightEol,
        FileTextEncoding leftEncoding,
        FileTextEncoding rightEncoding
    ) {
        this.leftPath = leftPath;
        this.rightPath = rightPath;
        this.leftText = leftText;
        this.rightText = rightText;
        this.leftLines = leftLines;
        this.rightLines = rightLines;
        this.diffChunks = diffChunks;
        this.guardedDiffFallbackUsed = guardedDiffFallbackUsed;
        this.preferredRightEol = preferredRightEol;
        this.leftEncoding = copyEncoding(leftEncoding);
        this.rightEncoding = copyEncoding(rightEncoding);
    }

    public static MergeDocModel load(Path leftPath, Path rightPath) throws IOException {
        Objects.requireNonNull(leftPath, "leftPath");
        Objects.requireNonNull(rightPath, "rightPath");
        UnicodeFileReader.ReadResult left = FILE_READER.readAll(leftPath);
        UnicodeFileReader.ReadResult right = FILE_READER.readAll(rightPath);
        return fromTextsWithEncodings(
            leftPath,
            rightPath,
            left.content(),
            right.content(),
            left.encoding(),
            right.encoding()
        );
    }

    public static MergeDocModel fromTexts(Path leftPath, Path rightPath, String leftText, String rightText) {
        return fromTextsWithEncodings(
            leftPath,
            rightPath,
            leftText,
            rightText,
            defaultUtf8Encoding(),
            defaultUtf8Encoding()
        );
    }

    private static MergeDocModel fromTextsWithEncodings(
        Path leftPath,
        Path rightPath,
        String leftText,
        String rightText,
        FileTextEncoding leftEncoding,
        FileTextEncoding rightEncoding
    ) {
        Objects.requireNonNull(leftPath, "leftPath");
        Objects.requireNonNull(rightPath, "rightPath");
        Objects.requireNonNull(leftText, "leftText");
        Objects.requireNonNull(rightText, "rightText");
        Objects.requireNonNull(leftEncoding, "leftEncoding");
        Objects.requireNonNull(rightEncoding, "rightEncoding");

        List<TextLine> leftLines = splitLines(leftText);
        List<TextLine> rightLines = splitLines(rightText);
        DiffComputation diffComputation = computeDiffChunks(toComparableLines(leftLines), toComparableLines(rightLines));

        return new MergeDocModel(
            leftPath,
            rightPath,
            leftText,
            rightText,
            Collections.unmodifiableList(leftLines),
            Collections.unmodifiableList(rightLines),
            Collections.unmodifiableList(diffComputation.chunks()),
            diffComputation.usedGuardedFallback(),
            detectPreferredEol(rightText),
            leftEncoding,
            rightEncoding
        );
    }

    public Path leftPath() {
        return leftPath;
    }

    public Path rightPath() {
        return rightPath;
    }

    public String leftText() {
        return leftText;
    }

    public String rightText() {
        return rightText;
    }

    public List<DiffChunk> diffChunks() {
        return diffChunks;
    }

    public boolean usedGuardedDiffFallback() {
        return guardedDiffFallbackUsed;
    }

    public FileTextEncoding rightEncoding() {
        return copyEncoding(rightEncoding);
    }

    public int leftLineCount() {
        return leftLines.size();
    }

    public int rightLineCount() {
        return rightLines.size();
    }

    public boolean hasDiffs() {
        return !diffChunks.isEmpty();
    }

    public int nextDiffIndex(int currentDiffIndex) {
        if (diffChunks.isEmpty()) {
            return -1;
        }
        if (currentDiffIndex < 0 || currentDiffIndex >= diffChunks.size() - 1) {
            return 0;
        }
        return currentDiffIndex + 1;
    }

    public int previousDiffIndex(int currentDiffIndex) {
        if (diffChunks.isEmpty()) {
            return -1;
        }
        if (currentDiffIndex <= 0 || currentDiffIndex >= diffChunks.size()) {
            return diffChunks.size() - 1;
        }
        return currentDiffIndex - 1;
    }

    public MergeDocModel mergeLeftChunkToRight(int diffIndex) {
        if (diffIndex < 0 || diffIndex >= diffChunks.size()) {
            return this;
        }
        DiffChunk chunk = diffChunks.get(diffIndex);
        List<TextLine> mergedRightLines = new ArrayList<>(rightLines);
        int rightStart = chunk.rightStartLine();
        int rightEnd = chunk.rightEndLine();
        mergedRightLines.subList(rightStart, rightEnd).clear();
        List<TextLine> replacementLines = normalizeLineEndings(
            leftLines.subList(chunk.leftStartLine(), chunk.leftEndLine()),
            preferredRightEol
        );
        mergedRightLines.addAll(rightStart, replacementLines);
        return fromTextsWithEncodings(leftPath, rightPath, leftText, joinLines(mergedRightLines), leftEncoding, rightEncoding);
    }

    private static FileTextEncoding defaultUtf8Encoding() {
        FileTextEncoding encoding = new FileTextEncoding();
        encoding.setCodepage(FileTextEncoding.CP_UTF_8);
        encoding.setBom(false);
        return encoding;
    }

    private static FileTextEncoding copyEncoding(FileTextEncoding source) {
        FileTextEncoding copy = new FileTextEncoding();
        copy.setCodepage(source.getCodepage());
        copy.setBom(source.hasBom());
        return copy;
    }

    private static List<TextLine> splitLines(String text) {
        List<TextLine> lines = new ArrayList<>();
        int start = 0;
        int index = 0;
        while (index < text.length()) {
            char value = text.charAt(index);
            if (value == '\r' || value == '\n') {
                String eol;
                if (value == '\r' && index + 1 < text.length() && text.charAt(index + 1) == '\n') {
                    eol = "\r\n";
                    index += 2;
                } else {
                    eol = value == '\r' ? "\r" : "\n";
                    index++;
                }
                lines.add(new TextLine(text.substring(start, index - eol.length()), eol));
                start = index;
                continue;
            }
            index++;
        }
        if (start < text.length()) {
            lines.add(new TextLine(text.substring(start), ""));
        } else if (text.isEmpty() || hasTerminalEol(text)) {
            lines.add(new TextLine("", ""));
        }
        return lines;
    }

    private static List<String> toComparableLines(List<TextLine> lines) {
        List<String> comparable = new ArrayList<>(lines.size());
        for (TextLine line : lines) {
            comparable.add(line.content() + '\u0000' + line.eol());
        }
        return comparable;
    }

    private static String joinLines(List<TextLine> lines) {
        StringBuilder builder = new StringBuilder();
        for (TextLine line : lines) {
            builder.append(line.content());
            builder.append(line.eol());
        }
        return builder.toString();
    }

    private static List<TextLine> normalizeLineEndings(List<TextLine> lines, String preferredEol) {
        List<TextLine> normalized = new ArrayList<>(lines.size());
        for (TextLine line : lines) {
            String eol = line.eol().isEmpty() ? "" : preferredEol;
            normalized.add(new TextLine(line.content(), eol));
        }
        return normalized;
    }

    private static String detectPreferredEol(String text) {
        int crlf = 0;
        int lf = 0;
        int cr = 0;
        for (int i = 0; i < text.length(); i++) {
            char value = text.charAt(i);
            if (value == '\r') {
                if (i + 1 < text.length() && text.charAt(i + 1) == '\n') {
                    crlf++;
                    i++;
                } else {
                    cr++;
                }
            } else if (value == '\n') {
                lf++;
            }
        }
        if (crlf >= lf && crlf >= cr && crlf > 0) {
            return "\r\n";
        }
        if (lf >= cr && lf > 0) {
            return "\n";
        }
        if (cr > 0) {
            return "\r";
        }
        return DEFAULT_EOL;
    }

    private static boolean hasTerminalEol(String text) {
        if (text.isEmpty()) {
            return false;
        }
        char last = text.charAt(text.length() - 1);
        return last == '\n' || last == '\r';
    }

    private static DiffComputation computeDiffChunks(List<String> left, List<String> right) {
        if (left.equals(right)) {
            return new DiffComputation(List.of(), false);
        }

        long matrixCells = (long) left.size() * (long) right.size();
        if (matrixCells > MAX_LCS_MATRIX_CELLS) {
            return new DiffComputation(
                List.of(new DiffChunk(0, left.size(), 0, right.size())),
                true
            );
        }

        List<EditStep> steps = computeEditScript(left, right);
        List<DiffChunk> chunks = new ArrayList<>();
        int leftLine = 0;
        int rightLine = 0;

        boolean inChunk = false;
        int chunkLeftStart = 0;
        int chunkRightStart = 0;

        for (EditStep step : steps) {
            if (step == EditStep.EQUAL) {
                if (inChunk) {
                    chunks.add(new DiffChunk(chunkLeftStart, leftLine, chunkRightStart, rightLine));
                    inChunk = false;
                }
                leftLine++;
                rightLine++;
                continue;
            }
            if (!inChunk) {
                inChunk = true;
                chunkLeftStart = leftLine;
                chunkRightStart = rightLine;
            }
            if (step == EditStep.DELETE) {
                leftLine++;
            } else {
                rightLine++;
            }
        }

        if (inChunk) {
            chunks.add(new DiffChunk(chunkLeftStart, leftLine, chunkRightStart, rightLine));
        }
        return new DiffComputation(chunks, false);
    }

    private static List<EditStep> computeEditScript(List<String> left, List<String> right) {
        int n = left.size();
        int m = right.size();
        int max = n + m;
        int offset = max;

        int[] v = new int[2 * max + 1];
        for (int i = 0; i < v.length; i++) {
            v[i] = -1;
        }
        v[offset + 1] = 0;

        List<int[]> trace = new ArrayList<>(max + 1);
        int endD = -1;

        for (int d = 0; d <= max; d++) {
            for (int k = -d; k <= d; k += 2) {
                int x;
                if (k == -d || (k != d && v[offset + k - 1] < v[offset + k + 1])) {
                    x = v[offset + k + 1];
                } else {
                    x = v[offset + k - 1] + 1;
                }
                int y = x - k;
                while (x < n && y < m && left.get(x).equals(right.get(y))) {
                    x++;
                    y++;
                }
                v[offset + k] = x;
                if (x >= n && y >= m) {
                    endD = d;
                    break;
                }
            }
            trace.add(v.clone());
            if (endD >= 0) {
                break;
            }
        }

        if (endD < 0) {
            return List.of();
        }

        List<EditStep> reversed = new ArrayList<>(n + m + 1);
        int x = n;
        int y = m;

        for (int d = endD; d > 0; d--) {
            int[] previous = trace.get(d - 1);
            int k = x - y;
            int prevK;
            if (k == -d || (k != d && previous[offset + k - 1] < previous[offset + k + 1])) {
                prevK = k + 1;
            } else {
                prevK = k - 1;
            }

            int prevX = previous[offset + prevK];
            int prevY = prevX - prevK;

            while (x > prevX && y > prevY) {
                reversed.add(EditStep.EQUAL);
                x--;
                y--;
            }

            if (x == prevX) {
                reversed.add(EditStep.INSERT);
                y--;
            } else {
                reversed.add(EditStep.DELETE);
                x--;
            }
        }

        while (x > 0 && y > 0) {
            reversed.add(EditStep.EQUAL);
            x--;
            y--;
        }
        while (x > 0) {
            reversed.add(EditStep.DELETE);
            x--;
        }
        while (y > 0) {
            reversed.add(EditStep.INSERT);
            y--;
        }

        Collections.reverse(reversed);
        return reversed;
    }

    private enum EditStep {
        EQUAL,
        INSERT,
        DELETE
    }

    private record TextLine(String content, String eol) {
    }

    private record DiffComputation(List<DiffChunk> chunks, boolean usedGuardedFallback) {
    }
}
