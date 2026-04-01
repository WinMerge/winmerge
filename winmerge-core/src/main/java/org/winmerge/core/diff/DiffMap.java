package org.winmerge.core.diff;

import java.util.ArrayList;
import java.util.List;

public final class DiffMap {
    public static final int BAD_MAP_ENTRY = -999_999_999;
    public static final int GHOST_MAP_ENTRY = 888_888_888;

    private final List<Integer> map;

    public DiffMap() {
        this.map = new ArrayList<>();
    }

    public void initDiffMap(int lines) {
        map.clear();
        for (int i = 0; i < lines; i++) {
            map.add(BAD_MAP_ENTRY);
        }
    }

    public List<Integer> entries() {
        return map;
    }
}
