package org.winmerge.desktop.ui.dialogs;

import java.util.List;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNull;
import static org.junit.jupiter.api.Assertions.assertTrue;

class DirSelectFilesDialogLogicTest {
    @Test
    void unavailableButtonShouldBeHidden() {
        // Parity with C++ ShowDlgItem(exists): hide, not just disable.
        assertFalse(DirSelectFilesDialogLogic.buttonVisible(false));
    }

    @Test
    void availableButtonShouldBeVisible() {
        assertTrue(DirSelectFilesDialogLogic.buttonVisible(true));
    }

    @Test
    void resolvePathReturnsNullForUnavailableSlot() {
        DirSelectFilesRequest request = buildRequest(
            new String[]{"a.txt", "b.txt", "c.txt"},
            new boolean[]{true, false, true}
        );
        // Button 1 maps to pane1, option1 (index 1) -> available=false -> null
        assertNull(DirSelectFilesDialogLogic.resolvePath(request, 1));
    }

    @Test
    void resolvePathReturnsPathForAvailableSlot() {
        DirSelectFilesRequest request = buildRequest(
            new String[]{"a.txt", "b.txt", "c.txt"},
            new boolean[]{true, false, true}
        );
        assertEquals("a.txt", DirSelectFilesDialogLogic.resolvePath(request, 0));
    }

    @Test
    void buildResultExcludesUnavailableSelectedButtons() {
        DirSelectFilesRequest request = buildRequest(
            new String[]{"a.txt", "b.txt", "c.txt"},
            new boolean[]{true, false, true}
        );
        // Select buttons 0 (available) and 1 (unavailable)
        DirSelectFilesResult result = DirSelectFilesDialogLogic.buildResult(request, List.of(0, 1));
        // Only button 0 (a.txt) should appear in the result
        assertEquals("a.txt", result.selectedFiles().getAt(0));
    }

    @Test
    void buildResultIncludesOnlyAvailableSelectedPaths() {
        DirSelectFilesRequest request = buildRequest(
            new String[]{"x.txt", "y.txt", "z.txt"},
            new boolean[]{true, true, false}
        );
        // Select buttons 0, 1, 2 - only 0 and 1 are available
        DirSelectFilesResult result = DirSelectFilesDialogLogic.buildResult(request, List.of(0, 1, 2));
        assertEquals("x.txt", result.selectedFiles().getAt(0));
        assertEquals("y.txt", result.selectedFiles().getAt(1));
    }

    private static DirSelectFilesRequest buildRequest(String[] paths, boolean[] available) {
        DirSelectFilesRequest.DirSelectFilesPane pane = new DirSelectFilesRequest.DirSelectFilesPane(
            "Pane", paths, available
        );
        return new DirSelectFilesRequest(pane, pane, null, List.of());
    }
}
