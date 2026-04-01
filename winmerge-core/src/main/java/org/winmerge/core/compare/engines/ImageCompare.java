package org.winmerge.core.compare.engines;

import org.winmerge.core.compare.DiffCode;

import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.IOException;

public final class ImageCompare implements CompareEngine {
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
        return CompareEngineResult.withUnknownCounts(DiffCode.FILE | DiffCode.IMAGE | result);
    }

    private int comparePair(CompareEngineContext context, int left, int right) {
        if (!context.exists(left) && !context.exists(right)) {
            return DiffCode.SAME;
        }
        if (!context.exists(left) || !context.exists(right)) {
            return DiffCode.DIFF;
        }
        try {
            BufferedImage leftImage = ImageIO.read(context.getPath(left).toFile());
            BufferedImage rightImage = ImageIO.read(context.getPath(right).toFile());
            if (leftImage == null || rightImage == null) {
                return DiffCode.CMPERR;
            }
            if (leftImage.getWidth() != rightImage.getWidth()
                    || leftImage.getHeight() != rightImage.getHeight()) {
                return DiffCode.DIFF;
            }
            double threshold = context.getColorDistanceThreshold();
            for (int y = 0; y < leftImage.getHeight(); y++) {
                if (context.getAbortChecker().shouldAbort()) {
                    return DiffCode.CMPABORT;
                }
                for (int x = 0; x < leftImage.getWidth(); x++) {
                    if (colorDistance(leftImage.getRGB(x, y), rightImage.getRGB(x, y)) > threshold) {
                        return DiffCode.DIFF;
                    }
                }
            }
            return DiffCode.SAME;
        } catch (IOException ex) {
            return DiffCode.CMPERR;
        }
    }

    private static double colorDistance(int leftRgb, int rightRgb) {
        int leftR = (leftRgb >> 16) & 0xFF;
        int leftG = (leftRgb >> 8) & 0xFF;
        int leftB = leftRgb & 0xFF;
        int rightR = (rightRgb >> 16) & 0xFF;
        int rightG = (rightRgb >> 8) & 0xFF;
        int rightB = rightRgb & 0xFF;
        int dr = leftR - rightR;
        int dg = leftG - rightG;
        int db = leftB - rightB;
        return Math.sqrt((dr * dr) + (dg * dg) + (db * db));
    }
}
