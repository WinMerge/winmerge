package org.winmerge.desktop.ui;

import java.nio.file.Path;

public interface DirtyTab {
    boolean hasUnsavedChanges();

    Path pathForSavePrompt();

    boolean saveChanges();
}
