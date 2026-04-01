package org.winmerge.plugins.builtin;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;

import org.junit.jupiter.api.Test;

class CommonsCompressArchivePluginTest {
    @Test
    void createsAndExtractsZipArchive() throws IOException {
        CommonsCompressArchivePlugin plugin = new CommonsCompressArchivePlugin();
        Path root = Files.createTempDirectory("winmerge-plugin-archive-test");
        try {
            Path source = root.resolve("source");
            Path nested = source.resolve("nested");
            Files.createDirectories(nested);
            Files.writeString(nested.resolve("example.txt"), "hello plugin archive", StandardCharsets.UTF_8);

            Path archive = root.resolve("example.zip");
            plugin.create(source, archive);
            assertTrue(Files.exists(archive));

            Path extracted = root.resolve("extracted");
            plugin.extract(archive, extracted);
            assertEquals(
                "hello plugin archive",
                Files.readString(extracted.resolve("nested/example.txt"), StandardCharsets.UTF_8)
            );
        } finally {
            deleteRecursively(root);
        }
    }

    private static void deleteRecursively(Path root) throws IOException {
        if (root == null || !Files.exists(root)) {
            return;
        }
        try (var stream = Files.walk(root)) {
            stream.sorted((a, b) -> b.getNameCount() - a.getNameCount()).forEach(path -> {
                try {
                    Files.deleteIfExists(path);
                } catch (IOException ignored) {
                }
            });
        }
    }
}
