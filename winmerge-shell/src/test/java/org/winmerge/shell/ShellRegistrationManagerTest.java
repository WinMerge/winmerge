package org.winmerge.shell;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.LinkedHashMap;
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
    void windowsUnregisterTreatsMissingKeysAsConvergedState() {
        RecordingCommandRunner runner = new RecordingCommandRunner();
        runner.scriptPrefix("reg query", new ShellCommandResult(1, "", "not found"));

        ShellFolderResolver resolver = new ShellFolderResolver(ShellPlatform.WINDOWS, Map.of(), Path.of("C:/Users/tester"));
        ShellRegistrationManager manager = new ShellRegistrationManager(
            ShellPlatform.WINDOWS,
            runner,
            resolver,
            Path.of("C:/Program Files/WinMerge/WinMerge.exe")
        );

        ShellOperationResult result = manager.unregisterContextMenu();

        assertTrue(result.success());
        assertTrue(runner.commands.stream().allMatch(command -> !"delete".equalsIgnoreCase(command.get(1))));
    }

    @Test
    void windowsUnregisterFailsWhenRegQueryFailsForNonMissingReason() {
        RecordingCommandRunner runner = new RecordingCommandRunner();
        runner.scriptPrefix("reg query", new ShellCommandResult(1, "", "ERROR: Access is denied."));

        ShellFolderResolver resolver = new ShellFolderResolver(ShellPlatform.WINDOWS, Map.of(), Path.of("C:/Users/tester"));
        ShellRegistrationManager manager = new ShellRegistrationManager(
            ShellPlatform.WINDOWS,
            runner,
            resolver,
            Path.of("C:/Program Files/WinMerge/WinMerge.exe")
        );

        ShellOperationResult result = manager.unregisterContextMenu();

        assertFalse(result.success());
        assertTrue(result.message().contains("Access is denied"));
        assertTrue(runner.commands.stream().allMatch(command -> !"delete".equalsIgnoreCase(command.get(1))));
    }

    @Test
    void nonWindowsFileAssociationUnregisterReturnsFailure() {
        RecordingCommandRunner runner = new RecordingCommandRunner();
        ShellFolderResolver resolver = new ShellFolderResolver(ShellPlatform.LINUX, Map.of(), Path.of("/tmp/home"));
        ShellRegistrationManager manager = new ShellRegistrationManager(
            ShellPlatform.LINUX,
            runner,
            resolver,
            Path.of("/opt/winmerge/bin/winmerge")
        );

        ShellOperationResult result = manager.unregisterFileAssociation(".txt", "text/plain");

        assertFalse(result.success());
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
        private final Map<String, ShellCommandResult> prefixScripts = new LinkedHashMap<>();

        void scriptPrefix(String prefix, ShellCommandResult result) {
            prefixScripts.put(prefix, result);
        }

        @Override
        public ShellCommandResult run(List<String> command) {
            commands.add(List.copyOf(command));
            String commandString = String.join(" ", command);
            for (Map.Entry<String, ShellCommandResult> scripted : prefixScripts.entrySet()) {
                if (commandString.startsWith(scripted.getKey())) {
                    return scripted.getValue();
                }
            }
            return new ShellCommandResult(0, "", "");
        }
    }
}
