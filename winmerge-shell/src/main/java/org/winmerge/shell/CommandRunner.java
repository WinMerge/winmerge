package org.winmerge.shell;

import java.io.IOException;
import java.util.List;

public interface CommandRunner {
    ShellCommandResult run(List<String> command) throws IOException, InterruptedException;
}
