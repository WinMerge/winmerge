package org.winmerge.core.diff;

import java.util.ArrayList;
import java.util.List;

public final class DiffList {
    private static final int INVALID_INDEX = -1;
    private static final int MAX_FILES = 3;

    private final List<DiffRangeInfo> diffs;

    private int firstSignificant;
    private int lastSignificant;
    private int firstSignificantLeftMiddle;
    private int firstSignificantLeftRight;
    private int firstSignificantMiddleRight;
    private int firstSignificantLeftOnly;
    private int firstSignificantMiddleOnly;
    private int firstSignificantRightOnly;
    private int firstSignificantConflict;
    private int lastSignificantLeftMiddle;
    private int lastSignificantLeftRight;
    private int lastSignificantMiddleRight;
    private int lastSignificantLeftOnly;
    private int lastSignificantMiddleOnly;
    private int lastSignificantRightOnly;
    private int lastSignificantConflict;

    public DiffList() {
        diffs = new ArrayList<>(64);
        resetSignificantIndices();
    }

    public void clear() {
        diffs.clear();
        resetSignificantIndices();
    }

    public int getSize() {
        return diffs.size();
    }

    public int getSignificantDiffs() {
        int significant = 0;
        for (DiffRangeInfo diff : diffs) {
            if (diff.operation() != OperationType.TRIVIAL) {
                significant++;
            }
        }
        return significant;
    }

    public void addDiff(DiffRange diffRange) {
        diffs.add(new DiffRangeInfo(diffRange));
    }

    public boolean isDiffSignificant(int diffIndex) {
        DiffRange diffRange = diffRangeAt(diffIndex);
        return diffRange != null && diffRange.operation() != OperationType.TRIVIAL;
    }

    public int getSignificantIndex(int diffIndex) {
        int significant = -1;
        int end = Math.min(diffIndex, diffs.size() - 1);
        for (int i = 0; i <= end; i++) {
            if (diffs.get(i).operation() != OperationType.TRIVIAL) {
                significant++;
            }
        }
        return significant;
    }

    public boolean getDiff(int diffIndex, DiffRange output) {
        DiffRange diffRange = diffRangeAt(diffIndex);
        if (diffRange == null) {
            return false;
        }
        copyRange(diffRange, output);
        return true;
    }

    public boolean setDiff(int diffIndex, DiffRange diffRange) {
        if (diffIndex < 0 || diffIndex >= diffs.size()) {
            return false;
        }
        diffs.set(diffIndex, new DiffRangeInfo(diffRange));
        return true;
    }

    public int lineRelDiff(int line, int diffIndex) {
        DiffRange diffRange = diffRangeAt(diffIndex);
        if (diffRange == null) {
            return INVALID_INDEX;
        }
        if (line < diffRange.diffBegin()) {
            return -1;
        }
        if (line > diffRange.diffEnd()) {
            return 1;
        }
        return 0;
    }

    public boolean lineInDiff(int line, int diffIndex) {
        DiffRange diffRange = diffRangeAt(diffIndex);
        return diffRange != null && line >= diffRange.diffBegin() && line <= diffRange.diffEnd();
    }

    public int lineToDiff(int line) {
        int diffCount = diffs.size();
        if (diffCount == 0) {
            return INVALID_INDEX;
        }

        if (line < diffs.get(0).diffBegin()) {
            return INVALID_INDEX;
        }
        if (line > diffs.get(diffCount - 1).diffEnd()) {
            return INVALID_INDEX;
        }

        int left = 0;
        int right = diffCount - 1;
        while (left <= right) {
            int middle = (left + right) / 2;
            int relation = lineRelDiff(line, middle);
            if (relation == -1) {
                right = middle - 1;
            } else if (relation == 1) {
                left = middle + 1;
            } else {
                return middle;
            }
        }
        return INVALID_INDEX;
    }

    public boolean getPrevDiff(int line, int[] diffIndexOut) {
        validateDiffIndexOut(diffIndexOut);
        boolean lineInDiff = true;
        int diffIndex = lineToDiff(line);
        if (diffIndex == INVALID_INDEX) {
            lineInDiff = false;
            for (int i = diffs.size() - 1; i >= 0; i--) {
                if (diffs.get(i).diffEnd() <= line) {
                    diffIndex = i;
                    break;
                }
            }
        }
        diffIndexOut[0] = diffIndex;
        return lineInDiff;
    }

    public boolean getNextDiff(int line, int[] diffIndexOut) {
        validateDiffIndexOut(diffIndexOut);
        boolean lineInDiff = true;
        int diffIndex = lineToDiff(line);
        if (diffIndex == INVALID_INDEX) {
            lineInDiff = false;
            for (int i = 0; i < diffs.size(); i++) {
                if (diffs.get(i).diffBegin() >= line) {
                    diffIndex = i;
                    break;
                }
            }
        }
        diffIndexOut[0] = diffIndex;
        return lineInDiff;
    }

    public boolean hasSignificantDiffs() {
        return firstSignificant != INVALID_INDEX;
    }

    public int prevSignificantDiffFromLine(int line) {
        for (int i = diffs.size() - 1; i >= 0; i--) {
            DiffRangeInfo diff = diffs.get(i);
            if (diff.operation() != OperationType.TRIVIAL && diff.diffEnd() <= line) {
                return i;
            }
        }
        return INVALID_INDEX;
    }

    public int nextSignificantDiffFromLine(int line) {
        for (int i = 0; i < diffs.size(); i++) {
            DiffRangeInfo diff = diffs.get(i);
            if (diff.operation() != OperationType.TRIVIAL && diff.diffBegin() >= line) {
                return i;
            }
        }
        return INVALID_INDEX;
    }

    public int firstSignificantDiff() {
        return firstSignificant;
    }

    public int nextSignificantDiff(int diffIndex) {
        if (diffIndex < 0 || diffIndex >= diffs.size()) {
            return INVALID_INDEX;
        }
        return diffs.get(diffIndex).next();
    }

    public int prevSignificantDiff(int diffIndex) {
        if (diffIndex < 0 || diffIndex >= diffs.size()) {
            return INVALID_INDEX;
        }
        return diffs.get(diffIndex).prev();
    }

    public int lastSignificantDiff() {
        return lastSignificant;
    }

    public DiffRange firstSignificantDiffRange() {
        if (firstSignificant == INVALID_INDEX) {
            return null;
        }
        return diffRangeAt(firstSignificant);
    }

    public DiffRange lastSignificantDiffRange() {
        if (lastSignificant == INVALID_INDEX) {
            return null;
        }
        return diffRangeAt(lastSignificant);
    }

    public int prevSignificant3wayDiffFromLine(int line, ThreeWayDiffType diffType) {
        for (int i = diffs.size() - 1; i >= 0; i--) {
            DiffRangeInfo diff = diffs.get(i);
            if (matches3wayType(diff.operation(), diffType) && diff.diffEnd() <= line) {
                return i;
            }
        }
        return INVALID_INDEX;
    }

    public int nextSignificant3wayDiffFromLine(int line, ThreeWayDiffType diffType) {
        for (int i = 0; i < diffs.size(); i++) {
            DiffRangeInfo diff = diffs.get(i);
            if (matches3wayType(diff.operation(), diffType) && diff.diffBegin() >= line) {
                return i;
            }
        }
        return INVALID_INDEX;
    }

    public int firstSignificant3wayDiff(ThreeWayDiffType diffType) {
        return switch (diffType) {
            case LEFT_MIDDLE -> firstSignificantLeftMiddle;
            case LEFT_RIGHT -> firstSignificantLeftRight;
            case MIDDLE_RIGHT -> firstSignificantMiddleRight;
            case LEFT_ONLY -> firstSignificantLeftOnly;
            case MIDDLE_ONLY -> firstSignificantMiddleOnly;
            case RIGHT_ONLY -> firstSignificantRightOnly;
            case CONFLICT -> firstSignificantConflict;
        };
    }

    public int nextSignificant3wayDiff(int diffIndex, ThreeWayDiffType diffType) {
        if (diffIndex < 0 || diffIndex >= diffs.size()) {
            return INVALID_INDEX;
        }
        while (diffs.get(diffIndex).next() != INVALID_INDEX) {
            diffIndex = diffs.get(diffIndex).next();
            if (matches3wayType(diffs.get(diffIndex).operation(), diffType)) {
                return diffIndex;
            }
        }
        return INVALID_INDEX;
    }

    public int prevSignificant3wayDiff(int diffIndex, ThreeWayDiffType diffType) {
        if (diffIndex < 0 || diffIndex >= diffs.size()) {
            return INVALID_INDEX;
        }
        while (diffs.get(diffIndex).prev() != INVALID_INDEX) {
            diffIndex = diffs.get(diffIndex).prev();
            if (matches3wayType(diffs.get(diffIndex).operation(), diffType)) {
                return diffIndex;
            }
        }
        return INVALID_INDEX;
    }

    public int lastSignificant3wayDiff(ThreeWayDiffType diffType) {
        return switch (diffType) {
            case LEFT_MIDDLE -> lastSignificantLeftMiddle;
            case LEFT_RIGHT -> lastSignificantLeftRight;
            case MIDDLE_RIGHT -> lastSignificantMiddleRight;
            case LEFT_ONLY -> lastSignificantLeftOnly;
            case MIDDLE_ONLY -> lastSignificantMiddleOnly;
            case RIGHT_ONLY -> lastSignificantRightOnly;
            case CONFLICT -> lastSignificantConflict;
        };
    }

    public DiffRange firstSignificant3wayDiffRange(ThreeWayDiffType diffType) {
        int diffIndex = firstSignificant3wayDiff(diffType);
        return diffIndex == INVALID_INDEX ? null : diffRangeAt(diffIndex);
    }

    public DiffRange lastSignificant3wayDiffRange(ThreeWayDiffType diffType) {
        int diffIndex = lastSignificant3wayDiff(diffType);
        return diffIndex == INVALID_INDEX ? null : diffRangeAt(diffIndex);
    }

    public int getMergeableSrcIndex(int diffIndex, int destinationIndex) {
        DiffRange diffRange = diffRangeAt(diffIndex);
        if (diffRange == null) {
            return INVALID_INDEX;
        }
        return switch (destinationIndex) {
            case 0, 2 -> diffRange.operation() == OperationType.SECOND_ONLY ? 1 : INVALID_INDEX;
            case 1 -> {
                if (diffRange.operation() == OperationType.FIRST_ONLY || diffRange.operation() == OperationType.SECOND_ONLY) {
                    yield 0;
                }
                if (diffRange.operation() == OperationType.THIRD_ONLY) {
                    yield 2;
                }
                yield INVALID_INDEX;
            }
            default -> INVALID_INDEX;
        };
    }

    public DiffRange diffRangeAt(int diffIndex) {
        if (diffIndex < 0 || diffIndex >= diffs.size()) {
            return null;
        }
        return diffs.get(diffIndex);
    }

    public void constructSignificantChain() {
        resetSignificantIndices();
        int prev = INVALID_INDEX;
        for (int i = 0; i < diffs.size(); i++) {
            DiffRangeInfo diff = diffs.get(i);
            if (diff.operation() == OperationType.TRIVIAL) {
                diff.initLinks();
                continue;
            }

            diff.setPrev(prev);
            if (prev != INVALID_INDEX) {
                diffs.get(prev).setNext(i);
            }
            prev = i;

            if (firstSignificant == INVALID_INDEX) {
                firstSignificant = i;
            }
            lastSignificant = i;

            if (diff.operation() != OperationType.THIRD_ONLY) {
                if (firstSignificantLeftMiddle == INVALID_INDEX) {
                    firstSignificantLeftMiddle = i;
                }
                lastSignificantLeftMiddle = i;
            }
            if (diff.operation() != OperationType.SECOND_ONLY) {
                if (firstSignificantLeftRight == INVALID_INDEX) {
                    firstSignificantLeftRight = i;
                }
                lastSignificantLeftRight = i;
            }
            if (diff.operation() != OperationType.FIRST_ONLY) {
                if (firstSignificantMiddleRight == INVALID_INDEX) {
                    firstSignificantMiddleRight = i;
                }
                lastSignificantMiddleRight = i;
            }
            if (diff.operation() == OperationType.FIRST_ONLY) {
                if (firstSignificantLeftOnly == INVALID_INDEX) {
                    firstSignificantLeftOnly = i;
                }
                lastSignificantLeftOnly = i;
            }
            if (diff.operation() == OperationType.SECOND_ONLY) {
                if (firstSignificantMiddleOnly == INVALID_INDEX) {
                    firstSignificantMiddleOnly = i;
                }
                lastSignificantMiddleOnly = i;
            }
            if (diff.operation() == OperationType.THIRD_ONLY) {
                if (firstSignificantRightOnly == INVALID_INDEX) {
                    firstSignificantRightOnly = i;
                }
                lastSignificantRightOnly = i;
            }
            if (diff.operation() == OperationType.DIFF) {
                if (firstSignificantConflict == INVALID_INDEX) {
                    firstSignificantConflict = i;
                }
                lastSignificantConflict = i;
            }
        }
    }

    public void swap(int index1, int index2) {
        for (DiffRangeInfo diff : diffs) {
            diff.swapSides(index1, index2);
        }
    }

    public void getExtraLinesCounts(int files, int[] extras) {
        if (files < 1 || files > MAX_FILES) {
            throw new IllegalArgumentException("files must be in [1,3]");
        }
        if (extras == null || extras.length < MAX_FILES) {
            throw new IllegalArgumentException("extras must provide at least 3 entries");
        }
        extras[0] = 0;
        extras[1] = 0;
        extras[2] = 0;

        for (int diffIndex = 0; diffIndex < getSize(); diffIndex++) {
            DiffRange diff = diffs.get(diffIndex);
            int maxLines = 0;
            int[] lines = new int[MAX_FILES];
            for (int file = 0; file < files; file++) {
                lines[file] = diff.end()[file] - diff.begin()[file] + 1;
                maxLines = Math.max(maxLines, lines[file]);
            }
            for (int file = 0; file < files; file++) {
                extras[file] += maxLines - lines[file];
            }
        }
    }

    public List<DiffRangeInfo> getDiffRangeInfoVector() {
        return diffs;
    }

    public void appendDiffList(DiffList list) {
        appendDiffList(list, null, 0);
    }

    public void appendDiffList(DiffList list, int[] offset, int diffOffset) {
        if (offset != null && offset.length < MAX_FILES) {
            throw new IllegalArgumentException("offset must provide at least 3 entries");
        }
        for (DiffRangeInfo source : list.diffs) {
            DiffRangeInfo range = new DiffRangeInfo(source);
            for (int file = 0; file < MAX_FILES; file++) {
                if (offset != null) {
                    range.begin()[file] += offset[file];
                    range.end()[file] += offset[file];
                }
                if (diffOffset != 0) {
                    range.blank()[file] += diffOffset;
                }
            }
            if (diffOffset != 0) {
                range.setDiffBegin(range.diffBegin() + diffOffset);
                range.setDiffEnd(range.diffEnd() + diffOffset);
            }
            addDiff(range);
        }
    }

    private static boolean matches3wayType(OperationType operation, ThreeWayDiffType diffType) {
        return switch (diffType) {
            case LEFT_MIDDLE -> operation != OperationType.TRIVIAL && operation != OperationType.THIRD_ONLY;
            case LEFT_RIGHT -> operation != OperationType.TRIVIAL && operation != OperationType.SECOND_ONLY;
            case MIDDLE_RIGHT -> operation != OperationType.TRIVIAL && operation != OperationType.FIRST_ONLY;
            case LEFT_ONLY -> operation == OperationType.FIRST_ONLY;
            case MIDDLE_ONLY -> operation == OperationType.SECOND_ONLY;
            case RIGHT_ONLY -> operation == OperationType.THIRD_ONLY;
            case CONFLICT -> operation == OperationType.DIFF;
        };
    }

    private static void copyRange(DiffRange source, DiffRange target) {
        System.arraycopy(source.begin(), 0, target.begin(), 0, MAX_FILES);
        System.arraycopy(source.end(), 0, target.end(), 0, MAX_FILES);
        System.arraycopy(source.blank(), 0, target.blank(), 0, MAX_FILES);
        target.setDiffBegin(source.diffBegin());
        target.setDiffEnd(source.diffEnd());
        target.setOperation(source.operation());
    }

    private static void validateDiffIndexOut(int[] diffIndexOut) {
        if (diffIndexOut == null || diffIndexOut.length == 0) {
            throw new IllegalArgumentException("diffIndexOut must have at least one entry");
        }
    }

    private void resetSignificantIndices() {
        firstSignificant = INVALID_INDEX;
        lastSignificant = INVALID_INDEX;
        firstSignificantLeftMiddle = INVALID_INDEX;
        firstSignificantLeftRight = INVALID_INDEX;
        firstSignificantMiddleRight = INVALID_INDEX;
        firstSignificantLeftOnly = INVALID_INDEX;
        firstSignificantMiddleOnly = INVALID_INDEX;
        firstSignificantRightOnly = INVALID_INDEX;
        firstSignificantConflict = INVALID_INDEX;
        lastSignificantLeftMiddle = INVALID_INDEX;
        lastSignificantLeftRight = INVALID_INDEX;
        lastSignificantMiddleRight = INVALID_INDEX;
        lastSignificantLeftOnly = INVALID_INDEX;
        lastSignificantMiddleOnly = INVALID_INDEX;
        lastSignificantRightOnly = INVALID_INDEX;
        lastSignificantConflict = INVALID_INDEX;
    }
}
