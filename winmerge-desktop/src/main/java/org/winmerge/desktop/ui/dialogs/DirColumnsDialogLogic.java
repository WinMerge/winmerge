package org.winmerge.desktop.ui.dialogs;

import java.util.ArrayList;
import java.util.Comparator;
import java.util.List;
import java.util.Set;

final class DirColumnsDialogLogic {
    private DirColumnsDialogLogic() {
    }

    static List<DirColumn> sortForDisplay(List<DirColumn> columns) {
        List<IndexedColumn> indexed = new ArrayList<>(columns.size());
        for (int i = 0; i < columns.size(); i++) {
            indexed.add(new IndexedColumn(i, columns.get(i)));
        }
        indexed.sort(
            Comparator
                .comparingInt((IndexedColumn entry) -> entry.column.logCol() < 0 ? Integer.MAX_VALUE : entry.column.logCol())
                .thenComparingInt(entry -> entry.column.phyCol() < 0 ? Integer.MAX_VALUE : entry.column.phyCol())
                .thenComparingInt(entry -> entry.index)
        );

        List<DirColumn> ordered = new ArrayList<>(indexed.size());
        for (IndexedColumn entry : indexed) {
            ordered.add(entry.column);
        }
        return ordered;
    }

    static List<DirColumn> move(List<DirColumn> columns, int fromIndex, int toIndex) {
        if (fromIndex < 0 || fromIndex >= columns.size() || toIndex < 0 || toIndex >= columns.size()) {
            return List.copyOf(columns);
        }
        if (fromIndex == toIndex) {
            return List.copyOf(columns);
        }

        List<DirColumn> reordered = new ArrayList<>(columns);
        DirColumn moved = reordered.remove(fromIndex);
        reordered.add(toIndex, moved);
        return reordered;
    }

    static List<DirColumn> buildResult(List<DirColumn> currentOrder, Set<Integer> visibleIndexes) {
        List<DirColumn> visible = new ArrayList<>();
        List<DirColumn> hidden = new ArrayList<>();

        for (int i = 0; i < currentOrder.size(); i++) {
            DirColumn column = currentOrder.get(i);
            boolean forcedHidden = column.logCol() < 0;
            boolean checkedVisible = visibleIndexes.contains(i);
            if (!forcedHidden && checkedVisible) {
                visible.add(column);
            } else {
                hidden.add(column);
            }
        }

        List<DirColumn> result = new ArrayList<>(currentOrder.size());
        int nextVisibleOrder = 0;
        for (DirColumn column : visible) {
            result.add(new DirColumn(column.name(), column.description(), nextVisibleOrder, nextVisibleOrder));
            nextVisibleOrder++;
        }
        for (DirColumn column : hidden) {
            result.add(new DirColumn(column.name(), column.description(), -1, -1));
        }
        return result;
    }

    private record IndexedColumn(int index, DirColumn column) {
    }
}
