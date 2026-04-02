package org.winmerge.desktop.ui.dialogs;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

class PatchDialogTest {
    @Test
    void rejectsEmptyResultPathAndInvalidContextLines() {
        assertFalse(PatchDialogLogic.canSubmit("", "3"));
        assertFalse(PatchDialogLogic.canSubmit("report.patch", "-1"));
        assertFalse(PatchDialogLogic.canSubmit("report.patch", "abc"));
        assertTrue(PatchDialogLogic.canSubmit("report.patch", "3"));
    }

    @Test
    void buildsResultForValidInputs() {
        PatchDialogResult result = PatchDialogLogic
            .buildResult(
                "left.txt",
                "right.txt",
                "result.patch",
                "Unified",
                "5",
                true,
                false,
                true,
                true
            )
            .orElseThrow();

        assertEquals("left.txt", result.leftPath());
        assertEquals("right.txt", result.rightPath());
        assertEquals("result.patch", result.resultPath());
        assertEquals("Unified", result.patchStyle());
        assertEquals(5, result.contextLines());
        assertTrue(result.copyToClipboard());
        assertFalse(result.appendToFile());
        assertTrue(result.openInEditor());
        assertTrue(result.includeCommandLine());
    }
}
