package org.winmerge.shell;

import java.util.Locale;

public enum ShellPlatform {
    WINDOWS,
    MAC,
    LINUX,
    OTHER;

    public static ShellPlatform detect() {
        String osName = System.getProperty("os.name", "").toLowerCase(Locale.ROOT);
        if (osName.contains("win")) {
            return WINDOWS;
        }
        if (osName.contains("mac")) {
            return MAC;
        }
        if (osName.contains("nix") || osName.contains("nux") || osName.contains("aix")) {
            return LINUX;
        }
        return OTHER;
    }
}
