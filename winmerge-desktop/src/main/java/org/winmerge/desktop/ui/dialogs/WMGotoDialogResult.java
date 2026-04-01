package org.winmerge.desktop.ui.dialogs;

public record WMGotoDialogResult(
    int fileIndex,
    WMGotoTarget target,
    int value
) {
}
