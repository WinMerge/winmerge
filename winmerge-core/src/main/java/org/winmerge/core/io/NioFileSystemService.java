package org.winmerge.core.io;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.OpenOption;
import java.nio.file.Path;
import java.nio.file.StandardOpenOption;

public final class NioFileSystemService implements FileSystemService {
    @Override
    public Path normalize(Path path) {
        return path.toAbsolutePath().normalize();
    }

    @Override
    public boolean exists(Path path) {
        return Files.exists(path);
    }

    @Override
    public boolean isRegularFile(Path path) {
        return Files.isRegularFile(path);
    }

    @Override
    public long size(Path path) throws IOException {
        return Files.size(path);
    }

    @Override
    public byte[] readAllBytes(Path path) throws IOException {
        return Files.readAllBytes(path);
    }

    @Override
    public String readString(Path path, Charset charset) throws IOException {
        return Files.readString(path, charset);
    }

    @Override
    public void writeString(Path path, String value, Charset charset, OpenOption... options) throws IOException {
        ensureParentDirectory(path);
        if (options == null || options.length == 0) {
            Files.writeString(path, value, charset, StandardOpenOption.CREATE, StandardOpenOption.TRUNCATE_EXISTING, StandardOpenOption.WRITE);
            return;
        }
        Files.writeString(path, value, charset, options);
    }

    @Override
    public void writeBytes(Path path, byte[] value, OpenOption... options) throws IOException {
        ensureParentDirectory(path);
        if (options == null || options.length == 0) {
            Files.write(path, value, StandardOpenOption.CREATE, StandardOpenOption.TRUNCATE_EXISTING, StandardOpenOption.WRITE);
            return;
        }
        Files.write(path, value, options);
    }

    @Override
    public ByteBuffer mapReadOnly(Path path) throws IOException {
        try (FileChannel channel = FileChannel.open(path, StandardOpenOption.READ)) {
            return channel.map(FileChannel.MapMode.READ_ONLY, 0, channel.size());
        }
    }

    private static void ensureParentDirectory(Path path) throws IOException {
        Path parent = path.getParent();
        if (parent != null) {
            Files.createDirectories(parent);
        }
    }
}
