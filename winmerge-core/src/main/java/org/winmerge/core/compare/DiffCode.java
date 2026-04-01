package org.winmerge.core.compare;

public final class DiffCode {
    public static final int TEXT_FLAGS = 0x3F;
    public static final int TEXT = 0x1;
    public static final int BIN = 0x2;
    public static final int BIN_SIDE_1 = 0x4;
    public static final int BIN_SIDE_2 = 0x8;
    public static final int BIN_SIDE_3 = 0x10;
    public static final int IMAGE = 0x20;
    public static final int TYPE_FLAGS = 0xC0;
    public static final int FILE = 0x40;
    public static final int DIR = 0x80;
    public static final int COMPARE_FLAGS = 0x7000;
    public static final int DIFF = 0x1000;
    public static final int SAME = 0x2000;
    public static final int CMP_ERR = 0x3000;
    public static final int CMP_ABORT = 0x4000;
    public static final int COMPARE_FLAGS_3WAY = 0x18000;
    public static final int DIFF_ALL = 0x0000;
    public static final int DIFF_1ST_ONLY = 0x8000;
    public static final int DIFF_2ND_ONLY = 0x10000;
    public static final int DIFF_3RD_ONLY = 0x18000;
    public static final int FILTER_FLAGS = 0x20000;
    public static final int INCLUDED = 0x00000;
    public static final int SKIPPED = 0x20000;
    public static final int SCAN_FLAGS = 0x100000;
    public static final int NEED_SCAN = 0x100000;
    public static final int THREE_WAY_FLAGS = 0x200000;
    public static final int THREE_WAY = 0x200000;
    public static final int EXPR_FLAGS = 0x400000;
    public static final int EXPR_DIFF = 0x400000;
    public static final int PATH_FLAGS = 0x800000;
    public static final int PATH_MISMATCH = 0x800000;
    public static final int SIDE_FLAGS = 0x70000000;
    public static final int FIRST = 0x10000000;
    public static final int SECOND = 0x20000000;
    public static final int THIRD = 0x40000000;
    public static final int BOTH = 0x30000000;
    public static final int ALL = 0x70000000;

    // Legacy compatibility aliases used by C++ parity ports.
    public static final int TEXTFLAGS = TEXT_FLAGS;
    public static final int BINSIDE1 = BIN_SIDE_1;
    public static final int BINSIDE2 = BIN_SIDE_2;
    public static final int BINSIDE3 = BIN_SIDE_3;
    public static final int TYPEFLAGS = TYPE_FLAGS;
    public static final int COMPAREFLAGS = COMPARE_FLAGS;
    public static final int CMPERR = CMP_ERR;
    public static final int CMPABORT = CMP_ABORT;
    public static final int COMPAREFLAGS3WAY = COMPARE_FLAGS_3WAY;
    public static final int DIFFALL = DIFF_ALL;
    public static final int DIFF1STONLY = DIFF_1ST_ONLY;
    public static final int DIFF2NDONLY = DIFF_2ND_ONLY;
    public static final int DIFF3RDONLY = DIFF_3RD_ONLY;
    public static final int FILTERFLAGS = FILTER_FLAGS;
    public static final int SCANFLAGS = SCAN_FLAGS;
    public static final int NEEDSCAN = NEED_SCAN;
    public static final int THREEWAYFLAGS = THREE_WAY_FLAGS;
    public static final int THREEWAY = THREE_WAY;
    public static final int EXPRFLAGS = EXPR_FLAGS;
    public static final int EXPRDIFF = EXPR_DIFF;
    public static final int PATHFLAGS = PATH_FLAGS;
    public static final int PATHMISMATCH = PATH_MISMATCH;
    public static final int SIDEFLAGS = SIDE_FLAGS;

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

    public boolean isResultAbort() {
        return (code & COMPARE_FLAGS) == CMP_ABORT;
    }

    public boolean isResultDiff() {
        return (code & COMPARE_FLAGS) == DIFF;
    }

    public boolean isResultSame() {
        return (code & COMPARE_FLAGS) == SAME;
    }

    public boolean isBin() {
        return (code & BIN) != 0;
    }

    public boolean isImage() {
        return (code & IMAGE) != 0;
    }

    public boolean exists(int index) {
        return switch (index) {
            case 0 -> (code & FIRST) != 0;
            case 1 -> (code & SECOND) != 0;
            case 2 -> (code & THIRD) != 0;
            default -> false;
        };
    }

    public static boolean isResultError(int code) {
        return (code & COMPARE_FLAGS) == CMP_ERR;
    }

    public static boolean isResultAbort(int code) {
        return (code & COMPARE_FLAGS) == CMP_ABORT;
    }
}
