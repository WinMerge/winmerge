package org.winmerge.shell;

import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;
import java.util.Objects;

public final class ShellRegistrationManager {
    private final ShellPlatform platform;
    private final CommandRunner commandRunner;
    private final ShellFolderResolver folderResolver;
    private final Path executablePath;

    public ShellRegistrationManager(
        ShellPlatform platform,
        CommandRunner commandRunner,
        ShellFolderResolver folderResolver,
        Path executablePath
    ) {
        this.platform = Objects.requireNonNull(platform, "platform");
        this.commandRunner = Objects.requireNonNull(commandRunner, "commandRunner");
        this.folderResolver = Objects.requireNonNull(folderResolver, "folderResolver");
        this.executablePath = Objects.requireNonNull(executablePath, "executablePath");
    }

    public ShellOperationResult registerContextMenu(ShellIntegrationSettings settings) {
        return switch (platform) {
            case WINDOWS -> registerContextMenuWindows(settings);
            case LINUX -> registerContextMenuLinux();
            case MAC -> ShellOperationResult.failure(
                "macOS Finder context menu registration requires a bundled Service/Quick Action and is not automated in this phase.",
                List.of()
            );
            case OTHER -> ShellOperationResult.failure("Context menu registration is not supported on this platform.", List.of());
        };
    }

    public ShellOperationResult unregisterContextMenu() {
        return switch (platform) {
            case WINDOWS -> unregisterContextMenuWindows();
            case LINUX -> unregisterContextMenuLinux();
            case MAC, OTHER -> ShellOperationResult.success("No context menu artifacts to remove on this platform.", List.of());
        };
    }

    public ShellOperationResult registerFileAssociation(String extension, String mimeType, String description) {
        String normalizedExtension = normalizeExtension(extension);
        return switch (platform) {
            case WINDOWS -> registerFileAssociationWindows(normalizedExtension, description);
            case LINUX, MAC, OTHER -> ShellOperationResult.failure(
                "File-association registration is disabled on this platform because rollback cannot be guaranteed.",
                List.of()
            );
        };
    }

    public ShellOperationResult unregisterFileAssociation(String extension, String mimeType) {
        String normalizedExtension = normalizeExtension(extension);
        return switch (platform) {
            case WINDOWS -> unregisterFileAssociationWindows(normalizedExtension);
            case LINUX, MAC, OTHER -> ShellOperationResult.failure(
                "Automated file-association rollback is not supported on this platform.",
                List.of()
            );
        };
    }

    private ShellOperationResult registerContextMenuWindows(ShellIntegrationSettings settings) {
        List<List<String>> commands = new ArrayList<>();
        String commandLine = "\"" + executablePath.toAbsolutePath().normalize() + "\" \"%1\"";

        commands.add(regAdd("HKCU\\Software\\Classes\\*\\shell\\WinMerge", "Compare with WinMerge"));
        commands.add(regAdd("HKCU\\Software\\Classes\\*\\shell\\WinMerge\\command", commandLine));
        commands.add(regAdd("HKCU\\Software\\Classes\\Directory\\shell\\WinMerge", "Compare folders with WinMerge"));
        commands.add(regAdd("HKCU\\Software\\Classes\\Directory\\shell\\WinMerge\\command", commandLine));

        if (settings.advancedContextMenu()) {
            commands.add(regAdd("HKCU\\Software\\Classes\\Directory\\Background\\shell\\WinMergeAdvanced", "WinMerge Advanced Compare"));
            commands.add(regAdd("HKCU\\Software\\Classes\\Directory\\Background\\shell\\WinMergeAdvanced\\command", commandLine + " /advanced"));
        }

        if (settings.compareAsSubmenu()) {
            commands.add(regAdd("HKCU\\Software\\Classes\\*\\shell\\WinMergeCompareAs", "Compare As with WinMerge"));
            commands.add(regAdd("HKCU\\Software\\Classes\\*\\shell\\WinMergeCompareAs\\command", commandLine + " /compare-as"));
        }

        return runCommands(commands, "Windows context menu registration completed");
    }

    private ShellOperationResult unregisterContextMenuWindows() {
        return runRegDeletesIdempotent(
            List.of(
                "HKCU\\Software\\Classes\\*\\shell\\WinMerge",
                "HKCU\\Software\\Classes\\Directory\\shell\\WinMerge",
                "HKCU\\Software\\Classes\\Directory\\Background\\shell\\WinMergeAdvanced",
                "HKCU\\Software\\Classes\\*\\shell\\WinMergeCompareAs"
            ),
            "Windows context menu entries removed"
        );
    }

    private ShellOperationResult registerContextMenuLinux() {
        try {
            Path actionsDir = folderResolver.userHomeDirectory()
                .resolve(".local")
                .resolve("share")
                .resolve("file-manager")
                .resolve("actions");
            Files.createDirectories(actionsDir);
            Path entryFile = actionsDir.resolve("winmerge.desktop");

            String content = """
                [Desktop Entry]
                Type=Action
                Name=Compare with WinMerge
                Icon=applications-development
                Profiles=profile-zero;

                [X-Action-Profile profile-zero]
                MimeTypes=all/allfiles;
                Exec=%s %%F
                """.formatted(executablePath.toAbsolutePath().normalize());
            Files.writeString(entryFile, content, StandardCharsets.UTF_8);
            return ShellOperationResult.success(
                "Linux file-manager action installed at " + entryFile,
                List.of("write " + entryFile)
            );
        } catch (IOException ex) {
            return ShellOperationResult.failure("Failed to install Linux context action: " + ex.getMessage(), List.of());
        }
    }

    private ShellOperationResult unregisterContextMenuLinux() {
        try {
            Path entryFile = folderResolver.userHomeDirectory()
                .resolve(".local")
                .resolve("share")
                .resolve("file-manager")
                .resolve("actions")
                .resolve("winmerge.desktop");
            Files.deleteIfExists(entryFile);
            return ShellOperationResult.success("Linux file-manager action removed", List.of("delete " + entryFile));
        } catch (IOException ex) {
            return ShellOperationResult.failure("Failed to remove Linux context action: " + ex.getMessage(), List.of());
        }
    }

    private ShellOperationResult registerFileAssociationWindows(String extension, String description) {
        String suffix = extension.substring(1).toUpperCase(Locale.ROOT);
        String progId = "WinMerge." + suffix;
        String commandLine = "\"" + executablePath.toAbsolutePath().normalize() + "\" \"%1\"";
        String extensionKey = "HKCU\\Software\\Classes\\" + extension;

        RegistryDefaultValueQuery existingAssociation = queryRegistryDefaultValue(extensionKey);
        if (!existingAssociation.querySucceeded()) {
            return ShellOperationResult.failure(existingAssociation.errorMessage(), existingAssociation.executedCommands());
        }
        if (existingAssociation.keyExists() && !progId.equalsIgnoreCase(existingAssociation.defaultValue())) {
            return ShellOperationResult.failure(
                "Refusing to overwrite existing Windows association for "
                    + extension
                    + " because rollback cannot restore current handler '"
                    + existingAssociation.defaultValue()
                    + "'.",
                existingAssociation.executedCommands()
            );
        }

        List<List<String>> commands = List.of(
            regAdd(extensionKey, progId),
            regAdd("HKCU\\Software\\Classes\\" + progId, description == null || description.isBlank()
                ? "WinMerge file association (" + extension + ")"
                : description),
            regAdd("HKCU\\Software\\Classes\\" + progId + "\\shell\\open\\command", commandLine)
        );
        ShellOperationResult registrationResult = runCommands(commands, "Windows file association registration completed for " + extension);
        List<String> executed = new ArrayList<>(existingAssociation.executedCommands());
        executed.addAll(registrationResult.executedCommands());
        return registrationResult.success()
            ? ShellOperationResult.success(registrationResult.message(), executed)
            : ShellOperationResult.failure(registrationResult.message(), executed);
    }

    private ShellOperationResult unregisterFileAssociationWindows(String extension) {
        String suffix = extension.substring(1).toUpperCase(Locale.ROOT);
        String progId = "WinMerge." + suffix;
        String extensionKey = "HKCU\\Software\\Classes\\" + extension;

        RegistryDefaultValueQuery existingAssociation = queryRegistryDefaultValue(extensionKey);
        if (!existingAssociation.querySucceeded()) {
            return ShellOperationResult.failure(existingAssociation.errorMessage(), existingAssociation.executedCommands());
        }
        if (!existingAssociation.keyExists()) {
            return ShellOperationResult.success(
                "Windows file association already removed for " + extension,
                existingAssociation.executedCommands()
            );
        }
        if (!progId.equalsIgnoreCase(existingAssociation.defaultValue())) {
            return ShellOperationResult.failure(
                "Refusing to remove Windows association for "
                    + extension
                    + " because current handler is '"
                    + existingAssociation.defaultValue()
                    + "'.",
                existingAssociation.executedCommands()
            );
        }

        ShellOperationResult deleteResult = runRegDeletesIdempotent(
            List.of(
                extensionKey,
                "HKCU\\Software\\Classes\\" + progId
            ),
            "Windows file association removed for " + extension
        );
        List<String> executed = new ArrayList<>(existingAssociation.executedCommands());
        executed.addAll(deleteResult.executedCommands());
        return deleteResult.success()
            ? ShellOperationResult.success(deleteResult.message(), executed)
            : ShellOperationResult.failure(deleteResult.message(), executed);
    }

    private ShellOperationResult registerFileAssociationLinux(String mimeType) {
        if (mimeType == null || mimeType.isBlank()) {
            return ShellOperationResult.failure("mimeType is required for Linux file association.", List.of());
        }

        try {
            Path applicationsDir = folderResolver.userHomeDirectory()
                .resolve(".local")
                .resolve("share")
                .resolve("applications");
            Files.createDirectories(applicationsDir);
            Path desktopFile = applicationsDir.resolve("winmerge.desktop");
            String content = """
                [Desktop Entry]
                Type=Application
                Name=WinMerge
                Exec=%s %%F
                Terminal=false
                MimeType=%s;
                """.formatted(executablePath.toAbsolutePath().normalize(), mimeType);
            Files.writeString(desktopFile, content, StandardCharsets.UTF_8);
            List<List<String>> commands = List.of(List.of("xdg-mime", "default", "winmerge.desktop", mimeType));
            ShellOperationResult result = runCommands(commands, "Linux file association registered for " + mimeType);
            if (!result.success()) {
                return ShellOperationResult.failure(
                    result.message() + " (desktop file written to " + desktopFile + ")",
                    result.executedCommands()
                );
            }
            return result;
        } catch (IOException ex) {
            return ShellOperationResult.failure("Failed writing linux desktop file: " + ex.getMessage(), List.of());
        }
    }

    private ShellOperationResult registerFileAssociationMac(String extension) {
        String ext = extension.startsWith(".") ? extension.substring(1) : extension;
        List<List<String>> commands = List.of(List.of("duti", "-s", "org.winmerge.desktop", ext, "all"));
        return runCommands(commands, "macOS file association requested for ." + ext);
    }

    private ShellOperationResult runCommands(List<List<String>> commands, String successMessage) {
        List<String> executed = new ArrayList<>();
        for (List<String> command : commands) {
            executed.add(String.join(" ", command));
            try {
                ShellCommandResult result = commandRunner.run(command);
                if (!result.isSuccess()) {
                    return ShellOperationResult.failure(
                        "Command failed (" + result.exitCode() + "): " + commandFailureDetails(result),
                        executed
                    );
                }
            } catch (InterruptedException ex) {
                Thread.currentThread().interrupt();
                return ShellOperationResult.failure("Command interrupted: " + ex.getMessage(), executed);
            } catch (IOException ex) {
                return ShellOperationResult.failure("Command execution failed: " + ex.getMessage(), executed);
            }
        }
        return ShellOperationResult.success(successMessage, executed);
    }

    private ShellOperationResult runRegDeletesIdempotent(List<String> keys, String successMessage) {
        List<String> executed = new ArrayList<>();
        for (String key : keys) {
            List<String> queryCommand = List.of("reg", "query", key);
            executed.add(String.join(" ", queryCommand));
            try {
                ShellCommandResult queryResult = commandRunner.run(queryCommand);
                if (!queryResult.isSuccess()) {
                    if (isMissingRegistryKey(queryResult)) {
                        continue;
                    }
                    return ShellOperationResult.failure(
                        "Command failed (" + queryResult.exitCode() + "): " + commandFailureDetails(queryResult),
                        executed
                    );
                }
                List<String> deleteCommand = regDelete(key);
                executed.add(String.join(" ", deleteCommand));
                ShellCommandResult deleteResult = commandRunner.run(deleteCommand);
                if (!deleteResult.isSuccess()) {
                    return ShellOperationResult.failure(
                        "Command failed (" + deleteResult.exitCode() + "): " + commandFailureDetails(deleteResult),
                        executed
                    );
                }
            } catch (InterruptedException ex) {
                Thread.currentThread().interrupt();
                return ShellOperationResult.failure("Command interrupted: " + ex.getMessage(), executed);
            } catch (IOException ex) {
                return ShellOperationResult.failure("Command execution failed: " + ex.getMessage(), executed);
            }
        }
        return ShellOperationResult.success(successMessage, executed);
    }

    private static List<String> regAdd(String key, String value) {
        return List.of("reg", "add", key, "/ve", "/d", value, "/f");
    }

    private static List<String> regDelete(String key) {
        return List.of("reg", "delete", key, "/f");
    }

    private static String normalizeExtension(String extension) {
        if (extension == null || extension.isBlank()) {
            throw new IllegalArgumentException("extension must not be blank");
        }
        String normalized = extension.trim().toLowerCase(Locale.ROOT);
        return normalized.startsWith(".") ? normalized : "." + normalized;
    }

    private static String commandFailureDetails(ShellCommandResult result) {
        if (result.stderr() != null && !result.stderr().isBlank()) {
            return result.stderr();
        }
        if (result.stdout() != null && !result.stdout().isBlank()) {
            return result.stdout();
        }
        return "No output";
    }

    private static boolean isMissingRegistryKey(ShellCommandResult queryResult) {
        String output = ((queryResult.stderr() == null ? "" : queryResult.stderr()) + "\n"
            + (queryResult.stdout() == null ? "" : queryResult.stdout())).toLowerCase(Locale.ROOT);
        return output.contains("unable to find the specified registry key")
            || output.contains("unable to find")
            || output.contains("cannot find")
            || output.contains("not found");
    }

    private RegistryDefaultValueQuery queryRegistryDefaultValue(String key) {
        List<String> queryCommand = List.of("reg", "query", key, "/ve");
        List<String> executed = List.of(String.join(" ", queryCommand));
        try {
            ShellCommandResult queryResult = commandRunner.run(queryCommand);
            if (!queryResult.isSuccess()) {
                if (isMissingRegistryKey(queryResult)) {
                    return new RegistryDefaultValueQuery(true, false, "", "", executed);
                }
                return new RegistryDefaultValueQuery(
                    false,
                    false,
                    "",
                    "Command failed (" + queryResult.exitCode() + "): " + commandFailureDetails(queryResult),
                    executed
                );
            }

            String parsedValue = parseRegistryDefaultValue(queryResult.stdout());
            if (parsedValue == null) {
                return new RegistryDefaultValueQuery(
                    false,
                    true,
                    "",
                    "Unable to parse current registry handler for key " + key + ".",
                    executed
                );
            }
            return new RegistryDefaultValueQuery(true, true, parsedValue, "", executed);
        } catch (InterruptedException ex) {
            Thread.currentThread().interrupt();
            return new RegistryDefaultValueQuery(false, false, "", "Command interrupted: " + ex.getMessage(), executed);
        } catch (IOException ex) {
            return new RegistryDefaultValueQuery(false, false, "", "Command execution failed: " + ex.getMessage(), executed);
        }
    }

    private static String parseRegistryDefaultValue(String stdout) {
        if (stdout == null || stdout.isBlank()) {
            return null;
        }
        for (String line : stdout.split("\\R")) {
            String trimmed = line.trim();
            if (!trimmed.startsWith("(Default)")) {
                continue;
            }
            String[] parts = trimmed.split("\\s+", 3);
            if (parts.length < 2) {
                return "";
            }
            return parts.length == 2 ? "" : parts[2].trim();
        }
        return null;
    }

    private record RegistryDefaultValueQuery(
        boolean querySucceeded,
        boolean keyExists,
        String defaultValue,
        String errorMessage,
        List<String> executedCommands
    ) {
    }
}
