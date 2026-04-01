package org.winmerge.desktop.ui.dialogs;

import java.nio.file.Path;
import java.util.Optional;

public interface DialogService {
    void showAboutDialog(AboutDialogModel model);

    Optional<WMGotoDialogResult> showGotoDialog(WMGotoDialogRequest request);

    Optional<String> showComparisonResultFilterDialog(boolean threeWay);

    SaveClosingChoice showSaveClosingDialog(Path filePath);
}
