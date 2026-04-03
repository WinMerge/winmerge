package org.winmerge.desktop.ui.dialogs;

import java.util.Objects;

import javafx.beans.value.ChangeListener;
import javafx.fxml.FXML;
import javafx.geometry.Pos;
import javafx.scene.control.Button;
import javafx.scene.control.CheckBox;
import javafx.scene.control.Label;
import javafx.scene.control.ListCell;
import javafx.scene.control.ListView;
import javafx.scene.control.TextField;
import javafx.scene.input.MouseButton;
import javafx.scene.layout.HBox;
import javafx.scene.layout.Priority;

public final class LineFiltersDialogController {
    @FXML
    private CheckBox ignoreRegExpCheck;

    @FXML
    private ListView<LineFiltersDialogModel.LineFilterEntry> filtersList;

    @FXML
    private Button editButton;

    @FXML
    private Button removeButton;

    private LineFiltersDialogModel model;

    @FXML
    private void initialize() {
        requireInjected(ignoreRegExpCheck, "ignoreRegExpCheck");
        requireInjected(filtersList, "filtersList");
        requireInjected(editButton, "editButton");
        requireInjected(removeButton, "removeButton");

        filtersList.setEditable(true);
        filtersList.setCellFactory(list -> new EditableLineFilterCell());
        filtersList.getSelectionModel().selectedItemProperty().addListener((obs, oldValue, newValue) -> updateButtonState());
    }

    public void bind(LineFiltersDialogModel model) {
        Objects.requireNonNull(model, "model");

        if (this.model != null) {
            ignoreRegExpCheck.selectedProperty().unbindBidirectional(this.model.ignoreRegExpErrorsProperty());
        }

        this.model = model;
        filtersList.setItems(model.entries());
        ignoreRegExpCheck.selectedProperty().bindBidirectional(model.ignoreRegExpErrorsProperty());
        updateButtonState();
    }

    @FXML
    private void onAddClicked() {
        if (model == null) {
            return;
        }
        LineFiltersDialogModel.LineFilterEntry entry = model.addEntry("", false);
        int index = model.entries().indexOf(entry);
        filtersList.getSelectionModel().select(index);
        filtersList.scrollTo(index);
        filtersList.edit(index);
    }

    @FXML
    private void onEditClicked() {
        int index = filtersList.getSelectionModel().getSelectedIndex();
        if (index >= 0) {
            filtersList.edit(index);
        }
    }

    @FXML
    private void onRemoveClicked() {
        if (model == null) {
            return;
        }
        int index = filtersList.getSelectionModel().getSelectedIndex();
        model.removeEntryAt(index);
        if (!model.entries().isEmpty()) {
            int newIndex = Math.min(index, model.entries().size() - 1);
            filtersList.getSelectionModel().select(newIndex);
        }
    }

    void flushToModel() {
        if (model != null) {
            model.setIgnoreRegExpErrors(ignoreRegExpCheck.isSelected());
        }
    }

    private void updateButtonState() {
        boolean hasSelection = filtersList.getSelectionModel().getSelectedIndex() >= 0;
        editButton.setDisable(!hasSelection);
        removeButton.setDisable(!hasSelection);
    }

    private static void requireInjected(Object field, String fieldName) {
        Objects.requireNonNull(field, () -> "Missing @FXML injection for " + fieldName);
    }

    private static final class EditableLineFilterCell extends ListCell<LineFiltersDialogModel.LineFilterEntry> {
        private final HBox row = new HBox(8.0);
        private final CheckBox enabledCheck = new CheckBox();
        private final Label textLabel = new Label();
        private final TextField editorField = new TextField();

        private final ChangeListener<String> patternListener = (obs, oldValue, newValue) -> {
            if (!editorField.isVisible()) {
                textLabel.setText(newValue == null ? "" : newValue);
            }
        };

        private final ChangeListener<Boolean> enabledListener = (obs, oldValue, newValue) -> {
            if (!Objects.equals(enabledCheck.isSelected(), newValue)) {
                enabledCheck.setSelected(Boolean.TRUE.equals(newValue));
            }
        };

        private LineFiltersDialogModel.LineFilterEntry currentItem;

        EditableLineFilterCell() {
            row.setAlignment(Pos.CENTER_LEFT);
            HBox.setHgrow(textLabel, Priority.ALWAYS);
            HBox.setHgrow(editorField, Priority.ALWAYS);
            row.getChildren().addAll(enabledCheck, textLabel, editorField);

            editorField.setManaged(false);
            editorField.setVisible(false);

            setOnMouseClicked(event -> {
                if (event.getButton() == MouseButton.PRIMARY && event.getClickCount() == 2 && !isEmpty()) {
                    startEdit();
                }
            });

            editorField.setOnAction(event -> commitFromEditor());
            editorField.focusedProperty().addListener((obs, oldValue, newValue) -> {
                if (!newValue && editorField.isVisible()) {
                    commitFromEditor();
                }
            });

            enabledCheck.selectedProperty().addListener((obs, oldValue, newValue) -> {
                if (currentItem != null && currentItem.isEnabled() != newValue) {
                    currentItem.setEnabled(newValue);
                }
            });
        }

        @Override
        protected void updateItem(LineFiltersDialogModel.LineFilterEntry item, boolean empty) {
            if (currentItem != null) {
                currentItem.patternProperty().removeListener(patternListener);
                currentItem.enabledProperty().removeListener(enabledListener);
            }

            super.updateItem(item, empty);
            currentItem = item;

            if (empty || item == null) {
                setText(null);
                setGraphic(null);
                return;
            }

            item.patternProperty().addListener(patternListener);
            item.enabledProperty().addListener(enabledListener);

            textLabel.setText(item.getPattern());
            enabledCheck.setSelected(item.isEnabled());
            showLabelMode();
            setGraphic(row);
        }

        @Override
        public void startEdit() {
            if (getItem() == null) {
                return;
            }
            super.startEdit();
            editorField.setText(getItem().getPattern());
            textLabel.setManaged(false);
            textLabel.setVisible(false);
            editorField.setManaged(true);
            editorField.setVisible(true);
            editorField.requestFocus();
            editorField.selectAll();
        }

        @Override
        public void cancelEdit() {
            super.cancelEdit();
            showLabelMode();
        }

        private void commitFromEditor() {
            if (getItem() == null) {
                cancelEdit();
                return;
            }
            String value = editorField.getText() == null ? "" : editorField.getText();
            getItem().setPattern(value);
            super.commitEdit(getItem());
            showLabelMode();
        }

        private void showLabelMode() {
            textLabel.setManaged(true);
            textLabel.setVisible(true);
            editorField.setManaged(false);
            editorField.setVisible(false);
        }
    }
}
