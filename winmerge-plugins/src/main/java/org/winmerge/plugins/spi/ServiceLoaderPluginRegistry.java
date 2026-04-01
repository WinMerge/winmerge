package org.winmerge.plugins.spi;

import java.util.List;
import java.util.ServiceLoader;
import java.util.stream.StreamSupport;

public final class ServiceLoaderPluginRegistry {
    private final List<WinMergePlugin> plugins;

    public ServiceLoaderPluginRegistry() {
        this(ServiceLoader.load(WinMergePlugin.class));
    }

    ServiceLoaderPluginRegistry(ServiceLoader<WinMergePlugin> loader) {
        this.plugins = StreamSupport.stream(loader.spliterator(), false).toList();
    }

    public List<WinMergePlugin> plugins() {
        return plugins;
    }

    public <T extends WinMergePlugin> List<T> plugins(Class<T> type) {
        return plugins.stream()
            .filter(type::isInstance)
            .map(type::cast)
            .toList();
    }
}
