package org.winmerge.desktop.ui.dialogs;

public record DirCompareReportResult(
    String reportFilePath,
    ReportType reportType,
    boolean copyToClipboard,
    boolean includeFileCompareReport
) {
}
