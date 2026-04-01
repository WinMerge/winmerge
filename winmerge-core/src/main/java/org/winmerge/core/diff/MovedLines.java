package org.winmerge.core.diff;

import java.util.HashMap;
import java.util.Map;

public final class MovedLines {
    public enum Side {
        LEFT,
        RIGHT
    }

    private final Map<Integer, Integer> movedLeftToRight;
    private final Map<Integer, Integer> movedRightToLeft;

    public MovedLines() {
        movedLeftToRight = new HashMap<>();
        movedRightToLeft = new HashMap<>();
    }

    public void clear() {
        movedLeftToRight.clear();
        movedRightToLeft.clear();
    }

    public void add(Side firstSide, int firstLine, int secondLine) {
        if (firstSide == Side.LEFT) {
            movedLeftToRight.put(firstLine, secondLine);
        } else {
            movedRightToLeft.put(firstLine, secondLine);
        }
    }

    public int lineInBlock(int line, Side side) {
        if (side == Side.LEFT) {
            return movedLeftToRight.getOrDefault(line, -1);
        }
        return movedRightToLeft.getOrDefault(line, -1);
    }
}
