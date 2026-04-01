package org.winmerge.shell;

import java.awt.Desktop;
import java.io.IOException;
import java.net.URI;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.List;
import java.util.Objects;

public final class ShellLauncher {
    private final ShellPlatform platform;
    private final CommandRunner commandRunner;

    public ShellLauncher(ShellPlatform platform, CommandRunner commandRunner) {
        this.platform = Objects.requireNonNull(platform, "platform");
        this.commandRunner = Objects.requireNonNull(commandRunner, "commandRunner");
    }

    public ShellOperationResult open(Path path) {
        Objects.requireNonNull(path, "path");
        try {
            if (Desktop.isDesktopSupported() && Desktop.getDesktop().isSupported(Desktop.Action.OPEN)) {
                Desktop.getDesktop().open(path.toFile());
                return ShellOperationResult.success("Opened path with Desktop.open", List.of());
            }
        } catch (IOException ex) {
            return ShellOperationResult.failure("Desktop.open failed: " + ex.getMessage(), List.of());
        }
        List<String> command = openCommand(path);
        return runCommand(command, "Opened path using ProcessBuilder");
    }

    public ShellOperationResult edit(Path path) {
        Objects.requireNonNull(path, "path");
        try {
            if (Desktop.isDesktopSupported() && Desktop.getDesktop().isSupported(Desktop.Action.EDIT)) {
                Desktop.getDesktop().edit(path.toFile());
                return ShellOperationResult.success("Opened path with Desktop.edit", List.of());
            }
        } catch (IOException ex) {
            return ShellOperationResult.failure("Desktop.edit failed: " + ex.getMessage(), List.of());
        }
        return open(path);
    }

    public ShellOperationResult browse(URI uri) {
        Objects.requireNonNull(uri, "uri");
        try {
            if (Desktop.isDesktopSupported() && Desktop.getDesktop().isSupported(Desktop.Action.BROWSE)) {
                Desktop.getDesktop().browse(uri);
                return ShellOperationResult.success("Opened URI with Desktop.browse", List.of());
            }
        } catch (IOException ex) {
            return ShellOperationResult.failure("Desktop.browse failed: " + ex.getMessage(), List.of());
        }
        List<String> command = browseCommand(uri);
        return runCommand(command, "Opened URI using ProcessBuilder");
    }

    public ShellOperationResult openFileOrUri(Path file, URI fallbackUri) {
        Objects.requireNonNull(file, "file");
        Objects.requireNonNull(fallbackUri, "fallbackUri");
        if (Files.isRegularFile(file)) {
            return open(file);
        }
        return browse(fallbackUri);
    }

    public ShellOperationResult openParentFolder(Path file) {
        Objects.requireNonNull(file, "file");
        List<String> command = parentFolderCommand(file);
        return runCommand(command, "Opened parent folder using ProcessBuilder");
    }

    List<String> openCommand(Path path) {
        return switch (platform) {
            case WINDOWS -> List.of("explorer.exe", path.toString());
            case MAC -> List.of("open", path.toString());
            case LINUX -> List.of("xdg-open", path.toString());
            case OTHER -> List.of();
        };
    }

    List<String> browseCommand(URI uri) {
        return switch (platform) {
            case WINDOWS -> List.of("rundll32", "url.dll,FileProtocolHandler", uri.toString());
            case MAC -> List.of("open", uri.toString());
            case LINUX -> List.of("xdg-open", uri.toString());
            case OTHER -> List.of();
        };
    }

    List<String> parentFolderCommand(Path file) {
        Path normalized = file.toAbsolutePath().normalize();
        Path parent = normalized.getParent();
        return switch (platform) {
            case WINDOWS -> List.of("explorer.exe", "/select," + normalized);
            case MAC -> List.of("open", "-R", normalized.toString());
            case LINUX -> List.of("xdg-open", (parent == null ? normalized : parent).toString());
            case OTHER -> List.of();
        };
    }

    private ShellOperationResult runCommand(List<String> command, String successMessage) {
        if (command.isEmpty()) {
            return ShellOperationResult.failure("Operation not supported on this platform", List.of());
        }
        try {
            ShellCommandResult result = commandRunner.run(command);
            if (result.isSuccess()) {
                return ShellOperationResult.success(successMessage, List.of(String.join(" ", command)));
            }
            return ShellOperationResult.failure(
                "Command failed (" + result.exitCode() + "): " + commandFailureDetails(result),
                List.of(String.join(" ", command))
            );
        } catch (InterruptedException ex) {
            Thread.currentThread().interrupt();
            return ShellOperationResult.failure("Command interrupted: " + ex.getMessage(), List.of(String.join(" ", command)));
        } catch (IOException ex) {
            return ShellOperationResult.failure("Command execution failed: " + ex.getMessage(), List.of(String.join(" ", command)));
        }
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
}
