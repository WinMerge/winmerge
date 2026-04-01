package org.winmerge.core.compare.engines;

import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.io.TempDir;
import org.winmerge.core.compare.DiffCode;
import org.winmerge.core.compare.QuickCompareOptions;
import org.winmerge.core.compare.WhitespaceIgnoreChoice;
import org.winmerge.core.diff.DiffEngine;
import org.winmerge.core.io.NioFileSystemService;
import org.winmerge.core.io.PathContext;

import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

class CompareEnginesTest {
    @TempDir
    Path tempDir;

    @Test
    void existenceCompareDetectsMissingSideInThreeWay() throws IOException {
        Path left = write("left.txt", "same");
        Path middle = write("middle.txt", "same");
        Path missing = tempDir.resolve("missing.txt");

        CompareEngineContext context = context(left, middle, missing);
        CompareEngineResult result = new ExistenceCompare().compare(context);

        assertEquals(DiffCode.FILE | DiffCode.DIFF | DiffCode.DIFF3RDONLY, result.diffCode());
    }

    @Test
    void timeSizeCompareSupportsSizeMode() throws IOException {
        Path left = write("left.txt", "ab");
        Path right = write("right.txt", "ab");

        CompareEngineContext context = context(left, right);
        context.setCompareType(DiffEngine.CompareType.SIZE);
        CompareEngineResult result = new TimeSizeCompare().compare(context);
        assertEquals(DiffCode.FILE | DiffCode.SAME, result.diffCode());

        Files.writeString(right, "abcd");
        context.refreshFileInfos();
        result = new TimeSizeCompare().compare(context);
        assertEquals(DiffCode.FILE | DiffCode.DIFF, result.diffCode());
    }

    @Test
    void binaryCompareProducesSecondOnlyFlagForThreeWay() throws IOException {
        Path left = write("left.bin", "AAAA");
        Path middle = write("middle.bin", "BBBB");
        Path right = write("right.bin", "AAAA");

        CompareEngineResult result = new BinaryCompare().compare(context(left, middle, right));
        assertEquals(DiffCode.FILE | DiffCode.DIFF | DiffCode.DIFF2NDONLY, result.diffCode());
    }

    @Test
    void imageCompareFindsPixelDifference() throws IOException {
        Path left = tempDir.resolve("left.png");
        Path right = tempDir.resolve("right.png");

        BufferedImage imageA = new BufferedImage(2, 2, BufferedImage.TYPE_INT_RGB);
        imageA.setRGB(0, 0, 0x00FF00);
        imageA.setRGB(1, 1, 0x0000FF);
        BufferedImage imageB = new BufferedImage(2, 2, BufferedImage.TYPE_INT_RGB);
        imageB.setRGB(0, 0, 0x00FF00);
        imageB.setRGB(1, 1, 0xFF0000);
        ImageIO.write(imageA, "png", left.toFile());
        ImageIO.write(imageB, "png", right.toFile());

        CompareEngineContext context = context(left, right);
        context.setColorDistanceThreshold(0.0d);
        CompareEngineResult result = new ImageCompare().compare(context);
        assertEquals(DiffCode.FILE | DiffCode.IMAGE | DiffCode.DIFF, result.diffCode());
    }

    @Test
    void byteComparatorHonorsIgnoreCaseOption() throws IOException {
        Path left = write("left.txt", "AbC\n");
        Path right = write("right.txt", "aBc\n");
        CompareEngineContext context = context(left, right);
        QuickCompareOptions options = new QuickCompareOptions();
        options.setIgnoreCase(true);
        context.setQuickCompareOptions(options);

        CompareEngineResult result = new ByteComparator().compare(context);
        assertEquals(DiffCode.FILE | DiffCode.TEXT | DiffCode.SAME, result.diffCode());
    }

    @Test
    void byteCompareDetectsBinaryAndDiff() throws IOException {
        Path left = tempDir.resolve("left.bin");
        Path right = tempDir.resolve("right.bin");
        Files.write(left, new byte[] {0, 1, 2, 3});
        Files.write(right, new byte[] {0, 1, 2, 4});

        CompareEngineResult result = new ByteCompare().compare(context(left, right));
        assertTrue((result.diffCode() & DiffCode.BIN) != 0);
        assertEquals(DiffCode.DIFF, result.diffCode() & DiffCode.COMPAREFLAGS);
    }

    @Test
    void diffUtilsEngineReturnsKnownDiffCountsForTextDiff() throws IOException {
        Path left = write("left.txt", "alpha\nbeta\ngamma\ndelta\n");
        Path right = write("right.txt", "alpha\nBETA\ngamma\nDELTA\n");

        CompareEngineContext context = context(left, right);
        CompareEngineResult result = new DiffUtilsEngine().compare(context);
        assertEquals(DiffCode.FILE | DiffCode.TEXT | DiffCode.DIFF, result.diffCode());
        assertEquals(2, result.significantDiffs());
        assertEquals(0, result.trivialDiffs());
    }

    @Test
    void ignoreChangeTreatsTrailingWhitespaceBeforeEolAsSame() throws IOException {
        Path left = write("left.txt", "x \n");
        Path right = write("right.txt", "x\n");

        CompareEngineContext context = context(left, right);
        QuickCompareOptions options = new QuickCompareOptions();
        options.setIgnoreWhitespace(WhitespaceIgnoreChoice.IGNORE_CHANGE);
        context.setQuickCompareOptions(options);

        CompareEngineResult result = new ByteComparator().compare(context);
        assertEquals(DiffCode.FILE | DiffCode.TEXT | DiffCode.SAME, result.diffCode());
    }

    @Test
    void fullQuickCompareRoutesByCompareType() throws IOException {
        Path left = write("left.txt", "same");
        Path right = write("right.txt", "same");
        CompareEngineContext context = context(left, right);

        context.setCompareType(DiffEngine.CompareType.QUICK_CONTENT);
        CompareEngineResult quick = new FullQuickCompare().compare(context);
        assertEquals(DiffCode.SAME, quick.diffCode() & DiffCode.COMPAREFLAGS);

        context.setCompareType(DiffEngine.CompareType.CONTENT);
        CompareEngineResult full = new FullQuickCompare().compare(context);
        assertEquals(DiffCode.SAME, full.diffCode() & DiffCode.COMPAREFLAGS);
    }

    @Test
    void fullQuickCompareTreatsEncodingMismatchAsDiffWhenIgnoreCodepageDisabled() throws IOException {
        Path left = tempDir.resolve("left-utf8-bom.txt");
        Path right = tempDir.resolve("right-utf8.txt");
        Files.write(left, new byte[] {(byte) 0xEF, (byte) 0xBB, (byte) 0xBF});
        Files.writeString(left, "same\n", StandardCharsets.UTF_8, java.nio.file.StandardOpenOption.APPEND);
        Files.writeString(right, "same\n", StandardCharsets.UTF_8);

        CompareEngineContext context = context(left, right);
        context.setCompareType(DiffEngine.CompareType.CONTENT);
        context.setIgnoreCodepage(false);

        CompareEngineResult result = new FullQuickCompare().compare(context);
        assertEquals(DiffCode.DIFF, result.diffCode() & DiffCode.COMPAREFLAGS);
    }

    private CompareEngineContext context(Path left, Path right) {
        return new CompareEngineContext(new PathContext(left.toString(), right.toString()), new NioFileSystemService());
    }

    private CompareEngineContext context(Path left, Path middle, Path right) {
        return new CompareEngineContext(
                new PathContext(left.toString(), middle.toString(), right.toString()),
                new NioFileSystemService());
    }

    private Path write(String name, String content) throws IOException {
        Path path = tempDir.resolve(name);
        Files.writeString(path, content);
        return path;
    }
}
