package org.winmerge.desktop.ui.dialogs;

import java.util.List;

public record WindowsManagerModel(
    List<String> windowTitles,
    int activeIndex
) {
    public WindowsManagerModel {
        windowTitles = windowTitles == null ? List.of() : List.copyOf(windowTitles);
        activeIndex = Math.max(0, Math.min(activeIndex, Math.max(0, windowTitles.size() - 1)));
    }
}
