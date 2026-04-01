package org.winmerge.desktop.ui.options;

import java.util.Objects;

import org.winmerge.core.config.ConfigurationStore;

public final class AppSettingsStore {
    private static final String KEY_AUTOMATIC_RESCAN = "options.general.automaticRescan";
    private static final String KEY_EDITOR_TAB_SIZE = "options.editor.tabSize";

    private final ConfigurationStore configurationStore;

    public AppSettingsStore(ConfigurationStore configurationStore) {
        this.configurationStore = Objects.requireNonNull(configurationStore, "configurationStore");
    }

    public AppSettings load() {
        AppSettings settings = new AppSettings();
        settings.setAutomaticRescan(configurationStore.getBoolean(KEY_AUTOMATIC_RESCAN, true));
        settings.setEditorTabSize(configurationStore.getInt(KEY_EDITOR_TAB_SIZE, 4));
        return settings;
    }

    public void save(AppSettings settings) {
        Objects.requireNonNull(settings, "settings");
        configurationStore.put(KEY_AUTOMATIC_RESCAN, Boolean.toString(settings.isAutomaticRescan()));
        configurationStore.put(KEY_EDITOR_TAB_SIZE, Integer.toString(settings.getEditorTabSize()));
    }
}
