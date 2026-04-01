package org.winmerge.core.config;

import java.util.Optional;
import java.util.prefs.Preferences;

public final class PreferencesConfigurationStore implements ConfigurationStore {
    private final Preferences preferences;

    public PreferencesConfigurationStore() {
        this(Preferences.userRoot().node("org/winmerge/core"));
    }

    public PreferencesConfigurationStore(Preferences preferences) {
        this.preferences = preferences;
    }

    @Override
    public Optional<String> get(String key) {
        return Optional.ofNullable(preferences.get(key, null));
    }

    @Override
    public void put(String key, String value) {
        preferences.put(key, value);
    }
}
