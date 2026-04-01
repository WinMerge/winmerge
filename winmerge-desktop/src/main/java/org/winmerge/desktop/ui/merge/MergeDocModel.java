package org.winmerge.desktop.ui.merge;

import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Objects;

public final class MergeDocModel {
    private final Path leftPath;
    private final Path rightPath;
    private final String leftText;
    private final String rightText;
    private final List<String> leftLines;
    private final List<String> rightLines;
    private final List<DiffChunk> diffChunks;

    private MergeDocModel(
        Path leftPath,
        Path rightPath,
        String leftText,
        String rightText,
        List<String> leftLines,
        List<String> rightLines,
        List<DiffChunk> diffChunks
    ) {
        this.leftPath = leftPath;
        this.rightPath = rightPath;
        this.leftText = leftText;
        this.rightText = rightText;
        this.leftLines = leftLines;
        this.rightLines = rightLines;
        this.diffChunks = diffChunks;
    }

    public static MergeDocModel load(Path leftPath, Path rightPath) throws IOException {
        Objects.requireNonNull(leftPath, "leftPath");
        Objects.requireNonNull(rightPath, "rightPath");
        return fromTexts(
            leftPath,
            rightPath,
            Files.readString(leftPath, StandardCharsets.UTF_8),
            Files.readString(rightPath, StandardCharsets.UTF_8)
        );
    }

    public static MergeDocModel fromTexts(Path leftPath, Path rightPath, String leftText, String rightText) {
        Objects.requireNonNull(leftPath, "leftPath");
        Objects.requireNonNull(rightPath, "rightPath");
        Objects.requireNonNull(leftText, "leftText");
        Objects.requireNonNull(rightText, "rightText");

        List<String> leftLines = splitLines(leftText);
        List<String> rightLines = splitLines(rightText);
        List<DiffChunk> chunks = computeDiffChunks(leftLines, rightLines);

        return new MergeDocModel(
            leftPath,
            rightPath,
            leftText,
            rightText,
            Collections.unmodifiableList(leftLines),
            Collections.unmodifiableList(rightLines),
            Collections.unmodifiableList(chunks)
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
        List<String> mergedRightLines = new ArrayList<>(rightLines);
        int rightStart = chunk.rightStartLine();
        int rightEnd = chunk.rightEndLine();
        mergedRightLines.subList(rightStart, rightEnd).clear();
        mergedRightLines.addAll(
            rightStart,
            leftLines.subList(chunk.leftStartLine(), chunk.leftEndLine())
        );

        return fromTexts(leftPath, rightPath, leftText, joinLines(mergedRightLines));
    }

    private static List<String> splitLines(String text) {
        String normalized = text.replace("\r\n", "\n").replace('\r', '\n');
        return new ArrayList<>(List.of(normalized.split("\n", -1)));
    }

    private static String joinLines(List<String> lines) {
        return String.join("\n", lines);
    }

    private static List<DiffChunk> computeDiffChunks(List<String> left, List<String> right) {
        if (left.equals(right)) {
            return List.of();
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
        return chunks;
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
}
