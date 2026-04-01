package org.winmerge.shell;

import java.util.Locale;
import java.util.Optional;
import java.util.prefs.Preferences;

public final class ShellPreferencesStore {
    private static final String KEY_CONTEXT_MENU_ENABLED = "options.shell.contextMenuEnabled";
    private static final String KEY_ADVANCED_CONTEXT_MENU = "options.shell.advancedContextMenu";
    private static final String KEY_COMPARE_AS_SUBMENU = "options.shell.compareAsSubmenu";
    private static final String KEY_JUMP_LIST_TASKS = "options.shell.jumpListTasks";
    private static final String ASSOCIATIONS_NODE = "fileAssociations";

    private final Preferences preferences;

    public ShellPreferencesStore() {
        this(Preferences.userRoot().node("org/winmerge/shell"));
    }

    ShellPreferencesStore(Preferences preferences) {
        this.preferences = preferences;
    }

    public ShellIntegrationSettings load() {
        ShellIntegrationSettings defaults = ShellIntegrationSettings.defaults();
        return new ShellIntegrationSettings(
            preferences.getBoolean(KEY_CONTEXT_MENU_ENABLED, defaults.contextMenuEnabled()),
            preferences.getBoolean(KEY_ADVANCED_CONTEXT_MENU, defaults.advancedContextMenu()),
            preferences.getBoolean(KEY_COMPARE_AS_SUBMENU, defaults.compareAsSubmenu()),
            preferences.get(KEY_JUMP_LIST_TASKS, defaults.jumpListTasks())
        );
    }

    public void save(ShellIntegrationSettings settings) {
        preferences.putBoolean(KEY_CONTEXT_MENU_ENABLED, settings.contextMenuEnabled());
        preferences.putBoolean(KEY_ADVANCED_CONTEXT_MENU, settings.advancedContextMenu());
        preferences.putBoolean(KEY_COMPARE_AS_SUBMENU, settings.compareAsSubmenu());
        preferences.put(KEY_JUMP_LIST_TASKS, settings.jumpListTasks());
    }

    public void putFileAssociation(String extension, String value) {
        preferences.node(ASSOCIATIONS_NODE).put(normalizeExtension(extension), value == null ? "" : value);
    }

    public Optional<String> getFileAssociation(String extension) {
        return Optional.ofNullable(preferences.node(ASSOCIATIONS_NODE).get(normalizeExtension(extension), null));
    }

    public void removeFileAssociation(String extension) {
        preferences.node(ASSOCIATIONS_NODE).remove(normalizeExtension(extension));
    }

    private static String normalizeExtension(String extension) {
        if (extension == null || extension.isBlank()) {
            throw new IllegalArgumentException("extension must not be blank");
        }
        String normalized = extension.trim().toLowerCase(Locale.ROOT);
        return normalized.startsWith(".") ? normalized : "." + normalized;
    }
}
