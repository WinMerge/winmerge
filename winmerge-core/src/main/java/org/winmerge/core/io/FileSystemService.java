package org.winmerge.core.io;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.charset.Charset;
import java.nio.file.OpenOption;
import java.nio.file.Path;

public interface FileSystemService {
    static FileSystemService createDefault() {
        return new NioFileSystemService();
    }

    Path normalize(Path path);

    boolean exists(Path path);

    boolean isRegularFile(Path path);

    long size(Path path) throws IOException;

    byte[] readAllBytes(Path path) throws IOException;

    String readString(Path path, Charset charset) throws IOException;

    void writeString(Path path, String value, Charset charset, OpenOption... options) throws IOException;

    void writeBytes(Path path, byte[] value, OpenOption... options) throws IOException;

    ByteBuffer mapReadOnly(Path path) throws IOException;
}
