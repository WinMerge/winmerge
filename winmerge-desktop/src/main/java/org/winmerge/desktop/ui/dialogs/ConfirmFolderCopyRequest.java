package org.winmerge.desktop.ui.dialogs;

public record ConfirmFolderCopyRequest(
    String question,
    String fromText,
    String fromPath,
    String toText,
    String toPath,
    String caption
) {
    public ConfirmFolderCopyRequest {
        question = question == null ? "" : question;
        fromText = fromText == null ? "" : fromText;
        fromPath = fromPath == null ? "" : fromPath;
        toText = toText == null ? "" : toText;
        toPath = toPath == null ? "" : toPath;
        caption = caption == null ? "" : caption;
    }
}
