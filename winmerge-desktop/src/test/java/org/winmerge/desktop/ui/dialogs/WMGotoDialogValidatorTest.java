package org.winmerge.desktop.ui.dialogs;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

class WMGotoDialogValidatorTest {
    @Test
    void computesRangeForLineAndDifference() {
        WMGotoDialogRequest request = new WMGotoDialogRequest(
            "12",
            0,
            WMGotoTarget.LINE,
            3,
            new int[] {120, 80, 42},
            17
        );

        assertEquals(120, WMGotoDialogValidator.rangeMax(request, 0, WMGotoTarget.LINE));
        assertEquals(80, WMGotoDialogValidator.rangeMax(request, 1, WMGotoTarget.LINE));
        assertEquals(42, WMGotoDialogValidator.rangeMax(request, 2, WMGotoTarget.LINE));
        assertEquals(17, WMGotoDialogValidator.rangeMax(request, 0, WMGotoTarget.DIFFERENCE));
    }

    @Test
    void validatesPositiveNumericInputWithinRange() {
        WMGotoDialogRequest request = new WMGotoDialogRequest(
            "1",
            0,
            WMGotoTarget.LINE,
            2,
            new int[] {10, 20, 20},
            3
        );

        assertTrue(WMGotoDialogValidator.canSubmit("1", request, 0, WMGotoTarget.LINE));
        assertTrue(WMGotoDialogValidator.canSubmit("10", request, 0, WMGotoTarget.LINE));
        assertFalse(WMGotoDialogValidator.canSubmit("11", request, 0, WMGotoTarget.LINE));
        assertFalse(WMGotoDialogValidator.canSubmit("0", request, 0, WMGotoTarget.LINE));
        assertFalse(WMGotoDialogValidator.canSubmit("-1", request, 0, WMGotoTarget.LINE));
        assertFalse(WMGotoDialogValidator.canSubmit("abc", request, 0, WMGotoTarget.LINE));
        assertFalse(WMGotoDialogValidator.canSubmit("", request, 0, WMGotoTarget.LINE));
    }
}
