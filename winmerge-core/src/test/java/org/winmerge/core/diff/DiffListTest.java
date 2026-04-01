package org.winmerge.core.diff;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertNull;
import static org.junit.jupiter.api.Assertions.assertTrue;

class DiffListTest {
    @Test
    void buildsSignificantChainsAndLineLookups() {
        DiffList list = new DiffList();
        list.addDiff(diff(0, 1, OperationType.TRIVIAL));
        list.addDiff(diff(3, 4, OperationType.DIFF));
        list.addDiff(diff(6, 8, OperationType.SECOND_ONLY));

        list.constructSignificantChain();

        assertEquals(3, list.getSize());
        assertEquals(2, list.getSignificantDiffs());
        assertFalse(list.isDiffSignificant(0));
        assertTrue(list.isDiffSignificant(1));
        assertEquals(0, list.getSignificantIndex(1));
        assertEquals(1, list.firstSignificantDiff());
        assertEquals(2, list.lastSignificantDiff());
        assertEquals(2, list.nextSignificantDiff(1));
        assertEquals(1, list.prevSignificantDiff(2));
        assertEquals(1, list.lineToDiff(3));
        assertEquals(-1, list.lineToDiff(2));
        assertEquals(2, list.nextSignificantDiffFromLine(5));
        assertEquals(1, list.prevSignificantDiffFromLine(5));
        assertNotNull(list.firstSignificantDiffRange());
        assertNotNull(list.lastSignificantDiffRange());
        assertEquals(2, list.firstSignificant3wayDiff(ThreeWayDiffType.MIDDLE_ONLY));
        assertEquals(2, list.lastSignificant3wayDiff(ThreeWayDiffType.MIDDLE_ONLY));
        assertEquals(1, list.firstSignificant3wayDiff(ThreeWayDiffType.CONFLICT));
        assertEquals(1, list.lastSignificant3wayDiff(ThreeWayDiffType.CONFLICT));
        assertNull(list.firstSignificant3wayDiffRange(ThreeWayDiffType.RIGHT_ONLY));

        int[] prevDiffOut = new int[] {-1};
        int[] nextDiffOut = new int[] {-1};
        assertFalse(list.getPrevDiff(5, prevDiffOut));
        assertEquals(1, prevDiffOut[0]);
        assertFalse(list.getNextDiff(5, nextDiffOut));
        assertEquals(2, nextDiffOut[0]);
    }

    @Test
    void appendsOffsetsAndComputesExtraLines() {
        DiffList list = new DiffList();
        DiffRange initial = diff(0, 2, OperationType.DIFF);
        initial.begin()[0] = 5;
        initial.end()[0] = 6;
        initial.begin()[1] = 9;
        initial.end()[1] = 11;
        initial.begin()[2] = 13;
        initial.end()[2] = 13;
        list.addDiff(initial);

        int[] extras = new int[] {0, 0, 0};
        list.getExtraLinesCounts(3, extras);
        assertArrayEquals(new int[] {1, 0, 2}, extras);

        DiffList append = new DiffList();
        DiffRange appendRange = diff(1, 1, OperationType.FIRST_ONLY);
        append.addDiff(appendRange);
        list.appendDiffList(append, new int[] {10, 20, 30}, 3);
        list.constructSignificantChain();

        DiffRange appended = list.diffRangeAt(1);
        assertNotNull(appended);
        assertEquals(10, appended.begin()[0]);
        assertEquals(20, appended.begin()[1]);
        assertEquals(30, appended.begin()[2]);
        assertEquals(4, appended.diffBegin());
        assertEquals(4, appended.diffEnd());
        assertEquals(2, appended.blank()[0]);
        assertEquals(2, appended.blank()[1]);
        assertEquals(2, appended.blank()[2]);
        assertEquals(0, list.getMergeableSrcIndex(1, 1));
        assertEquals(-1, list.getMergeableSrcIndex(1, 0));

        list.swap(0, 1);
        DiffRange swapped = list.diffRangeAt(0);
        assertNotNull(swapped);
        assertEquals(9, swapped.begin()[0]);
        assertEquals(5, swapped.begin()[1]);
    }

    private static DiffRange diff(int begin, int end, OperationType operationType) {
        DiffRange range = new DiffRange();
        range.setDiffBegin(begin);
        range.setDiffEnd(end);
        range.setOperation(operationType);
        return range;
    }
}
