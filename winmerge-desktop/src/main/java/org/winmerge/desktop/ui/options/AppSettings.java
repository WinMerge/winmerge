package org.winmerge.desktop.ui.options;

import javafx.beans.property.BooleanProperty;
import javafx.beans.property.IntegerProperty;
import javafx.beans.property.SimpleBooleanProperty;
import javafx.beans.property.SimpleIntegerProperty;
import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringProperty;

import java.util.LinkedHashMap;
import java.util.Map;

public final class AppSettings {
    static final String KEY_AUTOMATIC_RESCAN = "options.general.automaticRescan";
    static final String KEY_EDITOR_TAB_SIZE = "options.editor.tabSize";

    private static final int MIN_EDITOR_TAB_SIZE = 1;
    private static final int MAX_EDITOR_TAB_SIZE = 16;

    private final Map<String, BooleanProperty> booleanSettings = new LinkedHashMap<>();
    private final Map<String, IntegerProperty> integerSettings = new LinkedHashMap<>();
    private final Map<String, StringProperty> stringSettings = new LinkedHashMap<>();

    public BooleanProperty automaticRescanProperty() {
        return booleanProperty(KEY_AUTOMATIC_RESCAN, true);
    }

    public boolean isAutomaticRescan() {
        return automaticRescanProperty().get();
    }

    public void setAutomaticRescan(boolean automaticRescan) {
        automaticRescanProperty().set(automaticRescan);
    }

    public IntegerProperty editorTabSizeProperty() {
        return integerProperty(KEY_EDITOR_TAB_SIZE, 4);
    }

    public int getEditorTabSize() {
        return editorTabSizeProperty().get();
    }

    public void setEditorTabSize(int editorTabSize) {
        int clamped = Math.max(MIN_EDITOR_TAB_SIZE, Math.min(MAX_EDITOR_TAB_SIZE, editorTabSize));
        editorTabSizeProperty().set(clamped);
    }

    public BooleanProperty booleanProperty(String key, boolean defaultValue) {
        if (key == null || key.isBlank()) {
            throw new IllegalArgumentException("key must not be blank");
        }
        return booleanSettings.computeIfAbsent(
            key,
            ignored -> new SimpleBooleanProperty(this, key, defaultValue)
        );
    }

    public IntegerProperty integerProperty(String key, int defaultValue) {
        if (key == null || key.isBlank()) {
            throw new IllegalArgumentException("key must not be blank");
        }
        return integerSettings.computeIfAbsent(
            key,
            ignored -> new SimpleIntegerProperty(this, key, defaultValue)
        );
    }

    public StringProperty stringProperty(String key, String defaultValue) {
        if (key == null || key.isBlank()) {
            throw new IllegalArgumentException("key must not be blank");
        }
        return stringSettings.computeIfAbsent(
            key,
            ignored -> new SimpleStringProperty(this, key, defaultValue == null ? "" : defaultValue)
        );
    }

    public boolean getBoolean(String key, boolean defaultValue) {
        return booleanProperty(key, defaultValue).get();
    }

    public void setBoolean(String key, boolean defaultValue, boolean value) {
        booleanProperty(key, defaultValue).set(value);
    }

    public int getInteger(String key, int defaultValue) {
        return integerProperty(key, defaultValue).get();
    }

    public void setInteger(String key, int defaultValue, int value) {
        if (KEY_EDITOR_TAB_SIZE.equals(key)) {
            setEditorTabSize(value);
            return;
        }
        integerProperty(key, defaultValue).set(value);
    }

    public String getString(String key, String defaultValue) {
        return stringProperty(key, defaultValue).get();
    }

    public void setString(String key, String defaultValue, String value) {
        stringProperty(key, defaultValue).set(value == null ? "" : value);
    }

    Map<String, BooleanProperty> booleanSettings() {
        return booleanSettings;
    }

    Map<String, IntegerProperty> integerSettings() {
        return integerSettings;
    }

    Map<String, StringProperty> stringSettings() {
        return stringSettings;
    }

    public AppSettings copy() {
        AppSettings copied = new AppSettings();
        copied.applyFrom(this);
        return copied;
    }

    public void applyFrom(AppSettings other) {
        if (other == null) {
            throw new IllegalArgumentException("other must not be null");
        }
        for (Map.Entry<String, BooleanProperty> entry : other.booleanSettings.entrySet()) {
            setBoolean(entry.getKey(), entry.getValue().get(), entry.getValue().get());
        }
        for (Map.Entry<String, IntegerProperty> entry : other.integerSettings.entrySet()) {
            setInteger(entry.getKey(), entry.getValue().get(), entry.getValue().get());
        }
        for (Map.Entry<String, StringProperty> entry : other.stringSettings.entrySet()) {
            setString(entry.getKey(), entry.getValue().get(), entry.getValue().get());
        }
    }
}
