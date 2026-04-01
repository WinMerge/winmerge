package org.winmerge.shell;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.junit.jupiter.api.Test;

class ShellRegistrationManagerTest {
    @Test
    void buildsWindowsRegistryCommandsForContextMenuRegistration() {
        RecordingCommandRunner runner = new RecordingCommandRunner();
        ShellFolderResolver resolver = new ShellFolderResolver(ShellPlatform.WINDOWS, Map.of(), Path.of("C:/Users/tester"));
        ShellRegistrationManager manager = new ShellRegistrationManager(
            ShellPlatform.WINDOWS,
            runner,
            resolver,
            Path.of("C:/Program Files/WinMerge/WinMerge.exe")
        );

        ShellOperationResult result = manager.registerContextMenu(new ShellIntegrationSettings(true, true, true, "newText,options"));

        assertTrue(result.success());
        assertFalse(runner.commands.isEmpty());
        assertEquals("reg", runner.commands.get(0).get(0));
    }

    @Test
    void writesLinuxContextMenuDesktopEntry() throws IOException {
        Path home = Files.createTempDirectory("winmerge-shell-test-home");
        try {
            RecordingCommandRunner runner = new RecordingCommandRunner();
            ShellFolderResolver resolver = new ShellFolderResolver(ShellPlatform.LINUX, Map.of(), home);
            ShellRegistrationManager manager = new ShellRegistrationManager(
                ShellPlatform.LINUX,
                runner,
                resolver,
                Path.of("/opt/winmerge/bin/winmerge")
            );

            ShellOperationResult result = manager.registerContextMenu(ShellIntegrationSettings.defaults());
            Path actionFile = home.resolve(".local/share/file-manager/actions/winmerge.desktop");

            assertTrue(result.success());
            assertTrue(Files.exists(actionFile));
            assertTrue(Files.readString(actionFile).contains("Compare with WinMerge"));
        } finally {
            deleteRecursively(home);
        }
    }

    private static void deleteRecursively(Path root) throws IOException {
        if (root == null || !Files.exists(root)) {
            return;
        }
        try (var stream = Files.walk(root)) {
            stream.sorted((a, b) -> b.getNameCount() - a.getNameCount()).forEach(path -> {
                try {
                    Files.deleteIfExists(path);
                } catch (IOException ignored) {
                }
            });
        }
    }

    private static final class RecordingCommandRunner implements CommandRunner {
        private final List<List<String>> commands = new ArrayList<>();

        @Override
        public ShellCommandResult run(List<String> command) {
            commands.add(List.copyOf(command));
            return new ShellCommandResult(0, "", "");
        }
    }
}
