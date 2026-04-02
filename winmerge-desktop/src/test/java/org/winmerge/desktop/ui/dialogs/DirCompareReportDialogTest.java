package org.winmerge.desktop.ui.dialogs;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

class DirCompareReportDialogTest {
    @Test
    void buildsResultForEachReportType() {
        DirCompareReportResult text = DirCompareReportDialogLogic
            .buildResult("report.csv", ReportType.TEXT, false, true)
            .orElseThrow();
        DirCompareReportResult html = DirCompareReportDialogLogic
            .buildResult("report.html", ReportType.HTML, false, true)
            .orElseThrow();
        DirCompareReportResult xml = DirCompareReportDialogLogic
            .buildResult("report.xml", ReportType.XML, false, true)
            .orElseThrow();

        assertEquals(ReportType.TEXT, text.reportType());
        assertFalse(text.includeFileCompareReport());

        assertEquals(ReportType.HTML, html.reportType());
        assertTrue(html.includeFileCompareReport());

        assertEquals(ReportType.XML, xml.reportType());
        assertFalse(xml.includeFileCompareReport());
    }

    @Test
    void allowsClipboardOnlyReportsWithoutFilePath() {
        assertTrue(DirCompareReportDialogLogic.canSubmit("", true));
        assertFalse(DirCompareReportDialogLogic.canSubmit("", false));
    }
}
