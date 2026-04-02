package org.winmerge.desktop.ui.dialogs;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Objects;
import java.util.Set;

import javafx.beans.property.BooleanProperty;
import javafx.beans.property.SimpleBooleanProperty;
import javafx.collections.FXCollections;
import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.control.CheckBox;
import javafx.scene.control.Label;
import javafx.scene.control.ListCell;
import javafx.scene.control.ListView;
import javafx.scene.layout.HBox;

public final class DirColumnsDialogController {
    @FXML
    private ListView<ColumnRow> columnsList;

    @FXML
    private Label descriptionLabel;

    @FXML
    private Button upButton;

    @FXML
    private Button downButton;

    private final BooleanProperty canSubmit = new SimpleBooleanProperty(false);
    private List<ColumnRow> rows = new ArrayList<>();

    @FXML
    private void initialize() {
        requireInjected(columnsList, "columnsList");
        requireInjected(descriptionLabel, "descriptionLabel");
        requireInjected(upButton, "upButton");
        requireInjected(downButton, "downButton");

        columnsList.setCellFactory(listView -> new ColumnRowCell());
        columnsList.getSelectionModel().selectedIndexProperty().addListener((obs, oldValue, newValue) -> updateSelectionState());

        upButton.setDisable(true);
        downButton.setDisable(true);
        descriptionLabel.setText("");
    }

    public void bind(List<DirColumn> columns) {
        Objects.requireNonNull(columns, "columns");

        List<DirColumn> ordered = DirColumnsDialogLogic.sortForDisplay(columns);
        rows = new ArrayList<>(ordered.size());
        for (DirColumn column : ordered) {
            rows.add(new ColumnRow(column, column.isVisible()));
        }

        columnsList.setItems(FXCollections.observableArrayList(rows));
        if (!rows.isEmpty()) {
            columnsList.getSelectionModel().select(0);
        }
        updateCanSubmit();
    }

    public BooleanProperty canSubmitProperty() {
        return canSubmit;
    }

    @FXML
    private void onMoveUp() {
        int selected = columnsList.getSelectionModel().getSelectedIndex();
        if (selected <= 0) {
            return;
        }
        rows = moveRows(selected, selected - 1);
        refreshRows(selected - 1);
    }

    @FXML
    private void onMoveDown() {
        int selected = columnsList.getSelectionModel().getSelectedIndex();
        if (selected < 0 || selected >= rows.size() - 1) {
            return;
        }
        rows = moveRows(selected, selected + 1);
        refreshRows(selected + 1);
    }

    public List<DirColumn> buildResult() {
        Set<Integer> visibleIndexes = new HashSet<>();
        List<DirColumn> orderedColumns = new ArrayList<>(rows.size());
        for (int i = 0; i < rows.size(); i++) {
            ColumnRow row = rows.get(i);
            orderedColumns.add(row.column());
            if (row.visible()) {
                visibleIndexes.add(i);
            }
        }
        return DirColumnsDialogLogic.buildResult(orderedColumns, visibleIndexes);
    }

    private List<ColumnRow> moveRows(int fromIndex, int toIndex) {
        List<DirColumn> orderedColumns = new ArrayList<>(rows.size());
        List<Boolean> visibility = new ArrayList<>(rows.size());
        for (ColumnRow row : rows) {
            orderedColumns.add(row.column());
            visibility.add(row.visible());
        }

        List<DirColumn> reorderedColumns = DirColumnsDialogLogic.move(orderedColumns, fromIndex, toIndex);
        List<Boolean> reorderedVisibility = new ArrayList<>(visibility);
        Boolean movedVisibility = reorderedVisibility.remove(fromIndex);
        reorderedVisibility.add(toIndex, movedVisibility);

        List<ColumnRow> reorderedRows = new ArrayList<>(reorderedColumns.size());
        for (int i = 0; i < reorderedColumns.size(); i++) {
            reorderedRows.add(new ColumnRow(reorderedColumns.get(i), reorderedVisibility.get(i)));
        }
        return reorderedRows;
    }

    private void refreshRows(int selectedIndex) {
        columnsList.setItems(FXCollections.observableArrayList(rows));
        columnsList.getSelectionModel().select(selectedIndex);
        updateSelectionState();
        updateCanSubmit();
    }

    private void updateSelectionState() {
        int selected = columnsList.getSelectionModel().getSelectedIndex();
        if (selected < 0 || selected >= rows.size()) {
            descriptionLabel.setText("");
            upButton.setDisable(true);
            downButton.setDisable(true);
            return;
        }

        ColumnRow selectedRow = rows.get(selected);
        descriptionLabel.setText(selectedRow.column().description());
        upButton.setDisable(selected == 0);
        downButton.setDisable(selected == rows.size() - 1);
    }

    private void updateCanSubmit() {
        boolean hasVisible = rows.stream().anyMatch(ColumnRow::visible);
        canSubmit.set(hasVisible);
    }

    private static void requireInjected(Object field, String fieldName) {
        Objects.requireNonNull(field, () -> "Missing @FXML injection for " + fieldName);
    }

    private record ColumnRow(DirColumn column, boolean visible) {
        private ColumnRow {
            Objects.requireNonNull(column, "column");
            if (column.isForcedHidden()) {
                visible = false;
            }
        }
    }

    private final class ColumnRowCell extends ListCell<ColumnRow> {
        private final CheckBox checkBox = new CheckBox();
        private final Label nameLabel = new Label();
        private final HBox container = new HBox(8.0, checkBox, nameLabel);

        private ColumnRowCell() {
            checkBox.setOnAction(event -> {
                ColumnRow item = getItem();
                if (item == null) {
                    return;
                }
                int index = getIndex();
                if (index < 0 || index >= rows.size()) {
                    return;
                }
                rows.set(index, new ColumnRow(item.column(), checkBox.isSelected()));
                updateCanSubmit();
            });
        }

        @Override
        protected void updateItem(ColumnRow item, boolean empty) {
            super.updateItem(item, empty);
            if (empty || item == null) {
                setText(null);
                setGraphic(null);
                return;
            }
            checkBox.setSelected(item.visible());
            checkBox.setDisable(item.column().isForcedHidden());
            nameLabel.setText(item.column().name());
            setGraphic(container);
        }
    }
}
