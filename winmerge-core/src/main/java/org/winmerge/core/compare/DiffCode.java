package org.winmerge.core.compare;

public final class DiffCode {
    public static final int TEXT_FLAGS = 0x3F;
    public static final int BIN = 0x2;
    public static final int TYPE_FLAGS = 0xC0;
    public static final int DIR = 0x80;
    public static final int COMPARE_FLAGS = 0x7000;
    public static final int SAME = 0x2000;
    public static final int CMP_ERR = 0x3000;
    public static final int FILTER_FLAGS = 0x20000;
    public static final int SKIPPED = 0x20000;
    public static final int SIDE_FLAGS = 0x70000000;
    public static final int FIRST = 0x10000000;
    public static final int SECOND = 0x20000000;
    public static final int THIRD = 0x40000000;
    public static final int BOTH = 0x30000000;
    public static final int ALL = 0x70000000;

    private final int code;

    public DiffCode(int code) {
        this.code = code;
    }

    public int value() {
        return code;
    }

    public boolean isDirectory() {
        return (code & TYPE_FLAGS) == DIR;
    }

    public boolean isSideFirstOnly() {
        return (code & SIDE_FLAGS) == FIRST;
    }

    public boolean isSideSecondOnly() {
        return (code & SIDE_FLAGS) == SECOND;
    }

    public boolean isSideThirdOnly() {
        return (code & SIDE_FLAGS) == THIRD;
    }

    public boolean isResultFiltered() {
        return (code & FILTER_FLAGS) == SKIPPED;
    }

    public boolean isResultError() {
        return (code & COMPARE_FLAGS) == CMP_ERR;
    }

    public boolean isResultSame() {
        return (code & COMPARE_FLAGS) == SAME;
    }

    public boolean isBin() {
        return (code & BIN) != 0;
    }
}
