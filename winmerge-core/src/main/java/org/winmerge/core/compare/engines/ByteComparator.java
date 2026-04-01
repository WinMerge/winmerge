package org.winmerge.core.compare.engines;

import org.winmerge.core.compare.DiffCode;
import org.winmerge.core.io.FileTextEncoding;
import org.winmerge.core.io.FileTextStats;

import java.io.IOException;
import java.nio.file.Path;

public class ByteComparator implements CompareEngine {
    @Override
    public CompareEngineResult compare(CompareEngineContext context) {
        context.refreshFileInfos();
        int files = context.getFileCount();
        if (files < 2) {
            return CompareEngineResult.withQuickUnknownCounts(DiffCode.FILE | DiffCode.CMPERR);
        }

        if (files == 2) {
            PairResult pair = comparePair(context, 0, 1);
            int result = DiffCode.FILE | pair.sideFlags() | pair.compareCode();
            return CompareEngineResult.withQuickUnknownCounts(result);
        }

        PairResult pair10 = comparePair(context, 1, 0);
        PairResult pair12 = comparePair(context, 1, 2);
        PairResult pair02 = comparePair(context, 0, 2);

        int compareCode = CompareEngineSupport.resolveThreeWay(
                pair10::compareCode,
                pair12::compareCode,
                pair02::compareCode);
        int sideFlags = mergeThreeWaySideFlags(pair10.sideFlags(), pair12.sideFlags());
        int result = DiffCode.FILE | sideFlags | compareCode;
        return CompareEngineResult.withQuickUnknownCounts(result);
    }

    protected PairResult comparePair(CompareEngineContext context, int left, int right) {
        CompareFileInfo leftInfo = context.getFileInfo(left);
        CompareFileInfo rightInfo = context.getFileInfo(right);
        if (!leftInfo.exists() && !rightInfo.exists()) {
            return new PairResult(DiffCode.SAME, DiffCode.TEXT);
        }
        if (!leftInfo.exists() || !rightInfo.exists()) {
            return new PairResult(DiffCode.DIFF, DiffCode.TEXT);
        }

        Path leftPath = context.getPath(left);
        Path rightPath = context.getPath(right);
        try {
            byte[] leftBytes = context.getFileSystem().readAllBytes(leftPath);
            byte[] rightBytes = context.getFileSystem().readAllBytes(rightPath);
            FileTextEncoding leftEncoding = CompareEngineSupport.detectEncoding(leftBytes);
            FileTextEncoding rightEncoding = CompareEngineSupport.detectEncoding(rightBytes);
            leftInfo.setEncoding(leftEncoding);
            rightInfo.setEncoding(rightEncoding);

            FileTextStats leftStats = new FileTextStats();
            FileTextStats rightStats = new FileTextStats();
            CompareEngineSupport.collectTextStats(leftStats, leftBytes);
            CompareEngineSupport.collectTextStats(rightStats, rightBytes);
            copyTextStats(leftStats, context.getTextStats(left));
            copyTextStats(rightStats, context.getTextStats(right));

            boolean binaryInput = leftStats.getZeroCount() > 0 || rightStats.getZeroCount() > 0;
            int compareCode = binaryInput
                    ? compareBinary(leftBytes, rightBytes, context)
                    : compareText(leftBytes, rightBytes, context);
            int sideFlags = CompareEngineSupport.binarySideFlagsFromStats(leftStats, rightStats);
            return new PairResult(compareCode, sideFlags);
        } catch (IOException ex) {
            return new PairResult(DiffCode.CMPERR, DiffCode.TEXT);
        }
    }

    protected int compareBinary(byte[] leftBytes, byte[] rightBytes, CompareEngineContext context) {
        int maxLength = Math.max(leftBytes.length, rightBytes.length);
        int minLength = Math.min(leftBytes.length, rightBytes.length);
        for (int i = 0; i < minLength; i++) {
            if ((i % 4096) == 0 && context.getAbortChecker().shouldAbort()) {
                return DiffCode.CMPABORT;
            }
            if (leftBytes[i] != rightBytes[i]) {
                return DiffCode.DIFF;
            }
        }
        if (maxLength != minLength) {
            return DiffCode.DIFF;
        }
        return DiffCode.SAME;
    }

    protected int compareText(byte[] leftBytes, byte[] rightBytes, CompareEngineContext context) {
        int result = CompareEngineSupport.compareTextWithOptions(leftBytes, rightBytes, context);
        if (result == DiffCode.DIFF && context.isStopAfterFirstDiff()) {
            return DiffCode.DIFF;
        }
        return result;
    }

    private static int mergeThreeWaySideFlags(int side10, int side12) {
        int sideFlags = DiffCode.TEXT;
        boolean hasBinary = (side10 & DiffCode.BIN) != 0 || (side12 & DiffCode.BIN) != 0;
        if (!hasBinary) {
            return sideFlags;
        }
        sideFlags = DiffCode.BIN;
        if ((side10 & DiffCode.BINSIDE1) != 0) {
            sideFlags |= DiffCode.BINSIDE2;
        }
        if ((side10 & DiffCode.BINSIDE2) != 0) {
            sideFlags |= DiffCode.BINSIDE1;
        }
        if ((side12 & DiffCode.BINSIDE2) != 0) {
            sideFlags |= DiffCode.BINSIDE3;
        }
        return sideFlags;
    }

    private static void copyTextStats(FileTextStats source, FileTextStats destination) {
        destination.setCrCount(source.getCrCount());
        destination.setLfCount(source.getLfCount());
        destination.setCrlfCount(source.getCrlfCount());
        destination.setZeroCount(source.getZeroCount());
    }

    protected record PairResult(int compareCode, int sideFlags) {
    }
}
