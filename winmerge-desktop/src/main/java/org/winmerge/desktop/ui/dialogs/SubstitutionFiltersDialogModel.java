package org.winmerge.desktop.ui.dialogs;

import java.util.Collection;
import java.util.List;

import javafx.beans.property.BooleanProperty;
import javafx.beans.property.SimpleBooleanProperty;
import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringProperty;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;

public final class SubstitutionFiltersDialogModel {
    private final ObservableList<SubstitutionFilterEntry> entries = FXCollections.observableArrayList();
    private final BooleanProperty enabled = new SimpleBooleanProperty(false);

    public SubstitutionFiltersDialogModel() {
    }

    public SubstitutionFiltersDialogModel(Collection<SubstitutionFilterSnapshot> initialEntries, boolean enabled) {
        if (initialEntries != null) {
            initialEntries.forEach(entry -> this.entries.add(
                new SubstitutionFilterEntry(
                    entry.findWhat(),
                    entry.replaceWith(),
                    entry.useRegExp(),
                    entry.caseSensitive(),
                    entry.matchWholeWordOnly(),
                    entry.enabled()
                )
            ));
        }
        this.enabled.set(enabled);
    }

    public ObservableList<SubstitutionFilterEntry> entries() {
        return entries;
    }

    public BooleanProperty enabledProperty() {
        return enabled;
    }

    public boolean enabled() {
        return enabled.get();
    }

    public void setEnabled(boolean value) {
        enabled.set(value);
    }

    public SubstitutionFilterEntry addEntry(
        String findWhat,
        String replaceWith,
        boolean useRegExp,
        boolean caseSensitive,
        boolean matchWholeWordOnly,
        boolean enabled
    ) {
        SubstitutionFilterEntry entry = new SubstitutionFilterEntry(
            findWhat,
            replaceWith,
            useRegExp,
            caseSensitive,
            matchWholeWordOnly,
            enabled
        );
        entries.add(entry);
        return entry;
    }

    public void removeEntryAt(int index) {
        if (index < 0 || index >= entries.size()) {
            return;
        }
        entries.remove(index);
    }

    public void clearEntries() {
        entries.clear();
    }

    public List<SubstitutionFilterSnapshot> snapshot() {
        return entries.stream().map(entry -> new SubstitutionFilterSnapshot(
            entry.getFindWhat(),
            entry.getReplaceWith(),
            entry.isUseRegExp(),
            entry.isCaseSensitive(),
            entry.isMatchWholeWordOnly(),
            entry.isEnabled()
        )).toList();
    }

    public static final class SubstitutionFilterEntry {
        private final StringProperty findWhat = new SimpleStringProperty("");
        private final StringProperty replaceWith = new SimpleStringProperty("");
        private final BooleanProperty useRegExp = new SimpleBooleanProperty(false);
        private final BooleanProperty caseSensitive = new SimpleBooleanProperty(false);
        private final BooleanProperty matchWholeWordOnly = new SimpleBooleanProperty(false);
        private final BooleanProperty enabled = new SimpleBooleanProperty(true);

        public SubstitutionFilterEntry(
            String findWhat,
            String replaceWith,
            boolean useRegExp,
            boolean caseSensitive,
            boolean matchWholeWordOnly,
            boolean enabled
        ) {
            setFindWhat(findWhat);
            setReplaceWith(replaceWith);
            setUseRegExp(useRegExp);
            setCaseSensitive(caseSensitive);
            setMatchWholeWordOnly(matchWholeWordOnly);
            setEnabled(enabled);
        }

        public StringProperty findWhatProperty() {
            return findWhat;
        }

        public String getFindWhat() {
            return findWhat.get();
        }

        public void setFindWhat(String value) {
            findWhat.set(value == null ? "" : value);
        }

        public StringProperty replaceWithProperty() {
            return replaceWith;
        }

        public String getReplaceWith() {
            return replaceWith.get();
        }

        public void setReplaceWith(String value) {
            replaceWith.set(value == null ? "" : value);
        }

        public BooleanProperty useRegExpProperty() {
            return useRegExp;
        }

        public boolean isUseRegExp() {
            return useRegExp.get();
        }

        public void setUseRegExp(boolean value) {
            useRegExp.set(value);
            if (value) {
                setMatchWholeWordOnly(false);
            }
        }

        public BooleanProperty caseSensitiveProperty() {
            return caseSensitive;
        }

        public boolean isCaseSensitive() {
            return caseSensitive.get();
        }

        public void setCaseSensitive(boolean value) {
            caseSensitive.set(value);
        }

        public BooleanProperty matchWholeWordOnlyProperty() {
            return matchWholeWordOnly;
        }

        public boolean isMatchWholeWordOnly() {
            return matchWholeWordOnly.get();
        }

        public void setMatchWholeWordOnly(boolean value) {
            matchWholeWordOnly.set(value);
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

    public record SubstitutionFilterSnapshot(
        String findWhat,
        String replaceWith,
        boolean useRegExp,
        boolean caseSensitive,
        boolean matchWholeWordOnly,
        boolean enabled
    ) {
    }
}
