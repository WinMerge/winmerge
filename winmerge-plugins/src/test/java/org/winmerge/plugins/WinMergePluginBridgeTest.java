package org.winmerge.plugins;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.io.IOException;
import java.io.InputStream;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.List;
import java.util.jar.JarEntry;
import java.util.jar.JarOutputStream;

import org.junit.jupiter.api.Test;
import org.winmerge.plugins.fixtures.ExternalFixturePrediffPlugin;

class WinMergePluginBridgeTest {
    @Test
    void discoversBundledPluginsViaServiceLoader() {
        WinMergePluginBridge bridge = new WinMergePluginBridge();

        List<String> ids = bridge.availablePlugins().stream().map(descriptor -> descriptor.id()).toList();

        assertTrue(ids.contains("archive.commons-compress"));
        assertTrue(ids.contains("prediff.normalize-line-endings"));
        assertTrue(ids.contains("prediff.trim-trailing-whitespace"));
        assertTrue(bridge.status().startsWith("ready("));
    }

    @Test
    void discoversExternalPluginFromConfiguredPluginDirectory() throws IOException {
        Path root = Files.createTempDirectory("winmerge-external-plugin-test");
        try {
            Path pluginDirectory = root.resolve("MergePlugins");
            Files.createDirectories(pluginDirectory);
            Path pluginJar = pluginDirectory.resolve("external-fixture-plugin.jar");
            writeExternalFixturePluginJar(pluginJar);

            WinMergePluginBridge bridge = new WinMergePluginBridge(List.of(pluginDirectory));
            List<String> ids = bridge.availablePlugins().stream().map(descriptor -> descriptor.id()).toList();

            assertTrue(ids.contains("prediff.external-fixture"));
            assertEquals("content|external", bridge.applyPrediffPipeline("content", List.of("prediff.external-fixture")));
        } finally {
            deleteRecursively(root);
        }
    }

    @Test
    void appliesPrediffPipelineInOrder() {
        WinMergePluginBridge bridge = new WinMergePluginBridge();
        String input = "first\r\nsecond   \r\nthird\t\r\n";

        String output = bridge.applyPrediffPipeline(
            input,
            List.of("prediff.normalize-line-endings", "prediff.trim-trailing-whitespace")
        );

        assertEquals("first\nsecond\nthird\n", output);
    }

    private static void writeExternalFixturePluginJar(Path jarPath) throws IOException {
        String className = ExternalFixturePrediffPlugin.class.getName();
        String classResourcePath = className.replace('.', '/') + ".class";
        String servicesPath = "META-INF/services/org.winmerge.plugins.spi.WinMergePlugin";

        try (JarOutputStream jarOutput = new JarOutputStream(Files.newOutputStream(jarPath))) {
            try (InputStream classBytes = ExternalFixturePrediffPlugin.class
                .getClassLoader()
                .getResourceAsStream(classResourcePath)) {
                assertNotNull(classBytes, "Fixture class bytes must be available on test classpath");
                jarOutput.putNextEntry(new JarEntry(classResourcePath));
                classBytes.transferTo(jarOutput);
                jarOutput.closeEntry();
            }

            jarOutput.putNextEntry(new JarEntry(servicesPath));
            byte[] serviceDescriptor = (className + "\n").getBytes(StandardCharsets.UTF_8);
            jarOutput.write(serviceDescriptor);
            jarOutput.closeEntry();
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
