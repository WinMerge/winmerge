package org.winmerge.desktop.ui.dialogs;

import java.util.Objects;
import java.util.Optional;
import java.util.function.Consumer;
import java.util.function.Supplier;

import javafx.css.PseudoClass;
import javafx.fxml.FXML;
import javafx.scene.control.Alert;
import javafx.scene.control.Button;
import javafx.scene.control.TableColumn;
import javafx.scene.control.TableView;
import javafx.scene.control.TextField;
import javafx.scene.control.cell.CheckBoxTableCell;
import javafx.scene.control.cell.TextFieldTableCell;
import org.winmerge.desktop.i18n.I18n;

public final class FileFiltersDialogController {
    private static final PseudoClass INVALID_MASK = PseudoClass.getPseudoClass("invalid-mask");

    @FXML
    private TextField maskField;

    @FXML
    private TableView<FileFiltersDialogModel.FileFilterEntry> filtersTable;

    @FXML
    private TableColumn<FileFiltersDialogModel.FileFilterEntry, Boolean> enabledColumn;

    @FXML
    private TableColumn<FileFiltersDialogModel.FileFilterEntry, String> nameColumn;

    @FXML
    private TableColumn<FileFiltersDialogModel.FileFilterEntry, String> descriptionColumn;

    @FXML
    private TableColumn<FileFiltersDialogModel.FileFilterEntry, String> locationColumn;

    @FXML
    private Button testButton;

    @FXML
    private Button editButton;

    @FXML
    private Button deleteButton;

    private FileFiltersDialogModel model;
    private Supplier<Optional<SharedFilterDialog.FilterType>> sharedFilterChooser = Optional::empty;
    private Consumer<FileFiltersDialogModel.FileFilterEntry> testFilterAction = entry -> {
    };

    @FXML
    private void initialize() {
        requireInjected(maskField, "maskField");
        requireInjected(filtersTable, "filtersTable");
        requireInjected(enabledColumn, "enabledColumn");
        requireInjected(nameColumn, "nameColumn");
        requireInjected(descriptionColumn, "descriptionColumn");
        requireInjected(locationColumn, "locationColumn");
        requireInjected(testButton, "testButton");
        requireInjected(editButton, "editButton");
        requireInjected(deleteButton, "deleteButton");

        filtersTable.setEditable(true);

        enabledColumn.setCellValueFactory(cell -> cell.getValue().enabledProperty());
        enabledColumn.setCellFactory(CheckBoxTableCell.forTableColumn(enabledColumn));

        nameColumn.setCellValueFactory(cell -> cell.getValue().nameProperty());
        nameColumn.setCellFactory(TextFieldTableCell.forTableColumn());
        nameColumn.setOnEditCommit(event -> event.getRowValue().setName(event.getNewValue()));

        descriptionColumn.setCellValueFactory(cell -> cell.getValue().descriptionProperty());
        descriptionColumn.setCellFactory(TextFieldTableCell.forTableColumn());
        descriptionColumn.setOnEditCommit(event -> event.getRowValue().setDescription(event.getNewValue()));

        locationColumn.setCellValueFactory(cell -> cell.getValue().locationProperty());
        locationColumn.setCellFactory(TextFieldTableCell.forTableColumn());
        locationColumn.setOnEditCommit(event -> event.getRowValue().setLocation(event.getNewValue()));

        filtersTable.getSelectionModel().selectedItemProperty().addListener((obs, oldValue, newValue) -> updateButtonState());
    }

    public void bind(FileFiltersDialogModel model) {
        this.model = Objects.requireNonNull(model, "model");

        maskField.setText(model.getMaskExpression());
        maskField.textProperty().addListener((obs, oldValue, newValue) -> {
            model.setMaskExpression(newValue);
            maskField.pseudoClassStateChanged(INVALID_MASK, !FileFiltersDialogModel.isMaskValid(newValue));
        });
        maskField.pseudoClassStateChanged(INVALID_MASK, !FileFiltersDialogModel.isMaskValid(maskField.getText()));

        filtersTable.setItems(model.filters());
        updateButtonState();
    }

    public void setSharedFilterChooser(Supplier<Optional<SharedFilterDialog.FilterType>> sharedFilterChooser) {
        this.sharedFilterChooser = Objects.requireNonNull(sharedFilterChooser, "sharedFilterChooser");
    }

    public void setTestFilterAction(Consumer<FileFiltersDialogModel.FileFilterEntry> testFilterAction) {
        this.testFilterAction = Objects.requireNonNull(testFilterAction, "testFilterAction");
    }

    @FXML
    private void onNewClicked() {
        if (model == null) {
            return;
        }

        SharedFilterDialog.FilterType type = sharedFilterChooser.get().orElse(SharedFilterDialog.FilterType.PRIVATE);
        int index = model.filters().size() + 1;
        String prefix = type == SharedFilterDialog.FilterType.SHARED ? "shared" : "private";

        FileFiltersDialogModel.FileFilterEntry entry = model.addFilter(
            I18n.tr("IDS_FILEFILTER_DEFAULT_NAME_FMT", index),
            "",
            prefix + "/Filter" + index + ".flt",
            true
        );
        int addedIndex = model.filters().indexOf(entry);
        filtersTable.getSelectionModel().select(addedIndex);
        filtersTable.scrollTo(addedIndex);
    }

    @FXML
    private void onEditClicked() {
        int selectedIndex = filtersTable.getSelectionModel().getSelectedIndex();
        if (selectedIndex >= 0) {
            filtersTable.edit(selectedIndex, nameColumn);
        }
    }

    @FXML
    private void onTestClicked() {
        FileFiltersDialogModel.FileFilterEntry entry = filtersTable.getSelectionModel().getSelectedItem();
        if (entry != null) {
            testFilterAction.accept(entry);
        }
    }

    @FXML
    private void onDeleteClicked() {
        if (model == null) {
            return;
        }
        int selectedIndex = filtersTable.getSelectionModel().getSelectedIndex();
        FileFiltersDialogModel.FileFilterEntry selectedEntry = filtersTable.getSelectionModel().getSelectedItem();
        if (selectedIndex < 0 || selectedEntry == null) {
            return;
        }

        if (!model.deleteFilterAt(selectedIndex)) {
            Alert alert = new Alert(Alert.AlertType.ERROR);
            alert.setHeaderText(I18n.tr("IDS_FILTER_TITLE"));
            alert.setContentText(I18n.tr("IDS_FILEFILTER_DELETE_FAIL", selectedEntry.getLocation()));
            alert.showAndWait();
            return;
        }

        if (!model.filters().isEmpty()) {
            int index = Math.min(selectedIndex, model.filters().size() - 1);
            filtersTable.getSelectionModel().select(index);
        }
    }

    @FXML
    private void onInstallClicked() {
        if (model == null) {
            return;
        }
        int index = model.filters().size() + 1;
        FileFiltersDialogModel.FileFilterEntry entry = model.addFilter(
            I18n.tr("IDS_FILEFILTER_INSTALLED_NAME_FMT", index),
            "",
            "installed/Filter" + index + ".flt",
            true
        );
        int addedIndex = model.filters().indexOf(entry);
        filtersTable.getSelectionModel().select(addedIndex);
        filtersTable.scrollTo(addedIndex);
    }

    void flushToModel() {
        if (model != null) {
            model.setMaskExpression(maskField.getText());
        }
    }

    private void updateButtonState() {
        boolean hasSelection = filtersTable.getSelectionModel().getSelectedIndex() >= 0;
        testButton.setDisable(!hasSelection);
        editButton.setDisable(!hasSelection);
        deleteButton.setDisable(!hasSelection);
    }

    private static void requireInjected(Object field, String fieldName) {
        Objects.requireNonNull(field, () -> "Missing @FXML injection for " + fieldName);
    }
}
