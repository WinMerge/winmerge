package org.winmerge.desktop.ui.dialogs;

import java.util.Objects;

public final class FilterSettingsModel {
    private final FileFiltersDialogModel fileFiltersModel;
    private final LineFiltersDialogModel lineFiltersModel;
    private final SubstitutionFiltersDialogModel substitutionFiltersModel;

    public FilterSettingsModel(
        FileFiltersDialogModel fileFiltersModel,
        LineFiltersDialogModel lineFiltersModel,
        SubstitutionFiltersDialogModel substitutionFiltersModel
    ) {
        this.fileFiltersModel = Objects.requireNonNull(fileFiltersModel, "fileFiltersModel");
        this.lineFiltersModel = Objects.requireNonNull(lineFiltersModel, "lineFiltersModel");
        this.substitutionFiltersModel = Objects.requireNonNull(substitutionFiltersModel, "substitutionFiltersModel");
    }

    public FileFiltersDialogModel fileFiltersModel() {
        return fileFiltersModel;
    }

    public LineFiltersDialogModel lineFiltersModel() {
        return lineFiltersModel;
    }

    public SubstitutionFiltersDialogModel substitutionFiltersModel() {
        return substitutionFiltersModel;
    }
}
