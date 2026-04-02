package org.winmerge.desktop.ui.dialogs;

public record PatchDialogResult(
    String leftPath,
    String rightPath,
    String resultPath,
    String patchStyle,
    int contextLines,
    boolean copyToClipboard,
    boolean appendToFile,
    boolean openInEditor,
    boolean includeCommandLine
) {
}
