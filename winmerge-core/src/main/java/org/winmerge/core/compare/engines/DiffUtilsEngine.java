package org.winmerge.core.compare.engines;

import org.winmerge.core.compare.DiffCode;
import org.winmerge.core.compare.QuickCompareOptions;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

public final class DiffUtilsEngine implements CompareEngine {
    private static final DiffCounts UNKNOWN_COUNTS =
            new DiffCounts(CompareEngineResult.DIFFS_UNKNOWN, CompareEngineResult.DIFFS_UNKNOWN);
    private static final long MAX_LCS_MATRIX_CELLS = 8_000_000L;

    @Override
    public CompareEngineResult compare(CompareEngineContext context) {
        QuickCompareOptions options = new QuickCompareOptions(context.getDiffutilsOptions());
        options.setStopAfterFirstDiff(false);
        context.setQuickCompareOptions(options);
        context.setStopAfterFirstDiff(false);

        ByteComparator comparator = new ByteComparator();
        CompareEngineResult quickResult = comparator.compare(context);
        int compareBits = quickResult.diffCode() & (DiffCode.COMPAREFLAGS | DiffCode.COMPAREFLAGS3WAY);
        int textBits = quickResult.diffCode() & DiffCode.TEXTFLAGS;
        int diffCode = DiffCode.FILE | textBits | compareBits;

        int compareCode = compareBits & DiffCode.COMPAREFLAGS;
        if (compareCode == DiffCode.CMPERR || compareCode == DiffCode.CMPABORT) {
            return new CompareEngineResult(
                    diffCode,
                    CompareEngineResult.DIFFS_UNKNOWN,
                    CompareEngineResult.DIFFS_UNKNOWN);
        }
        if ((textBits & DiffCode.BIN) != 0 || !allComparedSidesExist(context)) {
            return new CompareEngineResult(
                    diffCode,
                    CompareEngineResult.DIFFS_UNKNOWN,
                    CompareEngineResult.DIFFS_UNKNOWN);
        }

        DiffCounts counts = context.getFileCount() == 2
                ? countPairDiffs(context, 0, 1)
                : countThreeWayDiffs(context, compareBits);
        return new CompareEngineResult(diffCode, counts.significant(), counts.trivial());
    }

    private static DiffCounts countThreeWayDiffs(CompareEngineContext context, int compareBits) {
        DiffCounts pair10 = countPairDiffs(context, 1, 0);
        DiffCounts pair12 = countPairDiffs(context, 1, 2);
        DiffCounts pair02 = countPairDiffs(context, 0, 2);
        int threeWayFlag = compareBits & DiffCode.COMPAREFLAGS3WAY;

        return switch (threeWayFlag) {
            case DiffCode.DIFF1STONLY -> pair10;
            case DiffCode.DIFF2NDONLY -> maxCounts(pair10, pair12);
            case DiffCode.DIFF3RDONLY -> pair12;
            default -> maxCounts(pair10, pair12, pair02);
        };
    }

    private static DiffCounts countPairDiffs(CompareEngineContext context, int leftIndex, int rightIndex) {
        try {
            String leftText = readText(context, leftIndex);
            String rightText = readText(context, rightIndex);
            int rawHunks = countDiffHunks(splitLines(leftText), splitLines(rightText));
            int significant = countDiffHunks(
                    splitLines(CompareEngineSupport.normalizeTextForComparison(leftText, context)),
                    splitLines(CompareEngineSupport.normalizeTextForComparison(rightText, context)));
            if (rawHunks < 0 || significant < 0) {
                return UNKNOWN_COUNTS;
            }
            return new DiffCounts(significant, Math.max(0, rawHunks - significant));
        } catch (IOException ex) {
            return UNKNOWN_COUNTS;
        }
    }

    private static DiffCounts maxCounts(DiffCounts... values) {
        int significant = Integer.MIN_VALUE;
        int trivial = Integer.MIN_VALUE;
        for (DiffCounts value : values) {
            if (value.significant() < 0 || value.trivial() < 0) {
                return UNKNOWN_COUNTS;
            }
            significant = Math.max(significant, value.significant());
            trivial = Math.max(trivial, value.trivial());
        }
        if (significant == Integer.MIN_VALUE) {
            return new DiffCounts(0, 0);
        }
        return new DiffCounts(significant, trivial);
    }

    private static String readText(CompareEngineContext context, int index) throws IOException {
        byte[] bytes = context.getFileSystem().readAllBytes(context.getPath(index));
        CompareFileInfo info = context.getFileInfo(index);
        info.setEncoding(CompareEngineSupport.detectEncoding(bytes));
        return CompareEngineSupport.decodeUsingEncoding(bytes, info.encoding());
    }

    private static String[] splitLines(String text) {
        return text.split("\\R", -1);
    }

    private static int countDiffHunks(String[] left, String[] right) {
        if (Arrays.equals(left, right)) {
            return 0;
        }
        long matrixCells = (long) left.length * (long) right.length;
        if (matrixCells > MAX_LCS_MATRIX_CELLS) {
            return CompareEngineResult.DIFFS_UNKNOWN;
        }

        int n = left.length;
        int m = right.length;
        int max = n + m;
        int offset = max;
        int[] v = new int[2 * max + 1];
        Arrays.fill(v, -1);
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
                while (x < n && y < m && left[x].equals(right[y])) {
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
            return CompareEngineResult.DIFFS_UNKNOWN;
        }
        return countChangeRunsFromTrace(trace, endD, n, m, offset);
    }

    private static int countChangeRunsFromTrace(
            List<int[]> trace,
            int endD,
            int n,
            int m,
            int offset
    ) {
        List<Boolean> changedSteps = new ArrayList<>(n + m);
        int x = n;
        int y = m;

        for (int d = endD; d > 0; d--) {
            int[] prevV = trace.get(d - 1);
            int k = x - y;
            int prevK;
            if (k == -d || (k != d && prevV[offset + k - 1] < prevV[offset + k + 1])) {
                prevK = k + 1;
            } else {
                prevK = k - 1;
            }
            int prevX = prevV[offset + prevK];
            int prevY = prevX - prevK;

            while (x > prevX && y > prevY) {
                changedSteps.add(Boolean.FALSE);
                x--;
                y--;
            }
            changedSteps.add(Boolean.TRUE);
            if (x == prevX) {
                y--;
            } else {
                x--;
            }
        }

        while (x > 0 && y > 0) {
            changedSteps.add(Boolean.FALSE);
            x--;
            y--;
        }
        while (x > 0) {
            changedSteps.add(Boolean.TRUE);
            x--;
        }
        while (y > 0) {
            changedSteps.add(Boolean.TRUE);
            y--;
        }

        Collections.reverse(changedSteps);
        int hunks = 0;
        boolean inChange = false;
        for (boolean changed : changedSteps) {
            if (changed) {
                if (!inChange) {
                    hunks++;
                    inChange = true;
                }
            } else {
                inChange = false;
            }
        }
        return hunks;
    }

    private static boolean allComparedSidesExist(CompareEngineContext context) {
        for (int i = 0; i < context.getFileCount(); i++) {
            if (!context.exists(i)) {
                return false;
            }
        }
        return true;
    }

    private record DiffCounts(int significant, int trivial) {
    }
}
