package org.winmerge.core.io;

import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.io.TempDir;

import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.nio.file.Path;
import java.util.concurrent.ExecutionException;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

class FileEncodingAndIOTest {
    @TempDir
    Path tempDir;

    @Test
    void encodesAndCollatesFileTextEncoding() {
        FileTextEncoding utf8 = new FileTextEncoding();
        utf8.setCodepage(FileTextEncoding.CP_UTF_8);

        FileTextEncoding utf8Bom = new FileTextEncoding();
        utf8Bom.setUnicoding(UnicodeEncoding.UTF8);
        utf8Bom.setBom(true);

        assertEquals("UTF-8", utf8.getName());
        assertEquals("UTF-8 (B)", utf8Bom.getName());
        assertTrue(FileTextEncoding.collate(utf8, utf8Bom) < 0);
    }

    @Test
    void supportsFilesystemAdapterOperations() throws Exception {
        NioFileSystemService fileSystem = new NioFileSystemService();
        Path file = tempDir.resolve("sample.txt");

        fileSystem.writeString(file, "line1\nline2", StandardCharsets.UTF_8);

        assertTrue(fileSystem.exists(file));
        assertTrue(fileSystem.isRegularFile(file));
        assertEquals("line1\nline2", fileSystem.readString(file, StandardCharsets.UTF_8));

        ByteBuffer buffer = fileSystem.mapReadOnly(file);
        byte[] mapped = new byte[buffer.remaining()];
        buffer.get(mapped);
        assertArrayEquals("line1\nline2".getBytes(StandardCharsets.UTF_8), mapped);
    }

    @Test
    void readsAndWritesUnicodeFilesWithBom() throws Exception {
        NioFileSystemService fileSystem = new NioFileSystemService();
        UnicodeFileWriter writer = new UnicodeFileWriter(fileSystem);
        UnicodeFileReader reader = new UnicodeFileReader(fileSystem);

        FileTextEncoding encoding = new FileTextEncoding();
        encoding.setUnicoding(UnicodeEncoding.UTF8);
        encoding.setBom(true);

        Path file = tempDir.resolve("unicode.txt");
        writer.write(file, "hello ünicode", encoding, true);

        UnicodeFileReader.ReadResult result = reader.readAll(file);

        assertEquals("hello ünicode", result.content());
        assertEquals(UnicodeEncoding.UTF8, result.encoding().getUnicoding());
        assertTrue(result.encoding().hasBom());
    }

    @Test
    void executesTasksViaConcurrencyAdapter() throws ExecutionException, InterruptedException {
        try (JavaConcurrencyService concurrency = new JavaConcurrencyService()) {
            int value = concurrency.submit(() -> 41 + 1).get();
            assertEquals(42, value);
        }
    }
}
