package org.winmerge.plugins.fixtures;

import java.util.EnumSet;

import org.winmerge.plugins.spi.PluginCapability;
import org.winmerge.plugins.spi.PluginDescriptor;
import org.winmerge.plugins.spi.PrediffPlugin;

public final class ExternalFixturePrediffPlugin implements PrediffPlugin {
    private static final PluginDescriptor DESCRIPTOR = new PluginDescriptor(
        "prediff.external-fixture",
        "External Fixture Prediff Plugin",
        "1.0.0",
        EnumSet.of(PluginCapability.PREDIFF)
    );

    @Override
    public PluginDescriptor descriptor() {
        return DESCRIPTOR;
    }

    @Override
    public String apply(String content) {
        return content + "|external";
    }
}
