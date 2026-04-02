package org.winmerge.desktop.ui.dialogs;

import java.nio.file.Path;
import java.util.List;
import java.util.Objects;
import java.util.Optional;
import java.util.function.Supplier;

import javafx.stage.Window;

public final class DefaultDialogService implements DialogService {
    private final Supplier<Window> ownerSupplier;

    public DefaultDialogService(Supplier<Window> ownerSupplier) {
        this.ownerSupplier = Objects.requireNonNull(ownerSupplier, "ownerSupplier");
    }

    @Override
    public void showAboutDialog(AboutDialogModel model) {
        AboutDialog dialog = new AboutDialog(ownerSupplier.get(), model);
        dialog.showAndWait();
    }

    @Override
    public Optional<WMGotoDialogResult> showGotoDialog(WMGotoDialogRequest request) {
        WMGotoDialog dialog = new WMGotoDialog(ownerSupplier.get(), request);
        return dialog.showAndWait();
    }

    @Override
    public void showCompareStatisticsDialog(CompareStatisticsDialogModel model) {
        CompareStatisticsDialog dialog = new CompareStatisticsDialog(ownerSupplier.get(), model);
        dialog.showAndWait();
    }

    @Override
    public Optional<String> showComparisonResultFilterDialog(boolean threeWay) {
        ComparisonResultFilterDialog dialog = new ComparisonResultFilterDialog(ownerSupplier.get(), threeWay);
        return dialog.showAndWait();
    }

    @Override
    public void showFilterSettingsDialog(FilterSettingsModel model) {
        FilterSettingsDialog dialog = new FilterSettingsDialog(ownerSupplier.get(), model);
        dialog.showAndWait();
    }

    @Override
    public Optional<FilterConditionResult> showFilterConditionDialog(FilterConditionRequest request) {
        FilterConditionDialog dialog = new FilterConditionDialog(ownerSupplier.get(), request);
        return dialog.showAndWait();
    }

    @Override
    public Optional<SharedFilterDialog.FilterType> showSharedFilterDialog() {
        SharedFilterDialog dialog = new SharedFilterDialog(ownerSupplier.get(), SharedFilterDialog.FilterType.PRIVATE);
        return dialog.showAndWait();
    }

    @Override
    public void showTestFilterDialog(TestFilterModel model) {
        TestFilterDialog dialog = new TestFilterDialog(ownerSupplier.get(), model);
        dialog.showAndWait();
    }

    @Override
    public SaveClosingChoice showSaveClosingDialog(Path filePath) {
        SaveClosingDialog dialog = new SaveClosingDialog(ownerSupplier.get(), filePath);
        return dialog.showAndWait().orElse(SaveClosingChoice.CANCEL);
    }

    @Override
    public List<DirColumn> showDirColumnsDialog(List<DirColumn> columns) {
        DirColumnsDialog dialog = new DirColumnsDialog(ownerSupplier.get(), columns);
        return dialog.showAndWait().orElse(List.copyOf(columns));
    }

    @Override
    public Optional<DirCompareReportResult> showDirCompareReportDialog(DirCompareReportRequest request) {
        DirCompareReportDialog dialog = new DirCompareReportDialog(ownerSupplier.get(), request);
        return dialog.showAndWait();
    }

    @Override
    public Optional<DirSelectFilesResult> showDirSelectFilesDialog(DirSelectFilesRequest request) {
        DirSelectFilesDialog dialog = new DirSelectFilesDialog(ownerSupplier.get(), request);
        return dialog.showAndWait();
    }

    @Override
    public List<String> showDirAdditionalPropertiesDialog(List<DirPropertyNode> nodes) {
        DirAdditionalPropertiesDialog dialog = new DirAdditionalPropertiesDialog(ownerSupplier.get(), nodes);
        return dialog.showAndWait().orElse(List.of());
    }

    @Override
    public ConfirmFolderCopyChoice showConfirmFolderCopyDialog(ConfirmFolderCopyRequest request) {
        ConfirmFolderCopyDialog dialog = new ConfirmFolderCopyDialog(ownerSupplier.get(), request);
        return dialog.showAndWait().orElse(ConfirmFolderCopyChoice.NO);
    }
}
