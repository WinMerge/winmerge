package org.winmerge.core.config;

import java.util.Optional;

public interface ConfigurationStore {
    static ConfigurationStore createDefault() {
        return new PreferencesConfigurationStore();
    }

    Optional<String> get(String key);

    void put(String key, String value);

    default boolean getBoolean(String key, boolean defaultValue) {
        return get(key).map(Boolean::parseBoolean).orElse(defaultValue);
    }

    default int getInt(String key, int defaultValue) {
        return get(key)
            .map(value -> {
                try {
                    return Integer.parseInt(value);
                } catch (NumberFormatException ex) {
                    return defaultValue;
                }
            })
            .orElse(defaultValue);
    }
}
