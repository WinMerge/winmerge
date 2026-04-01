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
        source.setBoolean("options.compare.ignoreCase", false, true);
        source.setInteger("options.compare.diffAlgorithm", 0, 2);
        source.setString("options.compareBinary.filePatterns", "*.bin", "*.bin;*.dat");

        AppSettings copy = source.copy();
        assertFalse(copy.isAutomaticRescan());
        assertEquals(8, copy.getEditorTabSize());
        assertTrue(copy.getBoolean("options.compare.ignoreCase", false));
        assertEquals(2, copy.getInteger("options.compare.diffAlgorithm", 0));
        assertEquals("*.bin;*.dat", copy.getString("options.compareBinary.filePatterns", "*.bin"));

        source.setAutomaticRescan(true);
        source.setEditorTabSize(2);
        source.setBoolean("options.compare.ignoreCase", false, false);

        assertFalse(copy.isAutomaticRescan());
        assertEquals(8, copy.getEditorTabSize());
        assertTrue(copy.getBoolean("options.compare.ignoreCase", false));

        AppSettings target = new AppSettings();
        target.applyFrom(source);

        assertTrue(target.isAutomaticRescan());
        assertEquals(2, target.getEditorTabSize());
        assertFalse(target.getBoolean("options.compare.ignoreCase", false));
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
