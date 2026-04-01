package org.winmerge.desktop.ui.options;

import java.util.Objects;
import java.util.Optional;

import javafx.scene.control.ButtonType;

/**
 * Encapsulates the options-dialog draft/apply/commit lifecycle so it can be
 * verified independently from UI rendering concerns.
 */
public final class OptionsDialogSession {
    private final AppSettings appSettings;
    private final AppSettingsStore appSettingsStore;
    private final AppSettings baseline;
    private final AppSettings draft;

    public OptionsDialogSession(AppSettings appSettings, AppSettingsStore appSettingsStore) {
        this.appSettings = Objects.requireNonNull(appSettings, "appSettings");
        this.appSettingsStore = Objects.requireNonNull(appSettingsStore, "appSettingsStore");
        this.baseline = appSettings.copy();
        this.draft = appSettings.copy();
    }

    public AppSettings draft() {
        return draft;
    }

    public void applyPreview() {
        appSettings.applyFrom(draft);
    }

    public boolean finish(Optional<ButtonType> result) {
        if (result.isPresent() && result.get() == ButtonType.OK) {
            appSettings.applyFrom(draft);
            appSettingsStore.save(appSettings);
            return true;
        }
        appSettings.applyFrom(baseline);
        return false;
    }
}
