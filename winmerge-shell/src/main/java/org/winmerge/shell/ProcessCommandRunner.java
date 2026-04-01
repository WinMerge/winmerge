package org.winmerge.shell;

import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.util.List;
import java.util.Objects;

public final class ProcessCommandRunner implements CommandRunner {
    @Override
    public ShellCommandResult run(List<String> command) throws IOException, InterruptedException {
        Objects.requireNonNull(command, "command");
        ProcessBuilder processBuilder = new ProcessBuilder(command);
        Process process = processBuilder.start();
        byte[] stdoutBytes = process.getInputStream().readAllBytes();
        byte[] stderrBytes = process.getErrorStream().readAllBytes();
        int exitCode = process.waitFor();
        return new ShellCommandResult(
            exitCode,
            new String(stdoutBytes, StandardCharsets.UTF_8).trim(),
            new String(stderrBytes, StandardCharsets.UTF_8).trim()
        );
    }
}
