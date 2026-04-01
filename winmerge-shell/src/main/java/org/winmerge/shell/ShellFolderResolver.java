package org.winmerge.shell;

import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Map;
import java.util.Objects;

public final class ShellFolderResolver {
    private final ShellPlatform platform;
    private final Map<String, String> environment;
    private final Path userHome;

    public ShellFolderResolver(ShellPlatform platform) {
        this(platform, System.getenv(), Paths.get(System.getProperty("user.home", ".")));
    }

    ShellFolderResolver(ShellPlatform platform, Map<String, String> environment, Path userHome) {
        this.platform = Objects.requireNonNull(platform, "platform");
        this.environment = Objects.requireNonNull(environment, "environment");
        this.userHome = Objects.requireNonNull(userHome, "userHome");
    }

    public Path appDataDirectory() {
        return switch (platform) {
            case WINDOWS -> fromEnvironment("APPDATA", userHome.resolve("AppData").resolve("Roaming"));
            case MAC -> userHome.resolve("Library").resolve("Application Support").resolve("WinMerge");
            case LINUX -> fromEnvironment("XDG_CONFIG_HOME", userHome.resolve(".config")).resolve("winmerge");
            case OTHER -> userHome.resolve(".winmerge");
        };
    }

    public Path documentsDirectory() {
        return switch (platform) {
            case WINDOWS -> fromEnvironment("USERPROFILE", userHome).resolve("Documents");
            case MAC, LINUX, OTHER -> userHome.resolve("Documents");
        };
    }

    public Path userHomeDirectory() {
        return userHome;
    }

    private Path fromEnvironment(String key, Path fallback) {
        String value = environment.get(key);
        if (value == null || value.isBlank()) {
            return fallback;
        }
        return Paths.get(value);
    }
}
