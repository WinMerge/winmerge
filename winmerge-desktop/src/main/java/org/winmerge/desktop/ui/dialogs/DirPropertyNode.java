package org.winmerge.desktop.ui.dialogs;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public final class DirPropertyNode {
    private final String name;
    private final String canonicalName;
    private final String displayName;
    private final List<DirPropertyNode> children;
    private boolean selected;

    public DirPropertyNode(
        String name,
        String canonicalName,
        String displayName,
        boolean selected,
        List<DirPropertyNode> children
    ) {
        this.name = name == null ? "" : name;
        this.canonicalName = canonicalName == null ? "" : canonicalName;
        this.displayName = displayName == null ? "" : displayName;
        this.selected = selected;
        this.children = Collections.unmodifiableList(new ArrayList<>(children == null ? List.of() : children));
    }

    public String name() {
        return name;
    }

    public String canonicalName() {
        return canonicalName;
    }

    public String displayName() {
        return displayName;
    }

    public boolean selected() {
        return selected;
    }

    public void setSelected(boolean selected) {
        this.selected = selected;
    }

    public List<DirPropertyNode> children() {
        return children;
    }

    public boolean isLeaf() {
        return children.isEmpty() && !canonicalName.isBlank();
    }
}
