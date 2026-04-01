package org.winmerge.core.compare.engines;

import org.winmerge.core.compare.DiffCode;

public final class BinaryCompare implements CompareEngine {
    @Override
    public CompareEngineResult compare(CompareEngineContext context) {
        context.refreshFileInfos();
        int files = context.getFileCount();
        int result;
        if (files == 2) {
            result = comparePair(context, 0, 1);
        } else {
            result = CompareEngineSupport.resolveThreeWay(
                    () -> comparePair(context, 1, 0),
                    () -> comparePair(context, 1, 2),
                    () -> comparePair(context, 0, 2));
        }
        return CompareEngineResult.withUnknownCounts(DiffCode.FILE | result);
    }

    private int comparePair(CompareEngineContext context, int left, int right) {
        CompareFileInfo leftInfo = context.getFileInfo(left);
        CompareFileInfo rightInfo = context.getFileInfo(right);
        if (!leftInfo.exists() && !rightInfo.exists()) {
            return DiffCode.SAME;
        }
        if (!leftInfo.exists() || !rightInfo.exists()) {
            return DiffCode.DIFF;
        }
        if ((leftInfo.size() != rightInfo.size()) && leftInfo.size() != 0 && rightInfo.size() != 0) {
            return DiffCode.DIFF;
        }
        return CompareEngineSupport.compareBinaryStreams(
                context.getPath(left),
                context.getPath(right),
                context);
    }
}
