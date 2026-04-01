package org.winmerge.core.io;

import java.io.IOException;
import java.nio.charset.Charset;
import java.nio.file.Path;
import java.util.Arrays;
import java.util.List;

public final class UnicodeFileReader {
    private final FileSystemService fileSystem;

    public UnicodeFileReader(FileSystemService fileSystem) {
        this.fileSystem = fileSystem;
    }

    public ReadResult readAll(Path path) throws IOException {
        byte[] bytes = fileSystem.readAllBytes(path);
        FileTextEncoding encoding = detectEncoding(bytes);
        int offset = bomOffset(bytes, encoding);
        Charset charset = encoding.toCharset();
        String content = new String(bytes, offset, bytes.length - offset, charset);
        return new ReadResult(content, encoding);
    }

    public List<String> readLines(Path path) throws IOException {
        return Arrays.asList(readAll(path).content().split("\\R", -1));
    }

    public record ReadResult(String content, FileTextEncoding encoding) {
    }

    private static FileTextEncoding detectEncoding(byte[] data) {
        FileTextEncoding encoding = new FileTextEncoding();
        if (data.length >= 3 && (data[0] & 0xFF) == 0xEF && (data[1] & 0xFF) == 0xBB && (data[2] & 0xFF) == 0xBF) {
            encoding.setUnicoding(UnicodeEncoding.UTF8);
            encoding.setBom(true);
            return encoding;
        }
        if (data.length >= 2 && (data[0] & 0xFF) == 0xFF && (data[1] & 0xFF) == 0xFE) {
            encoding.setUnicoding(UnicodeEncoding.UCS2LE);
            encoding.setBom(true);
            return encoding;
        }
        if (data.length >= 2 && (data[0] & 0xFF) == 0xFE && (data[1] & 0xFF) == 0xFF) {
            encoding.setUnicoding(UnicodeEncoding.UCS2BE);
            encoding.setBom(true);
            return encoding;
        }
        encoding.setUnicoding(UnicodeEncoding.UTF8);
        encoding.setBom(false);
        return encoding;
    }

    private static int bomOffset(byte[] data, FileTextEncoding encoding) {
        if (!encoding.hasBom()) {
            return 0;
        }
        if (encoding.getUnicoding() == UnicodeEncoding.UTF8 && data.length >= 3) {
            return 3;
        }
        if ((encoding.getUnicoding() == UnicodeEncoding.UCS2LE || encoding.getUnicoding() == UnicodeEncoding.UCS2BE) && data.length >= 2) {
            return 2;
        }
        return 0;
    }
}
