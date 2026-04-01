package org.winmerge.core.io;

import java.io.IOException;
import java.nio.charset.Charset;
import java.nio.file.Path;

public final class UnicodeFileWriter {
    private final FileSystemService fileSystem;

    public UnicodeFileWriter(FileSystemService fileSystem) {
        this.fileSystem = fileSystem;
    }

    public void write(Path path, String content, FileTextEncoding encoding) throws IOException {
        write(path, content, encoding, encoding.hasBom());
    }

    public void write(Path path, String content, FileTextEncoding encoding, boolean includeBom) throws IOException {
        Charset charset = encoding.toCharset();
        byte[] payload = content.getBytes(charset);

        byte[] bytes;
        if (includeBom) {
            byte[] bom = bomBytes(encoding);
            bytes = new byte[bom.length + payload.length];
            System.arraycopy(bom, 0, bytes, 0, bom.length);
            System.arraycopy(payload, 0, bytes, bom.length, payload.length);
        } else {
            bytes = payload;
        }

        fileSystem.writeBytes(path, bytes);
    }

    private static byte[] bomBytes(FileTextEncoding encoding) {
        return switch (encoding.getUnicoding()) {
            case UTF8 -> new byte[] {(byte) 0xEF, (byte) 0xBB, (byte) 0xBF};
            case UCS2LE -> new byte[] {(byte) 0xFF, (byte) 0xFE};
            case UCS2BE -> new byte[] {(byte) 0xFE, (byte) 0xFF};
            case NONE -> new byte[0];
        };
    }
}
