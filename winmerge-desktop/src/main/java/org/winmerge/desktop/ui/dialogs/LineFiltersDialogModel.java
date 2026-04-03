package org.winmerge.desktop.ui.dialogs;

import java.util.Collection;
import java.util.List;

import javafx.beans.property.BooleanProperty;
import javafx.beans.property.SimpleBooleanProperty;
import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringProperty;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;

public final class LineFiltersDialogModel {
    private final ObservableList<LineFilterEntry> entries = FXCollections.observableArrayList();
    private final BooleanProperty ignoreRegExpErrors = new SimpleBooleanProperty(false);

    public LineFiltersDialogModel() {
    }

    public LineFiltersDialogModel(Collection<LineFilterSnapshot> initialEntries, boolean ignoreRegExpErrors) {
        if (initialEntries != null) {
            initialEntries.forEach(entry -> this.entries.add(new LineFilterEntry(entry.pattern(), entry.enabled())));
        }
        this.ignoreRegExpErrors.set(ignoreRegExpErrors);
    }

    public ObservableList<LineFilterEntry> entries() {
        return entries;
    }

    public BooleanProperty ignoreRegExpErrorsProperty() {
        return ignoreRegExpErrors;
    }

    public boolean ignoreRegExpErrors() {
        return ignoreRegExpErrors.get();
    }

    public void setIgnoreRegExpErrors(boolean value) {
        ignoreRegExpErrors.set(value);
    }

    public LineFilterEntry addEntry(String pattern, boolean enabled) {
        LineFilterEntry entry = new LineFilterEntry(pattern, enabled);
        entries.add(entry);
        return entry;
    }

    public void removeEntryAt(int index) {
        if (index < 0 || index >= entries.size()) {
            return;
        }
        entries.remove(index);
    }

    public List<LineFilterSnapshot> snapshot() {
        return entries.stream().map(entry -> new LineFilterSnapshot(entry.getPattern(), entry.isEnabled())).toList();
    }

    public static final class LineFilterEntry {
        private final StringProperty pattern = new SimpleStringProperty("");
        private final BooleanProperty enabled = new SimpleBooleanProperty(false);

        public LineFilterEntry(String pattern, boolean enabled) {
            setPattern(pattern);
            setEnabled(enabled);
        }

        public StringProperty patternProperty() {
            return pattern;
        }

        public String getPattern() {
            return pattern.get();
        }

        public void setPattern(String value) {
            pattern.set(value == null ? "" : value);
        }

        public BooleanProperty enabledProperty() {
            return enabled;
        }

        public boolean isEnabled() {
            return enabled.get();
        }

        public void setEnabled(boolean value) {
            enabled.set(value);
        }
    }

    public record LineFilterSnapshot(String pattern, boolean enabled) {
    }
}
