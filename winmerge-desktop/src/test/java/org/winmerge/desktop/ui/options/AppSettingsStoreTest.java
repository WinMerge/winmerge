package org.winmerge.desktop.ui.options;

import java.util.HashMap;
import java.util.Map;
import java.util.Optional;

import org.junit.jupiter.api.Test;
import org.winmerge.core.config.ConfigurationStore;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

class AppSettingsStoreTest {
    @Test
    void loadsDefaultsWhenStoreIsEmpty() {
        AppSettingsStore store = new AppSettingsStore(new InMemoryConfigurationStore());

        AppSettings settings = store.load();

        assertTrue(settings.isAutomaticRescan());
        assertEquals(4, settings.getEditorTabSize());
    }

    @Test
    void savesAndReloadsSettings() {
        InMemoryConfigurationStore backingStore = new InMemoryConfigurationStore();
        AppSettingsStore store = new AppSettingsStore(backingStore);

        AppSettings input = new AppSettings();
        input.setAutomaticRescan(false);
        input.setEditorTabSize(6);
        store.save(input);

        AppSettings loaded = store.load();
        assertFalse(loaded.isAutomaticRescan());
        assertEquals(6, loaded.getEditorTabSize());
    }

    private static final class InMemoryConfigurationStore implements ConfigurationStore {
        private final Map<String, String> values = new HashMap<>();

        @Override
        public Optional<String> get(String key) {
            return Optional.ofNullable(values.get(key));
        }

        @Override
        public void put(String key, String value) {
            values.put(key, value);
        }
    }
}
