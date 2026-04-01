package org.winmerge.plugins.builtin;

import java.util.EnumSet;
import java.util.regex.Pattern;

import org.winmerge.plugins.spi.PluginCapability;
import org.winmerge.plugins.spi.PluginDescriptor;
import org.winmerge.plugins.spi.PrediffPlugin;

public final class TrimTrailingWhitespacePrediffPlugin implements PrediffPlugin {
    private static final Pattern TRAILING_WHITESPACE = Pattern.compile("[ \\t]+$", Pattern.MULTILINE);

    private static final PluginDescriptor DESCRIPTOR = new PluginDescriptor(
        "prediff.trim-trailing-whitespace",
        "Trim Trailing Whitespace",
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
        return TRAILING_WHITESPACE.matcher(content).replaceAll("");
    }
}
