package org.winmerge.shell;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.nio.file.Path;
import java.util.List;

import org.junit.jupiter.api.Test;

class ShellLauncherTest {
    @Test
    void buildsWindowsParentFolderCommandWithSelect() {
        ShellLauncher launcher = new ShellLauncher(ShellPlatform.WINDOWS, new RecordingCommandRunner(new ShellCommandResult(0, "", "")));
        List<String> command = launcher.parentFolderCommand(Path.of("C:/temp/example.txt"));

        assertEquals("explorer.exe", command.get(0));
        assertTrue(command.get(1).startsWith("/select,"));
        assertTrue(command.get(1).toLowerCase().contains("example.txt"));
    }

    @Test
    void buildsLinuxParentFolderCommandWithXdgOpen() {
        ShellLauncher launcher = new ShellLauncher(ShellPlatform.LINUX, new RecordingCommandRunner(new ShellCommandResult(0, "", "")));
        Path file = Path.of("/tmp/winmerge/example.txt");
        List<String> command = launcher.parentFolderCommand(file);

        assertEquals(List.of("xdg-open", file.getParent().toString()), command);
    }

    @Test
    void returnsFailureForUnsupportedPlatform() {
        ShellLauncher launcher = new ShellLauncher(ShellPlatform.OTHER, new RecordingCommandRunner(new ShellCommandResult(0, "", "")));
        ShellOperationResult result = launcher.openParentFolder(Path.of("example.txt"));

        assertFalse(result.success());
    }

    @Test
    void runCommandUsesStdoutWhenStderrIsEmptyOnFailure() {
        ShellLauncher launcher = new ShellLauncher(
            ShellPlatform.WINDOWS,
            new RecordingCommandRunner(new ShellCommandResult(1, "merged diagnostic output", ""))
        );

        ShellOperationResult result = launcher.openParentFolder(Path.of("C:/temp/example.txt"));

        assertFalse(result.success());
        assertTrue(result.message().contains("merged diagnostic output"));
    }

    private static final class RecordingCommandRunner implements CommandRunner {
        private final ShellCommandResult scriptedResult;

        private RecordingCommandRunner(ShellCommandResult scriptedResult) {
            this.scriptedResult = scriptedResult;
        }

        @Override
        public ShellCommandResult run(List<String> command) {
            return scriptedResult;
        }
    }
}
