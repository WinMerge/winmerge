package org.winmerge.desktop.ui.dialogs;

import java.util.List;

import org.winmerge.core.io.PathContext;

public record DirSelectFilesResult(
    PathContext selectedFiles,
    List<Integer> selectedButtons
) {
    public DirSelectFilesResult {
        selectedButtons = selectedButtons == null ? List.of() : List.copyOf(selectedButtons);
    }
}
