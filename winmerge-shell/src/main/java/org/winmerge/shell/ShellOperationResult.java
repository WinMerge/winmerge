package org.winmerge.shell;

import java.util.List;

public record ShellOperationResult(boolean success, String message, List<String> executedCommands) {
    public ShellOperationResult {
        message = message == null ? "" : message;
        executedCommands = executedCommands == null ? List.of() : List.copyOf(executedCommands);
    }

    public static ShellOperationResult success(String message, List<String> commands) {
        return new ShellOperationResult(true, message, commands);
    }

    public static ShellOperationResult failure(String message, List<String> commands) {
        return new ShellOperationResult(false, message, commands);
    }
}
