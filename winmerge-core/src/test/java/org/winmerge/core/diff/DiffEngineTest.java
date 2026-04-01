package org.winmerge.core.diff;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

class DiffEngineTest {
    @Test
    void mergeStatusPropagatesFlagsAndMissingNl() {
        DiffEngine.DiffStatus left = new DiffEngine.DiffStatus();
        left.setBinaries(false);
        left.setPatchFileFailed(false);
        left.setIdentical(DiffEngine.IdentLevel.ALL);
        left.missingNl()[0] = false;
        left.missingNl()[1] = false;
        left.missingNl()[2] = false;

        DiffEngine.DiffStatus right = new DiffEngine.DiffStatus();
        right.setBinaries(true);
        right.setPatchFileFailed(true);
        right.setIdentical(DiffEngine.IdentLevel.EXCEPT_RIGHT);
        right.missingNl()[0] = true;
        right.missingNl()[1] = false;
        right.missingNl()[2] = true;

        left.mergeStatus(right);

        assertTrue(left.binaries());
        assertTrue(left.patchFileFailed());
        assertEquals(DiffEngine.IdentLevel.EXCEPT_RIGHT, left.identical());
        assertTrue(left.missingNl()[0]);
        assertFalse(left.missingNl()[1]);
        assertTrue(left.missingNl()[2]);
    }

    @Test
    void mergeStatusResetsMismatchedExceptStateToNone() {
        DiffEngine.DiffStatus left = new DiffEngine.DiffStatus();
        left.setIdentical(DiffEngine.IdentLevel.EXCEPT_LEFT);

        DiffEngine.DiffStatus right = new DiffEngine.DiffStatus();
        right.setIdentical(DiffEngine.IdentLevel.EXCEPT_RIGHT);

        left.mergeStatus(right);

        assertEquals(DiffEngine.IdentLevel.NONE, left.identical());
    }
}
