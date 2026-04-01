package org.winmerge.core.diff;

import org.junit.jupiter.api.Test;

import java.util.ArrayList;
import java.util.List;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

class ThreeWayDiffTest {
    @Test
    void marksThirdOnlyWhenOnlyDiff12Exists() {
        List<DiffRange> diff3 = new ArrayList<>();
        List<DiffRange> diff10 = new ArrayList<>();
        List<DiffRange> diff12 = List.of(range(5, 8, 15, 19, OperationType.DIFF));

        int count = ThreeWayDiff.make3WayDiff(diff3, diff10, diff12, ignored -> true, false);

        assertEquals(1, count);
        assertEquals(OperationType.THIRD_ONLY, diff3.get(0).operation());
    }

    @Test
    void marksFirstOnlyWhenOnlyDiff10Exists() {
        List<DiffRange> diff3 = new ArrayList<>();
        List<DiffRange> diff10 = List.of(range(3, 4, 9, 10, OperationType.DIFF));
        List<DiffRange> diff12 = new ArrayList<>();

        int count = ThreeWayDiff.make3WayDiff(diff3, diff10, diff12, ignored -> true, false);

        assertEquals(1, count);
        assertEquals(OperationType.FIRST_ONLY, diff3.get(0).operation());
    }

    @Test
    void marksConflictOrSecondOnlyBasedOnComparator() {
        List<DiffRange> diff3 = new ArrayList<>();
        List<DiffRange> diff10 = List.of(range(5, 5, 30, 30, OperationType.DIFF));
        List<DiffRange> diff12 = List.of(range(5, 5, 40, 40, OperationType.DIFF));

        ThreeWayDiff.make3WayDiff(diff3, diff10, diff12, ignored -> false, false);
        assertEquals(OperationType.DIFF, diff3.get(0).operation());

        diff3.clear();
        ThreeWayDiff.make3WayDiff(diff3, diff10, diff12, ignored -> true, false);
        assertEquals(OperationType.SECOND_ONLY, diff3.get(0).operation());
    }

    @Test
    void propagatesTrivialOperationWhenEnabled() {
        List<DiffRange> diff3 = new ArrayList<>();
        List<DiffRange> diff10 = List.of(range(2, 2, 7, 7, OperationType.TRIVIAL));
        List<DiffRange> diff12 = List.of(range(2, 2, 8, 8, OperationType.TRIVIAL));

        ThreeWayDiff.make3WayDiff(diff3, diff10, diff12, ignored -> false, true);

        assertEquals(1, diff3.size());
        assertEquals(OperationType.TRIVIAL, diff3.get(0).operation());
    }

    @Test
    void clipsOverlappingRangesAtBoundary() {
        List<DiffRange> diff3 = new ArrayList<>();
        List<DiffRange> diff10 = List.of(
                range(1, 3, 10, 12, OperationType.DIFF),
                range(4, 5, 13, 14, OperationType.DIFF)
        );
        List<DiffRange> diff12 = List.of(
                range(1, 3, 20, 22, OperationType.DIFF),
                range(4, 5, 23, 24, OperationType.DIFF)
        );

        int count = ThreeWayDiff.make3WayDiff(diff3, diff10, diff12, ignored -> false, false);

        assertEquals(2, count);
        assertTrue(diff3.get(0).end()[0] < diff3.get(1).begin()[0]);
        assertTrue(diff3.get(0).end()[1] < diff3.get(1).begin()[1]);
        assertTrue(diff3.get(0).end()[2] < diff3.get(1).begin()[2]);
        assertFalse(diff3.isEmpty());
    }

    private static DiffRange range(int begin0, int end0, int begin1, int end1, OperationType operation) {
        DiffRange range = new DiffRange();
        range.begin()[0] = begin0;
        range.end()[0] = end0;
        range.begin()[1] = begin1;
        range.end()[1] = end1;
        range.setOperation(operation);
        return range;
    }
}
