package org.winmerge.core.compare.engines;

import org.winmerge.core.compare.DiffCode;
import org.winmerge.core.diff.DiffEngine;
import org.winmerge.core.io.FileTextEncoding;

public final class FullQuickCompare implements CompareEngine {
    @Override
    public CompareEngineResult compare(CompareEngineContext context) {
        DiffEngine.CompareType compareType = context.getCompareType();
        CompareEngineResult result;
        if (compareType == DiffEngine.CompareType.CONTENT) {
            result = new DiffUtilsEngine().compare(context);
        } else if (compareType == DiffEngine.CompareType.QUICK_CONTENT) {
            result = new ByteCompare().compare(context);
        } else {
            return CompareEngineResult.withUnknownCounts(DiffCode.FILE | DiffCode.CMPERR);
        }

        if (!allComparedSidesExist(context) && (result.diffCode() & DiffCode.COMPAREFLAGS) == DiffCode.SAME) {
            int diffCode = (result.diffCode() & ~DiffCode.COMPAREFLAGS) | DiffCode.DIFF;
            return new CompareEngineResult(diffCode, result.significantDiffs(), result.trivialDiffs());
        }
        if (!context.isIgnoreCodepage()
                && (result.diffCode() & DiffCode.COMPAREFLAGS) == DiffCode.SAME
                && !allComparedSidesUseSameEncoding(context)) {
            int diffCode = (result.diffCode() & ~DiffCode.COMPAREFLAGS) | DiffCode.DIFF;
            return new CompareEngineResult(diffCode, result.significantDiffs(), result.trivialDiffs());
        }
        return result;
    }

    private static boolean allComparedSidesExist(CompareEngineContext context) {
        for (int i = 0; i < context.getFileCount(); i++) {
            if (!context.exists(i)) {
                return false;
            }
        }
        return true;
    }

    private static boolean allComparedSidesUseSameEncoding(CompareEngineContext context) {
        FileTextEncoding baseline = null;
        for (int i = 0; i < context.getFileCount(); i++) {
            if (!context.exists(i)) {
                continue;
            }
            FileTextEncoding encoding = context.getFileInfo(i).encoding();
            if (baseline == null) {
                baseline = encoding;
                continue;
            }
            if (!baseline.equals(encoding)) {
                return false;
            }
        }
        return true;
    }
}
