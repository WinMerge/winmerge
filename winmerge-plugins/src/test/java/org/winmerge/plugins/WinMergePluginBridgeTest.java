package org.winmerge.plugins;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.List;

import org.junit.jupiter.api.Test;

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
    void appliesPrediffPipelineInOrder() {
        WinMergePluginBridge bridge = new WinMergePluginBridge();
        String input = "first\r\nsecond   \r\nthird\t\r\n";

        String output = bridge.applyPrediffPipeline(
            input,
            List.of("prediff.normalize-line-endings", "prediff.trim-trailing-whitespace")
        );

        assertEquals("first\nsecond\nthird\n", output);
    }
}
