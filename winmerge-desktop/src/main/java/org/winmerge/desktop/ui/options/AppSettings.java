package org.winmerge.desktop.ui.options;

import javafx.beans.property.BooleanProperty;
import javafx.beans.property.IntegerProperty;
import javafx.beans.property.SimpleBooleanProperty;
import javafx.beans.property.SimpleIntegerProperty;

public final class AppSettings {
    private static final int MIN_EDITOR_TAB_SIZE = 1;
    private static final int MAX_EDITOR_TAB_SIZE = 16;

    private final BooleanProperty automaticRescan = new SimpleBooleanProperty(this, "automaticRescan", true);
    private final IntegerProperty editorTabSize = new SimpleIntegerProperty(this, "editorTabSize", 4);

    public BooleanProperty automaticRescanProperty() {
        return automaticRescan;
    }

    public boolean isAutomaticRescan() {
        return automaticRescan.get();
    }

    public void setAutomaticRescan(boolean automaticRescan) {
        this.automaticRescan.set(automaticRescan);
    }

    public IntegerProperty editorTabSizeProperty() {
        return editorTabSize;
    }

    public int getEditorTabSize() {
        return editorTabSize.get();
    }

    public void setEditorTabSize(int editorTabSize) {
        int clamped = Math.max(MIN_EDITOR_TAB_SIZE, Math.min(MAX_EDITOR_TAB_SIZE, editorTabSize));
        this.editorTabSize.set(clamped);
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
        setAutomaticRescan(other.isAutomaticRescan());
        setEditorTabSize(other.getEditorTabSize());
    }
}
