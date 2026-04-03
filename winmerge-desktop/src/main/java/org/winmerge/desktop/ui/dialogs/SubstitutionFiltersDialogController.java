package org.winmerge.desktop.ui.dialogs;

import java.util.Objects;

import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.control.CheckBox;
import javafx.scene.control.TableColumn;
import javafx.scene.control.TableView;
import javafx.scene.control.cell.CheckBoxTableCell;
import javafx.scene.control.cell.TextFieldTableCell;
import org.winmerge.desktop.i18n.I18n;

public final class SubstitutionFiltersDialogController {
    @FXML
    private CheckBox enabledCheck;

    @FXML
    private TableView<SubstitutionFiltersDialogModel.SubstitutionFilterEntry> filtersTable;

    @FXML
    private TableColumn<SubstitutionFiltersDialogModel.SubstitutionFilterEntry, Boolean> enabledColumn;

    @FXML
    private TableColumn<SubstitutionFiltersDialogModel.SubstitutionFilterEntry, String> findWhatColumn;

    @FXML
    private TableColumn<SubstitutionFiltersDialogModel.SubstitutionFilterEntry, String> replaceWithColumn;

    @FXML
    private TableColumn<SubstitutionFiltersDialogModel.SubstitutionFilterEntry, Boolean> regexColumn;

    @FXML
    private TableColumn<SubstitutionFiltersDialogModel.SubstitutionFilterEntry, Boolean> caseSensitiveColumn;

    @FXML
    private TableColumn<SubstitutionFiltersDialogModel.SubstitutionFilterEntry, Boolean> wholeWordColumn;

    @FXML
    private Button removeButton;

    @FXML
    private Button clearButton;

    private SubstitutionFiltersDialogModel model;

    @FXML
    private void initialize() {
        requireInjected(enabledCheck, "enabledCheck");
        requireInjected(filtersTable, "filtersTable");
        requireInjected(enabledColumn, "enabledColumn");
        requireInjected(findWhatColumn, "findWhatColumn");
        requireInjected(replaceWithColumn, "replaceWithColumn");
        requireInjected(regexColumn, "regexColumn");
        requireInjected(caseSensitiveColumn, "caseSensitiveColumn");
        requireInjected(wholeWordColumn, "wholeWordColumn");
        requireInjected(removeButton, "removeButton");
        requireInjected(clearButton, "clearButton");

        filtersTable.setEditable(true);

        enabledColumn.setCellValueFactory(cell -> cell.getValue().enabledProperty());
        enabledColumn.setCellFactory(CheckBoxTableCell.forTableColumn(enabledColumn));

        findWhatColumn.setCellValueFactory(cell -> cell.getValue().findWhatProperty());
        findWhatColumn.setCellFactory(TextFieldTableCell.forTableColumn());
        findWhatColumn.setOnEditCommit(event -> event.getRowValue().setFindWhat(event.getNewValue()));

        replaceWithColumn.setCellValueFactory(cell -> cell.getValue().replaceWithProperty());
        replaceWithColumn.setCellFactory(TextFieldTableCell.forTableColumn());
        replaceWithColumn.setOnEditCommit(event -> event.getRowValue().setReplaceWith(event.getNewValue()));

        regexColumn.setCellValueFactory(cell -> cell.getValue().useRegExpProperty());
        regexColumn.setCellFactory(CheckBoxTableCell.forTableColumn(regexColumn));

        caseSensitiveColumn.setCellValueFactory(cell -> cell.getValue().caseSensitiveProperty());
        caseSensitiveColumn.setCellFactory(CheckBoxTableCell.forTableColumn(caseSensitiveColumn));

        wholeWordColumn.setCellValueFactory(cell -> cell.getValue().matchWholeWordOnlyProperty());
        wholeWordColumn.setCellFactory(CheckBoxTableCell.forTableColumn(wholeWordColumn));

        filtersTable.getSelectionModel().selectedItemProperty().addListener((obs, oldValue, newValue) -> updateButtonState());
    }

    public void bind(SubstitutionFiltersDialogModel model) {
        Objects.requireNonNull(model, "model");

        if (this.model != null) {
            enabledCheck.selectedProperty().unbindBidirectional(this.model.enabledProperty());
        }

        this.model = model;
        filtersTable.setItems(model.entries());
        enabledCheck.selectedProperty().bindBidirectional(model.enabledProperty());
        updateButtonState();
    }

    @FXML
    private void onAddClicked() {
        if (model == null) {
            return;
        }
        String placeholder = I18n.tr("IDS_SUBSTITUTION_FILTERS_EDIT_PLACEHOLDER");
        SubstitutionFiltersDialogModel.SubstitutionFilterEntry entry = model.addEntry(
            placeholder,
            placeholder,
            false,
            false,
            false,
            true
        );
        int index = model.entries().indexOf(entry);
        filtersTable.getSelectionModel().select(index);
        filtersTable.scrollTo(index);
    }

    @FXML
    private void onRemoveClicked() {
        if (model == null) {
            return;
        }
        int index = filtersTable.getSelectionModel().getSelectedIndex();
        model.removeEntryAt(index);
        updateButtonState();
    }

    @FXML
    private void onClearClicked() {
        if (model == null) {
            return;
        }
        model.clearEntries();
        updateButtonState();
    }

    void flushToModel() {
        if (model != null) {
            model.setEnabled(enabledCheck.isSelected());
        }
    }

    private void updateButtonState() {
        boolean hasSelection = filtersTable.getSelectionModel().getSelectedIndex() >= 0;
        removeButton.setDisable(!hasSelection);
        clearButton.setDisable(filtersTable.getItems() == null || filtersTable.getItems().isEmpty());
    }

    private static void requireInjected(Object field, String fieldName) {
        Objects.requireNonNull(field, () -> "Missing @FXML injection for " + fieldName);
    }
}
