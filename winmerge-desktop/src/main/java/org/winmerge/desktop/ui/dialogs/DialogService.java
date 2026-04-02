package org.winmerge.desktop.ui.dialogs;

import java.nio.file.Path;
import java.util.List;
import java.util.Optional;

import javafx.scene.paint.Color;
import javafx.scene.text.Font;

public interface DialogService {
    /**
     * CMessageBoxDialog mapping:
     * C++ modal message boxes are represented directly by JavaFX Alert.
     */
    void showMessageBox(MessageBoxRequest request);

    /**
     * CMyColorDialog mapping:
     * Uses a thin Dialog<Color> wrapper around the JavaFX ColorPicker control.
     */
    Optional<Color> showColorDialog(Color initialColor);

    /**
     * CMyFontDialog mapping:
     * Uses ControlsFX when present, otherwise falls back to a minimal JavaFX chooser.
     */
    Optional<Font> showFontDialog(Font initialFont);

    void showAboutDialog(AboutDialogModel model);

    Optional<WMGotoDialogResult> showGotoDialog(WMGotoDialogRequest request);

    void showCompareStatisticsDialog(CompareStatisticsDialogModel model);

    Optional<String> showComparisonResultFilterDialog(boolean threeWay);

    SaveClosingChoice showSaveClosingDialog(Path filePath);

    List<DirColumn> showDirColumnsDialog(List<DirColumn> columns);

    Optional<DirCompareReportResult> showDirCompareReportDialog(DirCompareReportRequest request);

    Optional<DirSelectFilesResult> showDirSelectFilesDialog(DirSelectFilesRequest request);

    List<String> showDirAdditionalPropertiesDialog(List<DirPropertyNode> nodes);

    ConfirmFolderCopyChoice showConfirmFolderCopyDialog(ConfirmFolderCopyRequest request);

    Optional<CodepageResult> showCodepageDialog(CodepageRequest request);

    Optional<PatchDialogResult> showPatchDialog(PatchDialogRequest request);

    Optional<Integer> showWindowsManagerDialog(WindowsManagerModel model);

    Optional<OpenTableResult> showOpenTableDialog(OpenTableRequest request);
}
