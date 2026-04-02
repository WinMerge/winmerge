package org.winmerge.desktop.ui.dialogs;

import java.util.Arrays;
import java.util.List;
import java.util.Objects;

public record DirSelectFilesRequest(
    DirSelectFilesPane pane1,
    DirSelectFilesPane pane2,
    DirSelectFilesPane pane3,
    List<Integer> initialSelectedButtons
) {
    public DirSelectFilesRequest {
        pane1 = Objects.requireNonNull(pane1, "pane1");
        pane2 = Objects.requireNonNull(pane2, "pane2");
        pane3 = pane3 == null ? null : pane3;
        initialSelectedButtons = initialSelectedButtons == null ? List.of() : List.copyOf(initialSelectedButtons);
    }

    public boolean isThreeWay() {
        return pane3 != null;
    }

    public record DirSelectFilesPane(String label, String[] filePaths, boolean[] available) {
        public DirSelectFilesPane {
            label = label == null ? "" : label;
            filePaths = normalizePaths(filePaths);
            available = normalizeAvailability(available);
        }

        private static String[] normalizePaths(String[] values) {
            String[] normalized = new String[] {"", "", ""};
            if (values == null) {
                return normalized;
            }
            for (int i = 0; i < normalized.length && i < values.length; i++) {
                normalized[i] = values[i] == null ? "" : values[i];
            }
            return normalized;
        }

        private static boolean[] normalizeAvailability(boolean[] values) {
            boolean[] normalized = new boolean[] {false, false, false};
            if (values == null) {
                return normalized;
            }
            System.arraycopy(values, 0, normalized, 0, Math.min(values.length, normalized.length));
            return normalized;
        }

        @Override
        public String[] filePaths() {
            return Arrays.copyOf(filePaths, filePaths.length);
        }

        @Override
        public boolean[] available() {
            return Arrays.copyOf(available, available.length);
        }
    }
}
