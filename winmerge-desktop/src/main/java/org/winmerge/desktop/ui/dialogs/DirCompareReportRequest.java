package org.winmerge.desktop.ui.dialogs;

public record DirCompareReportRequest(
    String reportFilePath,
    ReportType reportType,
    boolean copyToClipboard,
    boolean includeFileCompareReport
) {
    public DirCompareReportRequest {
        reportFilePath = reportFilePath == null ? "" : reportFilePath;
        reportType = reportType == null ? ReportType.TEXT : reportType;
    }
}
