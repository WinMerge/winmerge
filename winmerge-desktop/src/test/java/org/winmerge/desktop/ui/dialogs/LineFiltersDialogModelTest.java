package org.winmerge.desktop.ui.dialogs;

import java.util.List;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

class LineFiltersDialogModelTest {
    @Test
    void addRemoveAndSnapshotPreserveEnabledFlags() {
        LineFiltersDialogModel model = new LineFiltersDialogModel();

        model.addEntry("^foo$", true);
        model.addEntry("bar", false);
        model.removeEntryAt(1);

        List<LineFiltersDialogModel.LineFilterSnapshot> snapshot = model.snapshot();
        assertEquals(1, snapshot.size());
        assertEquals("^foo$", snapshot.get(0).pattern());
        assertTrue(snapshot.get(0).enabled());
    }

    @Test
    void ignoreRegexpFlagPersists() {
        LineFiltersDialogModel model = new LineFiltersDialogModel();
        model.setIgnoreRegExpErrors(true);

        assertTrue(model.ignoreRegExpErrors());

        model.setIgnoreRegExpErrors(false);
        assertFalse(model.ignoreRegExpErrors());
    }
}
