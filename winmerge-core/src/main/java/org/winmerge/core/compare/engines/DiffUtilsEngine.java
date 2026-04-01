package org.winmerge.core.compare.engines;

import org.winmerge.core.compare.DiffCode;
import org.winmerge.core.compare.QuickCompareOptions;

public final class DiffUtilsEngine implements CompareEngine {
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

        int significantDiffs = (compareBits & DiffCode.COMPAREFLAGS) == DiffCode.DIFF ? 1 : 0;
        int trivialDiffs = 0;
        if ((compareBits & DiffCode.COMPAREFLAGS) == DiffCode.CMPERR
                || (compareBits & DiffCode.COMPAREFLAGS) == DiffCode.CMPABORT) {
            significantDiffs = CompareEngineResult.DIFFS_UNKNOWN;
            trivialDiffs = CompareEngineResult.DIFFS_UNKNOWN;
        }
        return new CompareEngineResult(diffCode, significantDiffs, trivialDiffs);
    }
}
