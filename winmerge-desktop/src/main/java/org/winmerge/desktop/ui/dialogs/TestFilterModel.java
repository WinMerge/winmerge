package org.winmerge.desktop.ui.dialogs;

import java.util.Objects;
import java.util.function.Predicate;

public final class TestFilterModel {
    private String filterName;
    private Predicate<String> includeFilePredicate;
    private Predicate<String> includeDirectoryPredicate;

    public TestFilterModel(String filterName, Predicate<String> includeFilePredicate, Predicate<String> includeDirectoryPredicate) {
        updateFilterContext(filterName, includeFilePredicate, includeDirectoryPredicate);
    }

    public synchronized void updateFilterContext(
        String filterName,
        Predicate<String> includeFilePredicate,
        Predicate<String> includeDirectoryPredicate
    ) {
        this.filterName = filterName == null ? "" : filterName;
        this.includeFilePredicate = Objects.requireNonNull(includeFilePredicate, "includeFilePredicate");
        this.includeDirectoryPredicate = Objects.requireNonNull(includeDirectoryPredicate, "includeDirectoryPredicate");
    }

    public synchronized String filterName() {
        return filterName;
    }

    public synchronized void setFilterName(String filterName) {
        this.filterName = filterName == null ? "" : filterName;
    }

    public synchronized boolean testInput(String input, boolean directory) {
        String value = input == null ? "" : input;
        if (directory) {
            return includeDirectoryPredicate.test(value.replace('/', '\\'));
        }
        return includeFilePredicate.test(value);
    }
}
