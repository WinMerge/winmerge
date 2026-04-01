package org.winmerge.plugins;

import java.io.IOException;
import java.nio.file.Path;
import java.util.List;
import java.util.Locale;
import java.util.Objects;
import java.util.Optional;

import org.winmerge.plugins.spi.ArchivePlugin;
import org.winmerge.plugins.spi.PluginDescriptor;
import org.winmerge.plugins.spi.PrediffPlugin;
import org.winmerge.plugins.spi.ServiceLoaderPluginRegistry;
import org.winmerge.plugins.spi.WinMergePlugin;

public final class WinMergePluginBridge {
    private final ServiceLoaderPluginRegistry registry;

    public WinMergePluginBridge() {
        this(new ServiceLoaderPluginRegistry());
    }

    public WinMergePluginBridge(List<Path> pluginLocations) {
        this(new ServiceLoaderPluginRegistry(pluginLocations));
    }

    WinMergePluginBridge(ServiceLoaderPluginRegistry registry) {
        this.registry = Objects.requireNonNull(registry, "registry");
    }

    public String status() {
        int count = registry.plugins().size();
        return count == 0 ? "empty" : "ready(" + count + ")";
    }

    public List<PluginDescriptor> availablePlugins() {
        return registry.plugins().stream().map(plugin -> plugin.descriptor()).toList();
    }

    public String applyPrediffPipeline(String content, List<String> pluginIds) {
        Objects.requireNonNull(content, "content");
        Objects.requireNonNull(pluginIds, "pluginIds");
        String current = content;
        for (String pluginId : pluginIds) {
            PrediffPlugin plugin = resolvePrediffPlugin(pluginId);
            current = plugin.apply(current);
        }
        return current;
    }

    public void extractArchive(Path archivePath, Path destinationDirectory) throws IOException {
        ArchivePlugin plugin = resolveArchivePlugin(archivePath)
            .orElseThrow(() -> new IllegalArgumentException("No archive plugin supports " + archivePath));
        plugin.extract(archivePath, destinationDirectory);
    }

    public void createArchive(Path sourceDirectory, Path archivePath) throws IOException {
        ArchivePlugin plugin = resolveArchivePlugin(archivePath)
            .orElseThrow(() -> new IllegalArgumentException("No archive plugin supports " + archivePath));
        plugin.create(sourceDirectory, archivePath);
    }

    private PrediffPlugin resolvePrediffPlugin(String pluginId) {
        String id = normalizePluginId(pluginId);
        return registry.plugins(PrediffPlugin.class).stream()
            .filter(plugin -> plugin.descriptor().id().equalsIgnoreCase(id))
            .findFirst()
            .orElseThrow(() -> new IllegalArgumentException("Unknown prediff plugin id: " + pluginId));
    }

    private Optional<ArchivePlugin> resolveArchivePlugin(Path archivePath) {
        return registry.plugins(ArchivePlugin.class).stream()
            .filter(plugin -> plugin.supportsArchive(archivePath))
            .findFirst();
    }

    private static String normalizePluginId(String pluginId) {
        if (pluginId == null || pluginId.isBlank()) {
            throw new IllegalArgumentException("pluginId must not be blank");
        }
        return pluginId.trim().toLowerCase(Locale.ROOT);
    }
}
