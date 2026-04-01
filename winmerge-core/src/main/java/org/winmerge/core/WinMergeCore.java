package org.winmerge.core;

import org.winmerge.core.config.ConfigurationStore;
import org.winmerge.core.io.ConcurrencyService;
import org.winmerge.core.io.FileSystemService;

public final class WinMergeCore {
    private WinMergeCore() {
    }

    public static String version() {
        return "0.1.0-SNAPSHOT";
    }

    public static FileSystemService defaultFileSystemService() {
        return FileSystemService.createDefault();
    }

    public static ConcurrencyService defaultConcurrencyService() {
        return ConcurrencyService.createDefault();
    }

    public static ConfigurationStore defaultConfigurationStore() {
        return ConfigurationStore.createDefault();
    }
}
