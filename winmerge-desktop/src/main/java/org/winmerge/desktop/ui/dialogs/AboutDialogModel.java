package org.winmerge.desktop.ui.dialogs;

import java.nio.file.Path;
import java.util.Objects;

public record AboutDialogModel(
    String appName,
    String versionText,
    String websiteLabel,
    String websiteUrl,
    String summaryText,
    String asciiArt,
    Path contributorsPath
) {
    public AboutDialogModel {
        appName = Objects.requireNonNullElse(appName, "WinMerge");
        versionText = Objects.requireNonNullElse(versionText, "");
        websiteLabel = Objects.requireNonNullElse(websiteLabel, "");
        websiteUrl = Objects.requireNonNullElse(websiteUrl, "");
        summaryText = Objects.requireNonNullElse(summaryText, "");
        asciiArt = Objects.requireNonNullElse(asciiArt, "");
        contributorsPath = Objects.requireNonNull(contributorsPath, "contributorsPath");
    }
}
