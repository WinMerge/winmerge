package org.winmerge.core.compare.engines;

public record CompareEngineResult(int diffCode, int significantDiffs, int trivialDiffs) {
    public static final int DIFFS_UNKNOWN = -1;
    public static final int DIFFS_UNKNOWN_QUICKCOMPARE = -2;

    public static CompareEngineResult withUnknownCounts(int diffCode) {
        return new CompareEngineResult(diffCode, DIFFS_UNKNOWN, DIFFS_UNKNOWN);
    }

    public static CompareEngineResult withQuickUnknownCounts(int diffCode) {
        return new CompareEngineResult(diffCode, DIFFS_UNKNOWN_QUICKCOMPARE, DIFFS_UNKNOWN_QUICKCOMPARE);
    }
}
