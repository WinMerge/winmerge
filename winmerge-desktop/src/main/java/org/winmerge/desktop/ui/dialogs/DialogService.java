package org.winmerge.desktop.ui.dialogs;

import java.nio.file.Path;
import java.util.List;
import java.util.Optional;

public interface DialogService {
    void showAboutDialog(AboutDialogModel model);

    Optional<WMGotoDialogResult> showGotoDialog(WMGotoDialogRequest request);

    void showCompareStatisticsDialog(CompareStatisticsDialogModel model);

    Optional<String> showComparisonResultFilterDialog(boolean threeWay);

    void showFilterSettingsDialog(FilterSettingsModel model);

    Optional<FilterConditionResult> showFilterConditionDialog(FilterConditionRequest request);

    Optional<SharedFilterDialog.FilterType> showSharedFilterDialog();

    void showTestFilterDialog(TestFilterModel model);

    SaveClosingChoice showSaveClosingDialog(Path filePath);

    List<DirColumn> showDirColumnsDialog(List<DirColumn> columns);

    Optional<DirCompareReportResult> showDirCompareReportDialog(DirCompareReportRequest request);

    Optional<DirSelectFilesResult> showDirSelectFilesDialog(DirSelectFilesRequest request);

    List<String> showDirAdditionalPropertiesDialog(List<DirPropertyNode> nodes);

    ConfirmFolderCopyChoice showConfirmFolderCopyDialog(ConfirmFolderCopyRequest request);
}
