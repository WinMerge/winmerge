package org.winmerge.desktop.ui.dialogs;

import java.nio.file.Path;
import java.util.List;
import java.util.Objects;
import java.util.Optional;
import java.util.function.Supplier;

import javafx.scene.control.Alert;
import javafx.scene.paint.Color;
import javafx.scene.text.Font;
import javafx.stage.Window;

public final class DefaultDialogService implements DialogService {
    private final Supplier<Window> ownerSupplier;

    public DefaultDialogService(Supplier<Window> ownerSupplier) {
        this.ownerSupplier = Objects.requireNonNull(ownerSupplier, "ownerSupplier");
    }

    @Override
    public void showMessageBox(MessageBoxRequest request) {
        Objects.requireNonNull(request, "request");

        Alert alert = new Alert(request.alertType());
        Window owner = ownerSupplier.get();
        if (owner != null) {
            alert.initOwner(owner);
        }
        alert.setTitle(request.title());
        alert.setHeaderText(request.headerText());
        alert.setContentText(request.contentText());
        if (!request.buttons().isEmpty()) {
            alert.getButtonTypes().setAll(request.buttons());
        }
        alert.showAndWait();
    }

    @Override
    public Optional<Color> showColorDialog(Color initialColor) {
        ColorPickerDialog dialog = new ColorPickerDialog(ownerSupplier.get(), initialColor);
        return dialog.showAndWait();
    }

    @Override
    public Optional<Font> showFontDialog(Font initialFont) {
        FontChooserDialog dialog = new FontChooserDialog(ownerSupplier.get(), initialFont);
        return dialog.showAndWait();
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

    @Override
    public Optional<CodepageResult> showCodepageDialog(CodepageRequest request) {
        CodepageDialog dialog = new CodepageDialog(ownerSupplier.get(), request);
        return dialog.showAndWait();
    }

    @Override
    public Optional<PatchDialogResult> showPatchDialog(PatchDialogRequest request) {
        PatchDialog dialog = new PatchDialog(ownerSupplier.get(), request);
        return dialog.showAndWait();
    }

    @Override
    public Optional<Integer> showWindowsManagerDialog(WindowsManagerModel model) {
        WindowsManagerDialog dialog = new WindowsManagerDialog(ownerSupplier.get(), model);
        return dialog.showAndWait();
    }

    @Override
    public Optional<OpenTableResult> showOpenTableDialog(OpenTableRequest request) {
        OpenTableDialog dialog = new OpenTableDialog(ownerSupplier.get(), request);
        return dialog.showAndWait();
    }
}
