package org.winmerge.desktop.ui.options;

import java.util.HashMap;
import java.util.Map;
import java.util.Optional;

import javafx.scene.control.ButtonType;
import org.junit.jupiter.api.Test;
import org.winmerge.core.config.ConfigurationStore;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

class OptionsDialogSessionTest {
    @Test
    void applyPreviewAndCancelRestoresBaseline() {
        AppSettingsStore store = new AppSettingsStore(new InMemoryConfigurationStore());
        AppSettings appSettings = store.load();

        OptionsDialogSession session = new OptionsDialogSession(appSettings, store);
        session.draft().setEditorTabSize(8);
        session.draft().setBoolean("options.compare.ignoreCase", false, true);

        session.applyPreview();
        assertEquals(8, appSettings.getEditorTabSize());
        assertTrue(appSettings.getBoolean("options.compare.ignoreCase", false));

        boolean saved = session.finish(Optional.of(ButtonType.CANCEL));
        assertFalse(saved);
        assertEquals(4, appSettings.getEditorTabSize());
        assertFalse(appSettings.getBoolean("options.compare.ignoreCase", false));
    }

    @Test
    void okPersistsSettingsAndSurvivesReload() {
        InMemoryConfigurationStore backingStore = new InMemoryConfigurationStore();
        AppSettingsStore store = new AppSettingsStore(backingStore);
        AppSettings appSettings = store.load();

        OptionsDialogSession session = new OptionsDialogSession(appSettings, store);
        session.draft().setEditorTabSize(6);
        session.draft().setBoolean("options.compare.ignoreCase", false, true);

        boolean saved = session.finish(Optional.of(ButtonType.OK));
        assertTrue(saved);

        AppSettings reloaded = store.load();
        assertEquals(6, reloaded.getEditorTabSize());
        assertTrue(reloaded.getBoolean("options.compare.ignoreCase", false));
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
