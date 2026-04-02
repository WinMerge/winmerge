package org.winmerge.desktop.ui.dialogs;

public record DirColumn(
    String name,
    String description,
    int logCol,
    int phyCol
) {
    public DirColumn {
        name = name == null ? "" : name;
        description = description == null ? "" : description;
    }

    public boolean isForcedHidden() {
        return logCol < 0;
    }

    public boolean isVisible() {
        return !isForcedHidden() && phyCol >= 0;
    }
}
