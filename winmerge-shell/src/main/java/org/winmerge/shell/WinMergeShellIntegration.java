package org.winmerge.shell;

import java.net.URI;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Locale;
import java.util.Objects;
import java.util.Optional;

public final class WinMergeShellIntegration {
    private final ShellPlatform platform;
    private final ShellPreferencesStore preferencesStore;
    private final ShellLauncher shellLauncher;
    private final ShellRegistrationManager registrationManager;

    public WinMergeShellIntegration() {
        this(
            ShellPlatform.detect(),
            new ShellPreferencesStore(),
            new ProcessCommandRunner(),
            Paths.get(System.getProperty("winmerge.executable", "winmerge-desktop"))
        );
    }

    WinMergeShellIntegration(
        ShellPlatform platform,
        ShellPreferencesStore preferencesStore,
        CommandRunner commandRunner,
        Path executablePath
    ) {
        this.platform = Objects.requireNonNull(platform, "platform");
        this.preferencesStore = Objects.requireNonNull(preferencesStore, "preferencesStore");
        Objects.requireNonNull(commandRunner, "commandRunner");
        Objects.requireNonNull(executablePath, "executablePath");

        ShellFolderResolver folderResolver = new ShellFolderResolver(platform);
        this.shellLauncher = new ShellLauncher(platform, commandRunner);
        this.registrationManager = new ShellRegistrationManager(platform, commandRunner, folderResolver, executablePath);
    }

    public String integrationMode() {
        return loadSettings().contextMenuEnabled() ? "enabled" : "disabled";
    }

    public ShellPlatform platform() {
        return platform;
    }

    public ShellIntegrationSettings loadSettings() {
        return preferencesStore.load();
    }

    public ShellOperationResult applySettings(ShellIntegrationSettings settings) {
        Objects.requireNonNull(settings, "settings");
        preferencesStore.save(settings);
        if (settings.contextMenuEnabled()) {
            return registrationManager.registerContextMenu(settings);
        }
        return registrationManager.unregisterContextMenu();
    }

    public ShellOperationResult registerContextMenu() {
        ShellIntegrationSettings current = preferencesStore.load();
        ShellIntegrationSettings desired = new ShellIntegrationSettings(
            true,
            current.advancedContextMenu(),
            current.compareAsSubmenu(),
            current.jumpListTasks()
        );
        preferencesStore.save(desired);
        return registrationManager.registerContextMenu(desired);
    }

    public ShellOperationResult unregisterContextMenu() {
        ShellIntegrationSettings current = preferencesStore.load();
        ShellIntegrationSettings desired = new ShellIntegrationSettings(
            false,
            current.advancedContextMenu(),
            current.compareAsSubmenu(),
            current.jumpListTasks()
        );
        preferencesStore.save(desired);
        return registrationManager.unregisterContextMenu();
    }

    public ShellOperationResult registerFileAssociation(String extension, String mimeType, String description) {
        ShellOperationResult result = registrationManager.registerFileAssociation(extension, mimeType, description);
        if (result.success()) {
            preferencesStore.putFileAssociation(normalizeExtension(extension), mimeType == null ? "" : mimeType);
        }
        return result;
    }

    public ShellOperationResult unregisterFileAssociation(String extension, String mimeType) {
        ShellOperationResult result = registrationManager.unregisterFileAssociation(extension, mimeType);
        if (result.success()) {
            preferencesStore.removeFileAssociation(normalizeExtension(extension));
        }
        return result;
    }

    public Optional<String> registeredFileAssociation(String extension) {
        return preferencesStore.getFileAssociation(normalizeExtension(extension));
    }

    public ShellOperationResult open(Path path) {
        return shellLauncher.open(path);
    }

    public ShellOperationResult edit(Path path) {
        return shellLauncher.edit(path);
    }

    public ShellOperationResult browse(URI uri) {
        return shellLauncher.browse(uri);
    }

    public ShellOperationResult openFileOrUri(Path file, URI fallbackUri) {
        return shellLauncher.openFileOrUri(file, fallbackUri);
    }

    public ShellOperationResult openParentFolder(Path file) {
        return shellLauncher.openParentFolder(file);
    }

    private static String normalizeExtension(String extension) {
        if (extension == null || extension.isBlank()) {
            throw new IllegalArgumentException("extension must not be blank");
        }
        String normalized = extension.trim().toLowerCase(Locale.ROOT);
        return normalized.startsWith(".") ? normalized : "." + normalized;
    }
}
