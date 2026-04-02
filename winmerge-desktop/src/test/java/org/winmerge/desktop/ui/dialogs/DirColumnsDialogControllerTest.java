package org.winmerge.desktop.ui.dialogs;

import java.util.List;
import java.util.Set;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;

class DirColumnsDialogControllerTest {
    @Test
    void forcedHiddenColumnsRemainLastAfterReorderAttempt() {
        List<DirColumn> columns = List.of(
            new DirColumn("Filename", "Name", 0, 0),
            new DirColumn("Path", "Path", 1, 1),
            new DirColumn("Hidden", "Hidden", -1, -1)
        );

        List<DirColumn> sorted = DirColumnsDialogLogic.sortForDisplay(columns);
        List<DirColumn> reordered = DirColumnsDialogLogic.move(sorted, 2, 0);
        List<DirColumn> result = DirColumnsDialogLogic.buildResult(reordered, Set.of(0, 1, 2));

        assertEquals("Filename", result.get(0).name());
        assertEquals("Path", result.get(1).name());
        assertEquals("Hidden", result.get(2).name());
        assertEquals(-1, result.get(2).logCol());
    }

    @Test
    void uncheckedColumnsAreMovedToHiddenSection() {
        List<DirColumn> columns = List.of(
            new DirColumn("Filename", "Name", 0, 0),
            new DirColumn("Path", "Path", 1, 1),
            new DirColumn("Extension", "Ext", 2, 2)
        );

        List<DirColumn> result = DirColumnsDialogLogic.buildResult(columns, Set.of(0));

        assertEquals(0, result.get(0).logCol());
        assertEquals(-1, result.get(1).logCol());
        assertEquals(-1, result.get(2).logCol());
    }
}
