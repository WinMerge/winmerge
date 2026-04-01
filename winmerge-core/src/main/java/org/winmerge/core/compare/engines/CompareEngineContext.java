package org.winmerge.core.compare.engines;

import org.winmerge.core.compare.CompareOptions;
import org.winmerge.core.compare.DiffutilsOptions;
import org.winmerge.core.compare.QuickCompareOptions;
import org.winmerge.core.diff.DiffEngine;
import org.winmerge.core.io.FileSystemService;
import org.winmerge.core.io.FileTextStats;
import org.winmerge.core.io.PathContext;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;

public final class CompareEngineContext {
    private static final int MAX_SIDES = 3;
    private static final AbortChecker NEVER_ABORT = () -> false;

    @FunctionalInterface
    public interface AbortChecker {
        boolean shouldAbort();
    }

    private final PathContext paths;
    private final FileSystemService fileSystem;
    private final CompareFileInfo[] fileInfos;
    private final FileTextStats[] textStats;
    private final Path[] normalizedPaths;

    private CompareOptions compareOptions;
    private QuickCompareOptions quickCompareOptions;
    private DiffutilsOptions diffutilsOptions;
    private DiffEngine.CompareType compareType;
    private AbortChecker abortChecker;
    private boolean ignoreSmallTimeDiff;
    private boolean stopAfterFirstDiff;
    private boolean ignoreCodepage;
    private double colorDistanceThreshold;

    public CompareEngineContext(PathContext paths) {
        this(paths, FileSystemService.createDefault());
    }

    public CompareEngineContext(PathContext paths, FileSystemService fileSystem) {
        this.paths = paths;
        this.fileSystem = fileSystem;
        this.fileInfos = new CompareFileInfo[] {new CompareFileInfo(), new CompareFileInfo(), new CompareFileInfo()};
        this.textStats = new FileTextStats[] {new FileTextStats(), new FileTextStats(), new FileTextStats()};
        this.normalizedPaths = new Path[MAX_SIDES];
        this.compareOptions = new CompareOptions();
        this.quickCompareOptions = new QuickCompareOptions(compareOptions);
        this.diffutilsOptions = new DiffutilsOptions(compareOptions);
        this.compareType = DiffEngine.CompareType.QUICK_CONTENT;
        this.abortChecker = NEVER_ABORT;
        this.ignoreSmallTimeDiff = false;
        this.stopAfterFirstDiff = false;
        this.ignoreCodepage = true;
        this.colorDistanceThreshold = 0.0d;
        refreshFileInfos();
    }

    public PathContext getPaths() {
        return paths;
    }

    public int getFileCount() {
        return paths.getSize();
    }

    public Path getPath(int index) {
        validateIndex(index);
        return normalizedPaths[index];
    }

    public FileSystemService getFileSystem() {
        return fileSystem;
    }

    public CompareFileInfo getFileInfo(int index) {
        validateIndex(index);
        return fileInfos[index];
    }

    public FileTextStats getTextStats(int index) {
        validateIndex(index);
        return textStats[index];
    }

    public CompareOptions getCompareOptions() {
        return compareOptions;
    }

    public void setCompareOptions(CompareOptions compareOptions) {
        this.compareOptions = compareOptions;
        this.quickCompareOptions = new QuickCompareOptions(compareOptions);
        this.diffutilsOptions = new DiffutilsOptions(compareOptions);
    }

    public QuickCompareOptions getQuickCompareOptions() {
        return quickCompareOptions;
    }

    public void setQuickCompareOptions(QuickCompareOptions quickCompareOptions) {
        this.quickCompareOptions = quickCompareOptions;
    }

    public DiffutilsOptions getDiffutilsOptions() {
        return diffutilsOptions;
    }

    public void setDiffutilsOptions(DiffutilsOptions diffutilsOptions) {
        this.diffutilsOptions = diffutilsOptions;
    }

    public DiffEngine.CompareType getCompareType() {
        return compareType;
    }

    public void setCompareType(DiffEngine.CompareType compareType) {
        this.compareType = compareType;
    }

    public AbortChecker getAbortChecker() {
        return abortChecker;
    }

    public void setAbortChecker(AbortChecker abortChecker) {
        this.abortChecker = abortChecker == null ? NEVER_ABORT : abortChecker;
    }

    public boolean isIgnoreSmallTimeDiff() {
        return ignoreSmallTimeDiff;
    }

    public void setIgnoreSmallTimeDiff(boolean ignoreSmallTimeDiff) {
        this.ignoreSmallTimeDiff = ignoreSmallTimeDiff;
    }

    public boolean isStopAfterFirstDiff() {
        return stopAfterFirstDiff;
    }

    public void setStopAfterFirstDiff(boolean stopAfterFirstDiff) {
        this.stopAfterFirstDiff = stopAfterFirstDiff;
    }

    public boolean isIgnoreCodepage() {
        return ignoreCodepage;
    }

    public void setIgnoreCodepage(boolean ignoreCodepage) {
        this.ignoreCodepage = ignoreCodepage;
    }

    public double getColorDistanceThreshold() {
        return colorDistanceThreshold;
    }

    public void setColorDistanceThreshold(double colorDistanceThreshold) {
        this.colorDistanceThreshold = colorDistanceThreshold;
    }

    public boolean exists(int index) {
        return getFileInfo(index).exists();
    }

    public void refreshFileInfos() {
        for (int i = 0; i < MAX_SIDES; i++) {
            fileInfos[i].clear();
            textStats[i].clear();
            normalizedPaths[i] = null;
            if (i >= getFileCount()) {
                continue;
            }

            Path normalized = fileSystem.normalize(Path.of(paths.getPath(i, true)));
            normalizedPaths[i] = normalized;

            boolean exists = fileSystem.exists(normalized);
            fileInfos[i].setExists(exists);
            if (!exists) {
                continue;
            }

            try {
                fileInfos[i].setSize(fileSystem.size(normalized));
            } catch (IOException ignored) {
                fileInfos[i].setSize(CompareFileInfo.FILE_SIZE_NONE);
            }

            try {
                fileInfos[i].setModifiedTimeMillis(Files.getLastModifiedTime(normalized).toMillis());
            } catch (IOException ignored) {
                fileInfos[i].setModifiedTimeMillis(CompareFileInfo.TIME_VALUE_NONE);
            }
        }
    }

    private void validateIndex(int index) {
        if (index < 0 || index >= MAX_SIDES) {
            throw new IllegalArgumentException("Index must be in [0,2]");
        }
    }
}
