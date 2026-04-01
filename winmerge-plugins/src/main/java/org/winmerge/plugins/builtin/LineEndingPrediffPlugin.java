package org.winmerge.plugins.builtin;

import java.util.EnumSet;

import org.winmerge.plugins.spi.PluginCapability;
import org.winmerge.plugins.spi.PluginDescriptor;
import org.winmerge.plugins.spi.PrediffPlugin;

public final class LineEndingPrediffPlugin implements PrediffPlugin {
    private static final PluginDescriptor DESCRIPTOR = new PluginDescriptor(
        "prediff.normalize-line-endings",
        "Normalize Line Endings",
        "1.0.0",
        EnumSet.of(PluginCapability.PREDIFF)
    );

    @Override
    public PluginDescriptor descriptor() {
        return DESCRIPTOR;
    }

    @Override
    public String apply(String content) {
        if (content == null || content.isEmpty()) {
            return "";
        }
        return content.replace("\r\n", "\n").replace('\r', '\n');
    }
}
