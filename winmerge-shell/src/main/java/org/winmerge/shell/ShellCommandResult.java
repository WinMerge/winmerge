package org.winmerge.shell;

public record ShellCommandResult(int exitCode, String stdout, String stderr) {
    public boolean isSuccess() {
        return exitCode == 0;
    }
}
