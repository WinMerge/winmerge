package org.winmerge.core.diff;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;

class DiffStructuresTest {
    @Test
    void swapsDiffRangeSides() {
        DiffRange range = new DiffRange();
        range.begin()[0] = 1;
        range.begin()[1] = 10;
        range.end()[0] = 3;
        range.end()[1] = 12;
        range.blank()[0] = 0;
        range.blank()[1] = 2;

        range.swapSides(0, 1);

        assertEquals(10, range.begin()[0]);
        assertEquals(1, range.begin()[1]);
        assertEquals(12, range.end()[0]);
        assertEquals(3, range.end()[1]);
        assertEquals(2, range.blank()[0]);
        assertEquals(0, range.blank()[1]);
    }

    @Test
    void initializesDiffMapWithSentries() {
        DiffMap map = new DiffMap();
        map.initDiffMap(3);

        assertEquals(3, map.entries().size());
        assertEquals(DiffMap.BAD_MAP_ENTRY, map.entries().get(0));
        assertEquals(DiffMap.BAD_MAP_ENTRY, map.entries().get(1));
        assertEquals(DiffMap.BAD_MAP_ENTRY, map.entries().get(2));
    }

    @Test
    void tracksMovedLineMappings() {
        MovedLines movedLines = new MovedLines();
        movedLines.add(MovedLines.Side.LEFT, 4, 9);
        movedLines.add(MovedLines.Side.RIGHT, 11, 2);

        assertEquals(9, movedLines.lineInBlock(4, MovedLines.Side.LEFT));
        assertEquals(2, movedLines.lineInBlock(11, MovedLines.Side.RIGHT));
        assertEquals(-1, movedLines.lineInBlock(99, MovedLines.Side.LEFT));
    }
}
