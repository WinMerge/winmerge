package org.winmerge.core.compare.engines;

import org.winmerge.core.compare.DiffCode;

public final class ExistenceCompare implements CompareEngine {
    @Override
    public CompareEngineResult compare(CompareEngineContext context) {
        context.refreshFileInfos();
        int files = context.getFileCount();
        int code = DiffCode.SAME;
        if (context.exists(0) != context.exists(1)
                || (files > 2 && context.exists(0) != context.exists(2))) {
            code = DiffCode.DIFF;
        }

        if (files > 2 && (code & DiffCode.COMPAREFLAGS) == DiffCode.DIFF) {
            if (context.exists(1) == context.exists(2)) {
                code |= DiffCode.DIFF1STONLY;
            } else if (context.exists(0) == context.exists(2)) {
                code |= DiffCode.DIFF2NDONLY;
            } else if (context.exists(0) == context.exists(1)) {
                code |= DiffCode.DIFF3RDONLY;
            }
        }

        return CompareEngineResult.withUnknownCounts(DiffCode.FILE | code);
    }
}
