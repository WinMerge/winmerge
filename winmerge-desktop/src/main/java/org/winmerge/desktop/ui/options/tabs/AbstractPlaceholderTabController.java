package org.winmerge.desktop.ui.options.tabs;

import org.winmerge.desktop.ui.options.AppSettings;

public abstract class AbstractPlaceholderTabController implements OptionsTabController {
    @Override
    public void bind(AppSettings settings) {
        if (settings == null) {
            throw new IllegalArgumentException("settings must not be null");
        }
    }
}
