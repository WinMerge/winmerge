package org.winmerge.desktop.ui;

import java.nio.file.Path;
import java.util.Optional;
import java.util.concurrent.atomic.AtomicReference;

import javafx.scene.paint.Color;
import javafx.scene.text.Font;
import org.junit.jupiter.api.Test;
import org.winmerge.desktop.ui.dialogs.AboutDialogModel;
import org.winmerge.desktop.ui.dialogs.CodepageRequest;
import org.winmerge.desktop.ui.dialogs.CodepageResult;
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
import org.winmerge.desktop.ui.dialogs.MessageBoxRequest;
import org.winmerge.desktop.ui.dialogs.OpenTableRequest;
import org.winmerge.desktop.ui.dialogs.OpenTableResult;
import org.winmerge.desktop.ui.dialogs.PatchDialogRequest;
import org.winmerge.desktop.ui.dialogs.PatchDialogResult;
import org.winmerge.desktop.ui.dialogs.SaveClosingChoice;
import org.winmerge.desktop.ui.dialogs.WMGotoDialogRequest;
import org.winmerge.desktop.ui.dialogs.WMGotoDialogResult;
import org.winmerge.desktop.ui.dialogs.WindowsManagerModel;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

class SaveClosingHandlerTest {
    @Test
    void confirmCloseReturnsTrueWhenTabIsClean() {
        StubDirtyTab tab = new StubDirtyTab(false, true);
        StubDialogService dialogs = new StubDialogService(SaveClosingChoice.CANCEL);
        AtomicReference<String> status = new AtomicReference<>("");

        boolean allowed = SaveClosingHandler.confirmClose(tab, dialogs, status::set);

        assertTrue(allowed);
        assertEquals(0, dialogs.promptCalls);
    }

    @Test
    void confirmCloseSavesWhenRequested() {
        StubDirtyTab tab = new StubDirtyTab(true, true);
        StubDialogService dialogs = new StubDialogService(SaveClosingChoice.SAVE);
        AtomicReference<String> status = new AtomicReference<>("");

        boolean allowed = SaveClosingHandler.confirmClose(tab, dialogs, status::set);

        assertTrue(allowed);
        assertTrue(tab.saveCalled);
        assertEquals(1, dialogs.promptCalls);
        assertTrue(status.get().startsWith("Saved changes to"));
    }

    @Test
    void confirmCloseBlocksCloseWhenSaveFails() {
        StubDirtyTab tab = new StubDirtyTab(true, false);
        StubDialogService dialogs = new StubDialogService(SaveClosingChoice.SAVE);
        AtomicReference<String> status = new AtomicReference<>("");

        boolean allowed = SaveClosingHandler.confirmClose(tab, dialogs, status::set);

        assertFalse(allowed);
        assertTrue(tab.saveCalled);
        assertEquals("Save failed for /tmp/right.txt; close cancelled.", status.get());
    }

    @Test
    void confirmCloseAllowsDiscard() {
        StubDirtyTab tab = new StubDirtyTab(true, false);
        StubDialogService dialogs = new StubDialogService(SaveClosingChoice.DISCARD);
        AtomicReference<String> status = new AtomicReference<>("");

        boolean allowed = SaveClosingHandler.confirmClose(tab, dialogs, status::set);

        assertTrue(allowed);
        assertFalse(tab.saveCalled);
        assertEquals("Discarded unsaved changes for /tmp/right.txt.", status.get());
    }

    @Test
    void confirmCloseCancelsWhenRequested() {
        StubDirtyTab tab = new StubDirtyTab(true, true);
        StubDialogService dialogs = new StubDialogService(SaveClosingChoice.CANCEL);
        AtomicReference<String> status = new AtomicReference<>("");

        boolean allowed = SaveClosingHandler.confirmClose(tab, dialogs, status::set);

        assertFalse(allowed);
        assertFalse(tab.saveCalled);
        assertEquals("Close cancelled.", status.get());
    }

    private static final class StubDirtyTab implements DirtyTab {
        private final boolean dirty;
        private final boolean saveResult;
        private boolean saveCalled;

        private StubDirtyTab(boolean dirty, boolean saveResult) {
            this.dirty = dirty;
            this.saveResult = saveResult;
        }

        @Override
        public boolean hasUnsavedChanges() {
            return dirty;
        }

        @Override
        public Path pathForSavePrompt() {
            return Path.of("/tmp/right.txt");
        }

        @Override
        public boolean saveChanges() {
            saveCalled = true;
            return saveResult;
        }
    }

    private static final class StubDialogService implements DialogService {
        private final SaveClosingChoice choice;
        private int promptCalls;

        private StubDialogService(SaveClosingChoice choice) {
            this.choice = choice;
        }

        @Override
        public void showMessageBox(MessageBoxRequest request) {
        }

        @Override
        public Optional<Color> showColorDialog(Color initialColor) {
            return Optional.empty();
        }

        @Override
        public Optional<Font> showFontDialog(Font initialFont) {
            return Optional.empty();
        }

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
        public SaveClosingChoice showSaveClosingDialog(Path filePath) {
            promptCalls++;
            return choice;
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

        @Override
        public Optional<CodepageResult> showCodepageDialog(CodepageRequest request) {
            return Optional.empty();
        }

        @Override
        public Optional<PatchDialogResult> showPatchDialog(PatchDialogRequest request) {
            return Optional.empty();
        }

        @Override
        public Optional<Integer> showWindowsManagerDialog(WindowsManagerModel model) {
            return Optional.empty();
        }

        @Override
        public Optional<OpenTableResult> showOpenTableDialog(OpenTableRequest request) {
            return Optional.empty();
        }
    }
}
