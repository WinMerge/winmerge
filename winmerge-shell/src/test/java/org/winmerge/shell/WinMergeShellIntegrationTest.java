package org.winmerge.shell;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;

import java.nio.file.Path;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.UUID;
import java.util.prefs.BackingStoreException;
import java.util.prefs.Preferences;

import org.junit.jupiter.api.Test;

class WinMergeShellIntegrationTest {
    @Test
    void doesNotPersistContextMenuEnabledWhenRegistrationFails() throws BackingStoreException {
        Preferences node = Preferences.userRoot().node("org/winmerge/shell/tests/" + UUID.randomUUID());
        try {
            ShellPreferencesStore store = new ShellPreferencesStore(node);
            RecordingCommandRunner runner = new RecordingCommandRunner();
            runner.scriptPrefix("reg add", new ShellCommandResult(1, "", "simulated failure"));

            WinMergeShellIntegration integration = new WinMergeShellIntegration(
                ShellPlatform.WINDOWS,
                store,
                runner,
                Path.of("C:/Program Files/WinMerge/WinMerge.exe")
            );

            ShellOperationResult result = integration.registerContextMenu();

            assertFalse(result.success());
            assertFalse(store.load().contextMenuEnabled());
        } finally {
            node.removeNode();
        }
    }

    @Test
    void doesNotDeleteStoredAssociationWhenUnregisterFails() throws BackingStoreException {
        Preferences node = Preferences.userRoot().node("org/winmerge/shell/tests/" + UUID.randomUUID());
        try {
            ShellPreferencesStore store = new ShellPreferencesStore(node);
            store.putFileAssociation("txt", "text/plain");

            WinMergeShellIntegration integration = new WinMergeShellIntegration(
                ShellPlatform.LINUX,
                store,
                new RecordingCommandRunner(),
                Path.of("/opt/winmerge/bin/winmerge")
            );

            ShellOperationResult result = integration.unregisterFileAssociation("txt", "text/plain");

            assertFalse(result.success());
            assertEquals("text/plain", store.getFileAssociation(".txt").orElseThrow());
        } finally {
            node.removeNode();
        }
    }

    private static final class RecordingCommandRunner implements CommandRunner {
        private final Map<String, ShellCommandResult> prefixScripts = new LinkedHashMap<>();

        void scriptPrefix(String prefix, ShellCommandResult result) {
            prefixScripts.put(prefix, result);
        }

        @Override
        public ShellCommandResult run(List<String> command) {
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
