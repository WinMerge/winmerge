package org.winmerge.desktop.ui.dialogs;

import java.util.Optional;

final class DirCompareReportDialogLogic {
    private DirCompareReportDialogLogic() {
    }

    static boolean canSubmit(String reportFilePath, boolean copyToClipboard) {
        return copyToClipboard || (reportFilePath != null && !reportFilePath.trim().isEmpty());
    }

    static Optional<DirCompareReportResult> buildResult(
        String reportFilePath,
        ReportType reportType,
        boolean copyToClipboard,
        boolean includeFileCompareReport
    ) {
        if (!canSubmit(reportFilePath, copyToClipboard)) {
            return Optional.empty();
        }
        ReportType normalizedReportType = reportType == null ? ReportType.TEXT : reportType;
        boolean includeFileReport = normalizedReportType == ReportType.HTML && includeFileCompareReport;
        return Optional.of(
            new DirCompareReportResult(
                reportFilePath == null ? "" : reportFilePath.trim(),
                normalizedReportType,
                copyToClipboard,
                includeFileReport
            )
        );
    }
}
