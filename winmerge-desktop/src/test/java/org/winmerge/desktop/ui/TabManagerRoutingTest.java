package org.winmerge.desktop.ui;

import java.io.RandomAccessFile;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Optional;

import javafx.scene.control.TabPane;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.io.TempDir;
import org.winmerge.desktop.ui.dialogs.AboutDialogModel;
import org.winmerge.desktop.ui.dialogs.CompareStatisticsDialogModel;
import org.winmerge.desktop.ui.dialogs.ConfirmFolderCopyChoice;
import org.winmerge.desktop.ui.dialogs.ConfirmFolderCopyRequest;
import org.winmerge.desktop.ui.dialogs.DirColumn;
import org.winmerge.desktop.ui.dialogs.DirCompareReportRequest;
import org.winmerge.desktop.ui.dialogs.DirCompareReportResult;
import org.winmerge.desktop.ui.dialogs.DirPropertyNode;
import org.winmerge.desktop.ui.dialogs.DirSelectFilesRequest;
import org.winmerge.desktop.ui.dialogs.DirSelectFilesResult;
import org.winmerge.desktop.ui.dialogs.DialogService;
import org.winmerge.desktop.ui.dialogs.FilterConditionRequest;
import org.winmerge.desktop.ui.dialogs.FilterConditionResult;
import org.winmerge.desktop.ui.dialogs.FilterSettingsModel;
import org.winmerge.desktop.ui.dialogs.SaveClosingChoice;
import org.winmerge.desktop.ui.dialogs.SharedFilterDialog;
import org.winmerge.desktop.ui.dialogs.TestFilterModel;
import org.winmerge.desktop.ui.dialogs.WMGotoDialogRequest;
import org.winmerge.desktop.ui.dialogs.WMGotoDialogResult;
import org.winmerge.desktop.ui.merge.MergeDocModel;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

class TabManagerRoutingTest {
    @TempDir
    Path tempDir;

    @Test
    void routesUtf16BomFilesToTextDiffInsteadOfHex() throws Exception {
        Path left = tempDir.resolve("left-utf16le.txt");
        Path right = tempDir.resolve("right-utf16le.txt");
        writeUtf16LeWithBom(left, "alpha\r\nbeta\r\n");
        writeUtf16LeWithBom(right, "alpha\r\ngamma\r\n");

        assertFalse(TabManager.shouldOpenHexView(left, right));
    }

    @Test
    void routesOversizedFilesToHexDiffForBoundedTextCompare() throws Exception {
        Path oversized = tempDir.resolve("oversized.txt");
        Path right = tempDir.resolve("right.txt");
        setLength(oversized, MergeDocModel.textCompareSizeLimitBytes() + 1);
        Files.writeString(right, "small\n", StandardCharsets.UTF_8);

        assertTrue(TabManager.shouldOpenHexView(oversized, right));
    }

    @Test
    void rejectsThreeWayComparisonRequestsUntilImplemented() throws Exception {
        FxThreadTestSupport.initializeToolkit();
        TabManager tabManager = FxThreadTestSupport.callOnFxThread(() -> new TabManager(new TabPane(), new NoOpDialogService()));

        Path left = tempDir.resolve("left.txt");
        Path right = tempDir.resolve("right.txt");
        Path middle = tempDir.resolve("middle.txt");
        Files.writeString(left, "left\n", StandardCharsets.UTF_8);
        Files.writeString(right, "right\n", StandardCharsets.UTF_8);
        Files.writeString(middle, "middle\n", StandardCharsets.UTF_8);

        TabManager.ComparisonRequest request = new TabManager.ComparisonRequest(
            left,
            right,
            Optional.of(middle),
            TabManager.CompareTarget.FILES,
            "*.*"
        );

        assertThrows(UnsupportedOperationException.class, () -> tabManager.openComparison(request, null));
    }

    private static void writeUtf16LeWithBom(Path path, String text) throws Exception {
        byte[] payload = text.getBytes(StandardCharsets.UTF_16LE);
        byte[] bytes = new byte[payload.length + 2];
        bytes[0] = (byte) 0xFF;
        bytes[1] = (byte) 0xFE;
        System.arraycopy(payload, 0, bytes, 2, payload.length);
        Files.write(path, bytes);
    }

    private static void setLength(Path path, long length) throws Exception {
        try (RandomAccessFile file = new RandomAccessFile(path.toFile(), "rw")) {
            file.setLength(length);
        }
    }

    private static final class NoOpDialogService implements DialogService {
        @Override
        public void showAboutDialog(AboutDialogModel model) {
        }

        @Override
        public Optional<WMGotoDialogResult> showGotoDialog(WMGotoDialogRequest request) {
            return Optional.empty();
        }

        @Override
        public void showCompareStatisticsDialog(CompareStatisticsDialogModel model) {
        }

        @Override
        public Optional<String> showComparisonResultFilterDialog(boolean threeWay) {
            return Optional.empty();
        }

        @Override
        public void showFilterSettingsDialog(FilterSettingsModel model) {
        }

        @Override
        public Optional<FilterConditionResult> showFilterConditionDialog(FilterConditionRequest request) {
            return Optional.empty();
        }

        @Override
        public Optional<SharedFilterDialog.FilterType> showSharedFilterDialog() {
            return Optional.empty();
        }

        @Override
        public void showTestFilterDialog(TestFilterModel model) {
        }

        @Override
        public SaveClosingChoice showSaveClosingDialog(Path filePath) {
            return SaveClosingChoice.CANCEL;
        }

        @Override
        public java.util.List<DirColumn> showDirColumnsDialog(java.util.List<DirColumn> columns) {
            return columns;
        }

        @Override
        public Optional<DirCompareReportResult> showDirCompareReportDialog(DirCompareReportRequest request) {
            return Optional.empty();
        }

        @Override
        public Optional<DirSelectFilesResult> showDirSelectFilesDialog(DirSelectFilesRequest request) {
            return Optional.empty();
        }

        @Override
        public java.util.List<String> showDirAdditionalPropertiesDialog(java.util.List<DirPropertyNode> nodes) {
            return java.util.List.of();
        }

        @Override
        public ConfirmFolderCopyChoice showConfirmFolderCopyDialog(ConfirmFolderCopyRequest request) {
            return ConfirmFolderCopyChoice.NO;
        }
    }
}
