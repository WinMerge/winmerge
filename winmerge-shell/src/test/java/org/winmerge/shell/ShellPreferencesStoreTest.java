package org.winmerge.shell;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.UUID;
import java.util.prefs.BackingStoreException;
import java.util.prefs.Preferences;

import org.junit.jupiter.api.Test;

class ShellPreferencesStoreTest {
    @Test
    void loadsAndSavesShellSettingsRoundTrip() throws BackingStoreException {
        Preferences node = Preferences.userRoot().node("org/winmerge/shell/tests/" + UUID.randomUUID());
        try {
            ShellPreferencesStore store = new ShellPreferencesStore(node);
            ShellIntegrationSettings settings = new ShellIntegrationSettings(true, true, false, "newText,options");

            store.save(settings);
            ShellIntegrationSettings loaded = store.load();

            assertTrue(loaded.contextMenuEnabled());
            assertTrue(loaded.advancedContextMenu());
            assertFalse(loaded.compareAsSubmenu());
            assertEquals("newText,options", loaded.jumpListTasks());
        } finally {
            node.removeNode();
        }
    }

    @Test
    void normalizesExtensionKeysForFileAssociations() throws BackingStoreException {
        Preferences node = Preferences.userRoot().node("org/winmerge/shell/tests/" + UUID.randomUUID());
        try {
            ShellPreferencesStore store = new ShellPreferencesStore(node);

            store.putFileAssociation("TXT", "text/plain");

            assertEquals("text/plain", store.getFileAssociation(".txt").orElseThrow());
            assertEquals("text/plain", store.getFileAssociation("txt").orElseThrow());
        } finally {
            node.removeNode();
        }
    }
}
