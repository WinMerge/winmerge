package org.winmerge.plugins.spi;

public interface PrediffPlugin extends WinMergePlugin {
    String apply(String content);
}
