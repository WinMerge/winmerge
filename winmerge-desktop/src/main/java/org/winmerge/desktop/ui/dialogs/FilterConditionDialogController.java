package org.winmerge.desktop.ui.dialogs;

import java.time.LocalDate;
import java.util.Objects;
import java.util.Optional;

import javafx.beans.property.BooleanProperty;
import javafx.beans.property.SimpleBooleanProperty;
import javafx.collections.FXCollections;
import javafx.fxml.FXML;
import javafx.scene.control.CheckBox;
import javafx.scene.control.ComboBox;
import javafx.scene.control.DatePicker;
import javafx.scene.control.Label;
import javafx.scene.control.ListCell;
import javafx.scene.control.TextField;

public final class FilterConditionDialogController {
    @FXML
    private Label lhsLabel;

    @FXML
    private ComboBox<FilterConditionDialogLogic.OperatorOption> operatorCombo;

    @FXML
    private TextField value1Field;

    @FXML
    private TextField value2Field;

    @FXML
    private DatePicker value1DatePicker;

    @FXML
    private DatePicker value2DatePicker;

    @FXML
    private CheckBox caseSensitiveCheck;

    @FXML
    private Label expressionPreviewLabel;

    private final BooleanProperty canSubmit = new SimpleBooleanProperty(false);

    private FilterConditionRequest request;

    @FXML
    private void initialize() {
        requireInjected(lhsLabel, "lhsLabel");
        requireInjected(operatorCombo, "operatorCombo");
        requireInjected(value1Field, "value1Field");
        requireInjected(value2Field, "value2Field");
        requireInjected(value1DatePicker, "value1DatePicker");
        requireInjected(value2DatePicker, "value2DatePicker");
        requireInjected(caseSensitiveCheck, "caseSensitiveCheck");
        requireInjected(expressionPreviewLabel, "expressionPreviewLabel");

        operatorCombo.setCellFactory(list -> new ListCell<>() {
            @Override
            protected void updateItem(FilterConditionDialogLogic.OperatorOption item, boolean empty) {
                super.updateItem(item, empty);
                setText(empty || item == null ? "" : item.label());
            }
        });
        operatorCombo.setButtonCell(new ListCell<>() {
            @Override
            protected void updateItem(FilterConditionDialogLogic.OperatorOption item, boolean empty) {
                super.updateItem(item, empty);
                setText(empty || item == null ? "" : item.label());
            }
        });

        operatorCombo.valueProperty().addListener((obs, oldValue, newValue) -> refreshState());
        value1Field.textProperty().addListener((obs, oldValue, newValue) -> refreshState());
        value2Field.textProperty().addListener((obs, oldValue, newValue) -> refreshState());
        value1DatePicker.valueProperty().addListener((obs, oldValue, newValue) -> refreshState());
        value2DatePicker.valueProperty().addListener((obs, oldValue, newValue) -> refreshState());
        caseSensitiveCheck.selectedProperty().addListener((obs, oldValue, newValue) -> refreshState());
    }

    public void bind(FilterConditionRequest request) {
        this.request = Objects.requireNonNull(request, "request");

        lhsLabel.setText(request.lhsExpression());
        operatorCombo.setItems(FXCollections.observableArrayList(
            FilterConditionDialogLogic.operatorsForField(request.field(), request.lhsExpression())
        ));

        FilterConditionDialogLogic.OperatorOption matchingOperator = operatorCombo.getItems()
            .stream()
            .filter(option -> option.expressionTemplate().equals(request.initialOperatorExpression()))
            .findFirst()
            .orElse(operatorCombo.getItems().isEmpty() ? null : operatorCombo.getItems().get(0));
        operatorCombo.setValue(matchingOperator);

        value1Field.setText(request.initialValue1());
        value2Field.setText(request.initialValue2());
        value1DatePicker.setValue(parseDateOrNull(request.initialValue1()));
        value2DatePicker.setValue(parseDateOrNull(request.initialValue2()));
        caseSensitiveCheck.setSelected(request.initialCaseSensitive());

        refreshState();
    }

    public BooleanProperty canSubmitProperty() {
        return canSubmit;
    }

    public Optional<FilterConditionResult> buildResult() {
        FilterConditionDialogLogic.OperatorOption operator = operatorCombo.getValue();
        if (request == null || operator == null) {
            return Optional.empty();
        }

        return FilterConditionDialogLogic.buildResult(
            request,
            operator,
            value1Field.getText(),
            value2Field.getText(),
            value1DatePicker.getValue(),
            value2DatePicker.getValue(),
            caseSensitiveCheck.isSelected()
        );
    }

    private void refreshState() {
        if (request == null) {
            canSubmit.set(false);
            expressionPreviewLabel.setText("");
            return;
        }

        FilterConditionDialogLogic.OperatorOption selected = operatorCombo.getValue();
        boolean dateField = FilterConditionDialogLogic.isDateField(request.field(), request.lhsExpression());
        boolean showSecondValue = selected != null && FilterConditionDialogLogic.usesSecondValue(selected.expressionTemplate());

        setVisibleAndManaged(value1Field, !dateField);
        setVisibleAndManaged(value2Field, !dateField && showSecondValue);
        setVisibleAndManaged(value1DatePicker, dateField);
        setVisibleAndManaged(value2DatePicker, dateField && showSecondValue);

        Optional<FilterConditionResult> result = buildResult();
        expressionPreviewLabel.setText(result.map(FilterConditionResult::expression).orElse(""));
        canSubmit.set(result.isPresent());
    }

    private static LocalDate parseDateOrNull(String value) {
        if (value == null || value.isBlank()) {
            return null;
        }
        try {
            return LocalDate.parse(value.trim());
        } catch (Exception ignored) {
            return null;
        }
    }

    private static void setVisibleAndManaged(javafx.scene.Node node, boolean visible) {
        node.setVisible(visible);
        node.setManaged(visible);
    }

    private static void requireInjected(Object field, String fieldName) {
        Objects.requireNonNull(field, () -> "Missing @FXML injection for " + fieldName);
    }
}
