package org.winmerge.desktop.ui.options;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

class AppSettingsTest {
    @Test
    void copiesAndAppliesValues() {
        AppSettings source = new AppSettings();
        source.setAutomaticRescan(false);
        source.setEditorTabSize(8);

        AppSettings copy = source.copy();
        assertFalse(copy.isAutomaticRescan());
        assertEquals(8, copy.getEditorTabSize());

        source.setAutomaticRescan(true);
        source.setEditorTabSize(2);

        assertFalse(copy.isAutomaticRescan());
        assertEquals(8, copy.getEditorTabSize());

        AppSettings target = new AppSettings();
        target.applyFrom(source);

        assertTrue(target.isAutomaticRescan());
        assertEquals(2, target.getEditorTabSize());
    }

    @Test
    void clampsEditorTabSizeToSafeRange() {
        AppSettings settings = new AppSettings();

        settings.setEditorTabSize(-10);
        assertEquals(1, settings.getEditorTabSize());

        settings.setEditorTabSize(100);
        assertEquals(16, settings.getEditorTabSize());
    }
}
