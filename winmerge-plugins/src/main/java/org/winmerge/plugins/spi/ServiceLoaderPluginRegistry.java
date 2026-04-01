package org.winmerge.plugins.spi;

import java.io.File;
import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLClassLoader;
import java.nio.file.Files;
import java.nio.file.InvalidPathException;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.Objects;
import java.util.ServiceConfigurationError;
import java.util.ServiceLoader;
import java.util.regex.Pattern;
import java.util.stream.Stream;
import java.util.stream.StreamSupport;

public final class ServiceLoaderPluginRegistry {
    private static final String PLUGIN_DIRS_PROPERTY = "winmerge.plugins.dirs";
    private static final String PLUGIN_DIR_PROPERTY = "winmerge.plugins.dir";
    private static final String PLUGIN_DIRS_ENV = "WINMERGE_PLUGIN_DIRS";
    private static final String PLUGIN_DIR_ENV = "WINMERGE_PLUGIN_DIR";
    private static final String LEGACY_PLUGIN_DIRECTORY = "MergePlugins";

    private final List<WinMergePlugin> plugins;

    public ServiceLoaderPluginRegistry() {
        this(configuredPluginLocations(), Thread.currentThread().getContextClassLoader());
    }

    public ServiceLoaderPluginRegistry(List<Path> pluginLocations) {
        this(pluginLocations, Thread.currentThread().getContextClassLoader());
    }

    ServiceLoaderPluginRegistry(ServiceLoader<WinMergePlugin> loader) {
        this.plugins = deduplicatePlugins(StreamSupport.stream(loader.spliterator(), false).toList());
    }

    ServiceLoaderPluginRegistry(List<Path> pluginLocations, ClassLoader parentClassLoader) {
        Objects.requireNonNull(pluginLocations, "pluginLocations");
        Objects.requireNonNull(parentClassLoader, "parentClassLoader");
        this.plugins = loadPlugins(pluginLocations, parentClassLoader);
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

    private static List<WinMergePlugin> loadPlugins(List<Path> pluginLocations, ClassLoader parentClassLoader) {
        List<WinMergePlugin> discovered = new ArrayList<>(loadFromServiceLoader(parentClassLoader));

        List<URL> pluginJarUrls = resolvePluginJarUrls(normalizeDistinct(pluginLocations));
        if (!pluginJarUrls.isEmpty()) {
            URL[] urls = pluginJarUrls.toArray(URL[]::new);
            URLClassLoader pluginClassLoader = new URLClassLoader(urls, parentClassLoader);
            discovered.addAll(loadFromServiceLoader(pluginClassLoader));
        }

        return deduplicatePlugins(discovered);
    }

    private static List<WinMergePlugin> loadFromServiceLoader(ClassLoader classLoader) {
        List<WinMergePlugin> discovered = new ArrayList<>();
        ServiceLoader<WinMergePlugin> loader = ServiceLoader.load(WinMergePlugin.class, classLoader);
        var iterator = loader.iterator();
        while (true) {
            try {
                if (!iterator.hasNext()) {
                    break;
                }
                discovered.add(iterator.next());
            } catch (ServiceConfigurationError ex) {
                // Skip malformed plugin providers and continue scanning other entries.
            }
        }
        return discovered;
    }

    private static List<WinMergePlugin> deduplicatePlugins(List<WinMergePlugin> discovered) {
        Map<String, WinMergePlugin> byPluginId = new LinkedHashMap<>();
        for (WinMergePlugin plugin : discovered) {
            try {
                String key = plugin.descriptor().id().toLowerCase(Locale.ROOT);
                byPluginId.putIfAbsent(key, plugin);
            } catch (RuntimeException ignored) {
                // Ignore providers with invalid descriptors so they do not break registry construction.
            }
        }
        return List.copyOf(byPluginId.values());
    }

    private static List<URL> resolvePluginJarUrls(List<Path> locations) {
        List<URL> urls = new ArrayList<>();
        for (Path location : locations) {
            if (Files.isDirectory(location)) {
                try (Stream<Path> children = Files.list(location)) {
                    children
                        .filter(path -> Files.isRegularFile(path) && isJarFile(path))
                        .sorted()
                        .map(ServiceLoaderPluginRegistry::toUrl)
                        .forEach(urls::add);
                } catch (IOException ignored) {
                }
                continue;
            }
            if (Files.isRegularFile(location) && isJarFile(location)) {
                urls.add(toUrl(location));
            }
        }
        return urls;
    }

    private static boolean isJarFile(Path path) {
        String lower = path.getFileName().toString().toLowerCase(Locale.ROOT);
        return lower.endsWith(".jar");
    }

    private static URL toUrl(Path path) {
        try {
            return path.toUri().toURL();
        } catch (MalformedURLException ex) {
            throw new IllegalArgumentException("Invalid plugin path URL: " + path, ex);
        }
    }

    private static List<Path> configuredPluginLocations() {
        List<Path> configured = new ArrayList<>();
        addPathList(configured, System.getProperty(PLUGIN_DIRS_PROPERTY));
        addPath(configured, System.getProperty(PLUGIN_DIR_PROPERTY));
        addPathList(configured, System.getenv(PLUGIN_DIRS_ENV));
        addPath(configured, System.getenv(PLUGIN_DIR_ENV));

        Path legacyLocation = Path.of(LEGACY_PLUGIN_DIRECTORY);
        if (Files.exists(legacyLocation)) {
            configured.add(legacyLocation);
        }

        return normalizeDistinct(configured);
    }

    private static void addPathList(List<Path> target, String raw) {
        if (raw == null || raw.isBlank()) {
            return;
        }
        for (String candidate : raw.split(Pattern.quote(File.pathSeparator))) {
            addPath(target, candidate);
        }
    }

    private static void addPath(List<Path> target, String raw) {
        if (raw == null || raw.isBlank()) {
            return;
        }
        String trimmed = raw.trim();
        try {
            target.add(Path.of(trimmed));
        } catch (InvalidPathException ignored) {
        }
    }

    private static List<Path> normalizeDistinct(List<Path> locations) {
        return locations.stream()
            .map(path -> path.toAbsolutePath().normalize())
            .distinct()
            .toList();
    }
}
