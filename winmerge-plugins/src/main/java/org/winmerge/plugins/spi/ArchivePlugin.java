package org.winmerge.plugins.spi;

import java.io.IOException;
import java.nio.file.Path;

public interface ArchivePlugin extends WinMergePlugin {
    boolean supportsArchive(Path archivePath);

    void extract(Path archivePath, Path destinationDirectory) throws IOException;

    void create(Path sourceDirectory, Path archivePath) throws IOException;
}
