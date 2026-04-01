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
            Path source = createFixtureSource(root, "hello plugin archive");

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

    @Test
    void createsAndExtractsSevenZipArchive() throws IOException {
        CommonsCompressArchivePlugin plugin = new CommonsCompressArchivePlugin();
        Path root = Files.createTempDirectory("winmerge-plugin-7z-test");
        try {
            Path source = createFixtureSource(root, "hello seven zip");

            Path archive = root.resolve("example.7z");
            plugin.create(source, archive);
            assertTrue(Files.exists(archive));

            Path extracted = root.resolve("extracted-7z");
            plugin.extract(archive, extracted);
            assertEquals(
                "hello seven zip",
                Files.readString(extracted.resolve("nested/example.txt"), StandardCharsets.UTF_8)
            );
        } finally {
            deleteRecursively(root);
        }
    }

    @Test
    void recognizesRenamedArchiveBySignature() throws IOException {
        CommonsCompressArchivePlugin plugin = new CommonsCompressArchivePlugin();
        Path root = Files.createTempDirectory("winmerge-plugin-signature-test");
        try {
            Path source = createFixtureSource(root, "signature probe payload");
            Path archive = root.resolve("example.zip");
            plugin.create(source, archive);

            Path renamedArchive = root.resolve("example.data");
            Files.move(archive, renamedArchive);

            assertTrue(plugin.supportsArchive(renamedArchive));

            Path extracted = root.resolve("extracted-signature");
            plugin.extract(renamedArchive, extracted);
            assertEquals(
                "signature probe payload",
                Files.readString(extracted.resolve("nested/example.txt"), StandardCharsets.UTF_8)
            );
        } finally {
            deleteRecursively(root);
        }
    }

    private static Path createFixtureSource(Path root, String payload) throws IOException {
        Path source = root.resolve("source");
        Path nested = source.resolve("nested");
        Files.createDirectories(nested);
        Files.writeString(nested.resolve("example.txt"), payload, StandardCharsets.UTF_8);
        return source;
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
