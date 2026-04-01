package org.winmerge.core.config;

import org.junit.jupiter.api.Test;

import java.util.UUID;
import java.util.prefs.Preferences;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

class ConfigurationStoreTest {
    @Test
    void storesAndRetrievesValues() throws Exception {
        Preferences prefs = Preferences.userRoot().node("org/winmerge/core/tests/" + UUID.randomUUID());
        try {
            PreferencesConfigurationStore store = new PreferencesConfigurationStore(prefs);
            store.put("flag", "true");
            store.put("count", "5");

            assertEquals("true", store.get("flag").orElseThrow());
            assertTrue(store.getBoolean("flag", false));
            assertEquals(5, store.getInt("count", 0));
        } finally {
            prefs.removeNode();
        }
    }
}
