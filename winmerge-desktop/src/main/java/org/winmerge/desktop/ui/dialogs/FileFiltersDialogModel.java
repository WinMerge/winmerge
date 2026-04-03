package org.winmerge.desktop.ui.dialogs;

import java.util.Collection;
import java.util.List;
import java.util.Objects;

import javafx.beans.property.BooleanProperty;
import javafx.beans.property.SimpleBooleanProperty;
import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringProperty;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;

public final class FileFiltersDialogModel {
    @FunctionalInterface
    public interface DeleteHandler {
        boolean delete(FileFilterEntry entry);
    }

    @FunctionalInterface
    public interface DirWatcherHandle {
        void cancel();
    }

    private final ObservableList<FileFilterEntry> filters = FXCollections.observableArrayList();
    private final StringProperty maskExpression = new SimpleStringProperty("*.*");
    private DeleteHandler deleteHandler = entry -> true;
    private DirWatcherHandle dirWatcher;
    private TestFilterModel testFilterModel = new TestFilterModel("", value -> true, value -> true);

    public FileFiltersDialogModel() {
    }

    public FileFiltersDialogModel(Collection<FileFilterSnapshot> initialFilters, String maskExpression) {
        if (initialFilters != null) {
            initialFilters.forEach(entry -> filters.add(new FileFilterEntry(
                entry.name(),
                entry.description(),
                entry.location(),
                entry.enabled()
            )));
        }
        setMaskExpression(maskExpression);
    }

    public ObservableList<FileFilterEntry> filters() {
        return filters;
    }

    public StringProperty maskExpressionProperty() {
        return maskExpression;
    }

    public String getMaskExpression() {
        return maskExpression.get();
    }

    public void setMaskExpression(String value) {
        String normalized = value == null ? "" : value;
        if (normalized.isBlank()) {
            normalized = "*.*";
        }
        maskExpression.set(normalized);
    }

    public static boolean isMaskValid(String value) {
        if (value == null || value.trim().isEmpty()) {
            return false;
        }
        String trimmed = value.trim();
        return !"|".equals(trimmed);
    }

    public void setDeleteHandler(DeleteHandler deleteHandler) {
        this.deleteHandler = Objects.requireNonNull(deleteHandler, "deleteHandler");
    }

    public FileFilterEntry addFilter(String name, String description, String location, boolean enabled) {
        FileFilterEntry entry = new FileFilterEntry(name, description, location, enabled);
        filters.add(entry);
        return entry;
    }

    public boolean deleteFilterAt(int index) {
        if (index < 0 || index >= filters.size()) {
            return false;
        }
        FileFilterEntry entry = filters.get(index);
        if (!deleteHandler.delete(entry)) {
            return false;
        }
        filters.remove(index);
        return true;
    }

    public void setDirWatcher(DirWatcherHandle watcher) {
        dirWatcher = watcher;
    }

    public void cancelDirWatcher() {
        if (dirWatcher == null) {
            return;
        }
        dirWatcher.cancel();
        dirWatcher = null;
    }

    public TestFilterModel testFilterModel() {
        return testFilterModel;
    }

    public void setTestFilterModel(TestFilterModel model) {
        testFilterModel = Objects.requireNonNull(model, "model");
    }

    public List<FileFilterSnapshot> snapshot() {
        return filters.stream().map(entry -> new FileFilterSnapshot(
            entry.getName(),
            entry.getDescription(),
            entry.getLocation(),
            entry.isEnabled()
        )).toList();
    }

    public static final class FileFilterEntry {
        private final StringProperty name = new SimpleStringProperty("");
        private final StringProperty description = new SimpleStringProperty("");
        private final StringProperty location = new SimpleStringProperty("");
        private final BooleanProperty enabled = new SimpleBooleanProperty(true);

        public FileFilterEntry(String name, String description, String location, boolean enabled) {
            setName(name);
            setDescription(description);
            setLocation(location);
            setEnabled(enabled);
        }

        public StringProperty nameProperty() {
            return name;
        }

        public String getName() {
            return name.get();
        }

        public void setName(String value) {
            name.set(value == null ? "" : value);
        }

        public StringProperty descriptionProperty() {
            return description;
        }

        public String getDescription() {
            return description.get();
        }

        public void setDescription(String value) {
            description.set(value == null ? "" : value);
        }

        public StringProperty locationProperty() {
            return location;
        }

        public String getLocation() {
            return location.get();
        }

        public void setLocation(String value) {
            location.set(value == null ? "" : value);
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

    public record FileFilterSnapshot(String name, String description, String location, boolean enabled) {
    }
}
