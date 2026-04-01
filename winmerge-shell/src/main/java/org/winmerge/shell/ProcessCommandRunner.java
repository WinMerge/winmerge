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
        processBuilder.redirectErrorStream(true);
        Process process = processBuilder.start();
        byte[] mergedOutputBytes = process.getInputStream().readAllBytes();
        int exitCode = process.waitFor();
        String mergedOutput = new String(mergedOutputBytes, StandardCharsets.UTF_8).trim();
        return new ShellCommandResult(
            exitCode,
            mergedOutput,
            ""
        );
    }
}
