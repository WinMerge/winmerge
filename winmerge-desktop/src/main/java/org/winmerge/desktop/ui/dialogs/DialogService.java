package org.winmerge.desktop.ui.dialogs;

import java.util.Optional;

public interface DialogService {
    void showAboutDialog(AboutDialogModel model);

    Optional<WMGotoDialogResult> showGotoDialog(WMGotoDialogRequest request);
}
