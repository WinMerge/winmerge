package org.winmerge.plugins.spi;

import java.util.EnumSet;
import java.util.Objects;
import java.util.Set;

public record PluginDescriptor(
    String id,
    String displayName,
    String version,
    Set<PluginCapability> capabilities
) {
    public PluginDescriptor {
        id = Objects.requireNonNull(id, "id").trim();
        displayName = Objects.requireNonNull(displayName, "displayName").trim();
        version = Objects.requireNonNull(version, "version").trim();
        if (id.isEmpty()) {
            throw new IllegalArgumentException("id must not be blank");
        }
        if (displayName.isEmpty()) {
            throw new IllegalArgumentException("displayName must not be blank");
        }
        if (version.isEmpty()) {
            throw new IllegalArgumentException("version must not be blank");
        }
        if (capabilities == null || capabilities.isEmpty()) {
            throw new IllegalArgumentException("capabilities must not be empty");
        }
        capabilities = Set.copyOf(EnumSet.copyOf(capabilities));
    }
}
