package org.winmerge.desktop;

/**
 * Compatibility entrypoint kept while migration scripts still reference the old class name.
 */
public final class WinMergeDesktopApp {
    private WinMergeDesktopApp() {
    }

    public static void main(String[] args) {
        WinMergeApp.main(args);
    }
}
