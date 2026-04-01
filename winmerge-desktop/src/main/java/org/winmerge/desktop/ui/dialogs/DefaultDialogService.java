package org.winmerge.desktop.ui.dialogs;

import java.nio.file.Path;
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
    public Optional<String> showComparisonResultFilterDialog(boolean threeWay) {
        ComparisonResultFilterDialog dialog = new ComparisonResultFilterDialog(ownerSupplier.get(), threeWay);
        return dialog.showAndWait();
    }

    @Override
    public SaveClosingChoice showSaveClosingDialog(Path filePath) {
        SaveClosingDialog dialog = new SaveClosingDialog(ownerSupplier.get(), filePath);
        return dialog.showAndWait().orElse(SaveClosingChoice.CANCEL);
    }
}
