package org.winmerge.core.compare;

import java.util.ArrayList;
import java.util.EnumMap;
import java.util.List;
import java.util.concurrent.atomic.AtomicInteger;

public final class CompareStats {
    public enum CompareState {
        IDLE,
        START,
        COMPARE
    }

    public enum Result {
        L_UNIQUE,
        M_UNIQUE,
        R_UNIQUE,
        L_MISSING,
        M_MISSING,
        R_MISSING,
        DIFF,
        SAME,
        BIN_SAME,
        BIN_DIFF,
        L_DIR_UNIQUE,
        M_DIR_UNIQUE,
        R_DIR_UNIQUE,
        L_DIR_MISSING,
        M_DIR_MISSING,
        R_DIR_MISSING,
        SKIP,
        DIR_SKIP,
        DIR_SAME,
        DIR_DIFF,
        ERROR
    }

    private static final class ThreadState {
        private int hitCount;
        private DiffCode diffCode;

        private void reset(DiffCode current) {
            hitCount = 0;
            diffCode = current;
        }
    }

    private final EnumMap<Result, AtomicInteger> counts;
    private final AtomicInteger totalItems;
    private final AtomicInteger comparedItems;
    private final int directoryCount;
    private final List<ThreadState> threadStates;

    private CompareState state;
    private boolean compareDone;
    private int idleCompareThreadCount;

    public CompareStats(int directoryCount) {
        this.directoryCount = directoryCount;
        this.counts = new EnumMap<>(Result.class);
        for (Result result : Result.values()) {
            counts.put(result, new AtomicInteger(0));
        }
        this.totalItems = new AtomicInteger(0);
        this.comparedItems = new AtomicInteger(0);
        this.threadStates = new ArrayList<>();
        this.state = CompareState.IDLE;
        this.compareDone = false;
        this.idleCompareThreadCount = 0;
    }

    public int getCompareThreadCount() {
        return threadStates.size();
    }

    public void setCompareThreadCount(int compareThreads) {
        threadStates.clear();
        for (int i = 0; i < compareThreads; i++) {
            threadStates.add(new ThreadState());
        }
    }

    public int getIdleCompareThreadCount() {
        return idleCompareThreadCount;
    }

    public void setIdleCompareThreadCount(int idleCompareThreadCount) {
        if (idleCompareThreadCount >= threadStates.size()) {
            throw new IllegalArgumentException("idleCompareThreadCount must be less than thread count");
        }
        this.idleCompareThreadCount = idleCompareThreadCount;
    }

    public boolean isIdleCompareThread(int compareThreadIndex) {
        return compareThreadIndex >= (threadStates.size() - idleCompareThreadCount);
    }

    public void beginCompare(DiffCode diffCode, int compareThreadIndex) {
        threadStates.get(compareThreadIndex).reset(diffCode);
    }

    public void addItem(int diffCode) {
        if (diffCode == -1) {
            comparedItems.incrementAndGet();
            return;
        }
        addItem(getResultFromCode(diffCode));
    }

    public void addItem(Result result) {
        counts.get(result).incrementAndGet();
        comparedItems.incrementAndGet();
    }

    public void increaseTotalItems(int count) {
        totalItems.addAndGet(count);
    }

    public int getCount(Result result) {
        return counts.get(result).get();
    }

    public int getTotalItems() {
        return totalItems.get();
    }

    public int getComparedItems() {
        return comparedItems.get();
    }

    public void reset() {
        for (AtomicInteger count : counts.values()) {
            count.set(0);
        }
        setCompareState(CompareState.IDLE);
        totalItems.set(0);
        comparedItems.set(0);
        compareDone = false;
        threadStates.clear();
    }

    public void setCompareState(CompareState state) {
        if (state == CompareState.START) {
            compareDone = false;
        }
        if (state == CompareState.IDLE && this.state == CompareState.COMPARE) {
            compareDone = true;
        }
        this.state = state;
    }

    public CompareState getCompareState() {
        return state;
    }

    public boolean isCompareDone() {
        return compareDone;
    }

    public int getDirectoryCount() {
        return directoryCount;
    }

    public Result getResultFromCode(int rawCode) {
        DiffCode code = new DiffCode(rawCode);
        boolean isDir = code.isDirectory();

        if (code.isResultFiltered()) {
            return isDir ? Result.DIR_SKIP : Result.SKIP;
        }
        if (code.isSideFirstOnly()) {
            return isDir ? Result.L_DIR_UNIQUE : Result.L_UNIQUE;
        }
        if (code.isSideSecondOnly()) {
            if (isDir) {
                return directoryCount < 3 ? Result.R_DIR_UNIQUE : Result.M_DIR_UNIQUE;
            }
            return directoryCount < 3 ? Result.R_UNIQUE : Result.M_UNIQUE;
        }
        if (code.isSideThirdOnly()) {
            return isDir ? Result.R_DIR_UNIQUE : Result.R_UNIQUE;
        }

        int allMask = rawCode & DiffCode.ALL;
        if (directoryCount > 2) {
            if (allMask == (DiffCode.SECOND | DiffCode.THIRD)) {
                return isDir ? Result.L_DIR_MISSING : Result.L_MISSING;
            }
            if (allMask == (DiffCode.FIRST | DiffCode.THIRD)) {
                return isDir ? Result.M_DIR_MISSING : Result.M_MISSING;
            }
            if (allMask == (DiffCode.FIRST | DiffCode.SECOND)) {
                return isDir ? Result.R_DIR_MISSING : Result.R_MISSING;
            }
        }

        if (code.isResultError()) {
            return Result.ERROR;
        }
        if (code.isResultSame()) {
            if (isDir) {
                return Result.DIR_SAME;
            }
            return code.isBin() ? Result.BIN_SAME : Result.SAME;
        }

        if (isDir) {
            return Result.DIR_DIFF;
        }
        return code.isBin() ? Result.BIN_DIFF : Result.DIFF;
    }

    public void swap(int idx1, int idx2) {
        int secondIndex = directoryCount < 3 ? idx2 + 1 : idx2;
        swapPair(Result.values()[Result.L_UNIQUE.ordinal() + idx1], Result.values()[Result.L_UNIQUE.ordinal() + secondIndex]);
        swapPair(Result.values()[Result.L_MISSING.ordinal() + idx1], Result.values()[Result.L_MISSING.ordinal() + secondIndex]);
        swapPair(Result.values()[Result.L_DIR_UNIQUE.ordinal() + idx1], Result.values()[Result.L_DIR_UNIQUE.ordinal() + secondIndex]);
        swapPair(Result.values()[Result.L_DIR_MISSING.ordinal() + idx1], Result.values()[Result.L_DIR_MISSING.ordinal() + secondIndex]);
    }

    private void swapPair(Result left, Result right) {
        int current = counts.get(left).getAndSet(counts.get(right).get());
        counts.get(right).set(current);
    }
}
