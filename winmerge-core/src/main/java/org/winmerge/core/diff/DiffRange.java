package org.winmerge.core.diff;

import java.util.Arrays;

public class DiffRange {
    private final int[] begin;
    private final int[] end;
    private int diffBegin;
    private int diffEnd;
    private final int[] blank;
    private OperationType operation;

    public DiffRange() {
        this.begin = new int[3];
        this.end = new int[3];
        this.blank = new int[] {-1, -1, -1};
        this.operation = OperationType.NONE;
        this.diffBegin = 0;
        this.diffEnd = 0;
    }

    public DiffRange(DiffRange other) {
        this.begin = Arrays.copyOf(other.begin, other.begin.length);
        this.end = Arrays.copyOf(other.end, other.end.length);
        this.diffBegin = other.diffBegin;
        this.diffEnd = other.diffEnd;
        this.blank = Arrays.copyOf(other.blank, other.blank.length);
        this.operation = other.operation;
    }

    public int[] begin() {
        return begin;
    }

    public int[] end() {
        return end;
    }

    public int[] blank() {
        return blank;
    }

    public int diffBegin() {
        return diffBegin;
    }

    public void setDiffBegin(int diffBegin) {
        this.diffBegin = diffBegin;
    }

    public int diffEnd() {
        return diffEnd;
    }

    public void setDiffEnd(int diffEnd) {
        this.diffEnd = diffEnd;
    }

    public OperationType operation() {
        return operation;
    }

    public void setOperation(OperationType operation) {
        this.operation = operation;
    }

    public void swapSides(int index1, int index2) {
        validateIndex(index1);
        validateIndex(index2);

        int tmp = begin[index1];
        begin[index1] = begin[index2];
        begin[index2] = tmp;

        tmp = end[index1];
        end[index1] = end[index2];
        end[index2] = tmp;

        tmp = blank[index1];
        blank[index1] = blank[index2];
        blank[index2] = tmp;
    }

    private static void validateIndex(int index) {
        if (index < 0 || index > 2) {
            throw new IllegalArgumentException("Diff side index must be in [0,2]");
        }
    }
}
