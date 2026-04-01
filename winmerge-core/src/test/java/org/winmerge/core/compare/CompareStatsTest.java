package org.winmerge.core.compare;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

class CompareStatsTest {
    @Test
    void mapsDiffCodesToExpectedResults() {
        CompareStats stats = new CompareStats(3);

        int filteredFile = DiffCode.SKIPPED | DiffCode.FIRST;
        int secondOnlyFile = DiffCode.SECOND;
        int sameBinary = DiffCode.SAME | DiffCode.BIN | DiffCode.ALL;
        int leftMissingDir = DiffCode.DIR | DiffCode.SECOND | DiffCode.THIRD;

        assertEquals(CompareStats.Result.SKIP, stats.getResultFromCode(filteredFile));
        assertEquals(CompareStats.Result.M_UNIQUE, stats.getResultFromCode(secondOnlyFile));
        assertEquals(CompareStats.Result.BIN_SAME, stats.getResultFromCode(sameBinary));
        assertEquals(CompareStats.Result.L_DIR_MISSING, stats.getResultFromCode(leftMissingDir));
    }

    @Test
    void marksCompareDoneOnCompareToIdleTransition() {
        CompareStats stats = new CompareStats(2);

        stats.setCompareState(CompareStats.CompareState.START);
        stats.setCompareState(CompareStats.CompareState.COMPARE);
        stats.setCompareState(CompareStats.CompareState.IDLE);

        assertTrue(stats.isCompareDone());
    }
}
