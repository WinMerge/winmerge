package org.winmerge.core.compare.engines;

import org.winmerge.core.compare.DiffCode;
import org.winmerge.core.diff.DiffEngine;

public final class TimeSizeCompare implements CompareEngine {
    private static final long SMALL_TIME_DIFF_MILLIS = 2_000L;

    @Override
    public CompareEngineResult compare(CompareEngineContext context) {
        context.refreshFileInfos();
        int files = context.getFileCount();
        DiffEngine.CompareType compareType = context.getCompareType();

        int code = DiffCode.SAME;
        long timeDiff01 = 0L;
        long timeDiff12 = 0L;
        long timeDiff02 = 0L;

        if (compareType == DiffEngine.CompareType.DATE || compareType == DiffEngine.CompareType.DATE_SIZE) {
            timeDiff01 = absoluteRoundedDiff(
                    context.getFileInfo(0).modifiedTimeMillis(),
                    context.getFileInfo(1).modifiedTimeMillis());
            if (files > 2) {
                timeDiff12 = absoluteRoundedDiff(
                        context.getFileInfo(1).modifiedTimeMillis(),
                        context.getFileInfo(2).modifiedTimeMillis());
                timeDiff02 = absoluteRoundedDiff(
                        context.getFileInfo(0).modifiedTimeMillis(),
                        context.getFileInfo(2).modifiedTimeMillis());
            }
            if (context.isIgnoreSmallTimeDiff()) {
                timeDiff01 -= SMALL_TIME_DIFF_MILLIS;
                timeDiff12 -= SMALL_TIME_DIFF_MILLIS;
                timeDiff02 -= SMALL_TIME_DIFF_MILLIS;
            }
            code = (timeDiff01 <= 0 && timeDiff12 <= 0) ? DiffCode.SAME : DiffCode.DIFF;

            for (int i = 0; i < files; i++) {
                if (context.exists(i)
                        && context.getFileInfo(i).modifiedTimeMillis() == CompareFileInfo.TIME_VALUE_NONE) {
                    code = DiffCode.CMPERR;
                }
            }
        }

        if (compareType == DiffEngine.CompareType.DATE_SIZE || compareType == DiffEngine.CompareType.SIZE) {
            if (context.getFileInfo(0).size() != context.getFileInfo(1).size()
                    || (files > 2 && context.getFileInfo(1).size() != context.getFileInfo(2).size())) {
                code = DiffCode.DIFF;
            }

            for (int i = 0; i < files; i++) {
                if (context.exists(i)
                        && context.getFileInfo(i).size() == CompareFileInfo.FILE_SIZE_NONE) {
                    code = DiffCode.CMPERR;
                }
            }
        }

        if (files > 2 && (code & DiffCode.COMPAREFLAGS) == DiffCode.DIFF) {
            if (compareType == DiffEngine.CompareType.DATE) {
                if (timeDiff12 <= 0) {
                    code |= DiffCode.DIFF1STONLY;
                } else if (timeDiff02 <= 0) {
                    code |= DiffCode.DIFF2NDONLY;
                } else if (timeDiff01 <= 0) {
                    code |= DiffCode.DIFF3RDONLY;
                }
            } else if (compareType == DiffEngine.CompareType.DATE_SIZE) {
                if (timeDiff12 <= 0 && context.getFileInfo(1).size() == context.getFileInfo(2).size()) {
                    code |= DiffCode.DIFF1STONLY;
                } else if (timeDiff02 <= 0 && context.getFileInfo(0).size() == context.getFileInfo(2).size()) {
                    code |= DiffCode.DIFF2NDONLY;
                } else if (timeDiff01 <= 0 && context.getFileInfo(0).size() == context.getFileInfo(1).size()) {
                    code |= DiffCode.DIFF3RDONLY;
                }
            } else if (compareType == DiffEngine.CompareType.SIZE) {
                if (context.getFileInfo(1).size() == context.getFileInfo(2).size()) {
                    code |= DiffCode.DIFF1STONLY;
                } else if (context.getFileInfo(0).size() == context.getFileInfo(2).size()) {
                    code |= DiffCode.DIFF2NDONLY;
                } else if (context.getFileInfo(0).size() == context.getFileInfo(1).size()) {
                    code |= DiffCode.DIFF3RDONLY;
                }
            }
        }

        return CompareEngineResult.withUnknownCounts(DiffCode.FILE | code);
    }

    private static long absoluteRoundedDiff(long leftMillis, long rightMillis) {
        if (leftMillis == CompareFileInfo.TIME_VALUE_NONE || rightMillis == CompareFileInfo.TIME_VALUE_NONE) {
            return Long.MAX_VALUE;
        }
        long roundedLeft = (leftMillis / 1000L) * 1000L;
        long roundedRight = (rightMillis / 1000L) * 1000L;
        return Math.abs(roundedLeft - roundedRight);
    }
}
