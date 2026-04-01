package org.winmerge.desktop.ui.dialogs;

import java.util.Objects;
import java.util.Optional;

import javafx.beans.property.BooleanProperty;
import javafx.beans.property.SimpleBooleanProperty;
import javafx.fxml.FXML;
import javafx.scene.control.CheckBox;
import javafx.scene.control.RadioButton;
import javafx.scene.control.ToggleGroup;

public final class ComparisonResultFilterDialogController {
    @FXML
    private RadioButton includeRadio;

    @FXML
    private RadioButton excludeRadio;

    @FXML
    private CheckBox identicalCheck;

    @FXML
    private CheckBox differentCheck;

    @FXML
    private CheckBox leftOnlyCheck;

    @FXML
    private CheckBox rightOnlyCheck;

    @FXML
    private CheckBox skippedCheck;

    @FXML
    private CheckBox middleOnlyCheck;

    @FXML
    private CheckBox leftOnlyDifferentCheck;

    @FXML
    private CheckBox middleOnlyDifferentCheck;

    @FXML
    private CheckBox rightOnlyDifferentCheck;

    @FXML
    private CheckBox leftOnlyMissingCheck;

    @FXML
    private CheckBox middleOnlyMissingCheck;

    @FXML
    private CheckBox rightOnlyMissingCheck;

    private final BooleanProperty canSubmit = new SimpleBooleanProperty(false);
    private boolean threeWay;

    @FXML
    private void initialize() {
        requireInjected(includeRadio, "includeRadio");
        requireInjected(excludeRadio, "excludeRadio");
        requireInjected(identicalCheck, "identicalCheck");
        requireInjected(differentCheck, "differentCheck");
        requireInjected(leftOnlyCheck, "leftOnlyCheck");
        requireInjected(rightOnlyCheck, "rightOnlyCheck");
        requireInjected(skippedCheck, "skippedCheck");
        requireInjected(middleOnlyCheck, "middleOnlyCheck");
        requireInjected(leftOnlyDifferentCheck, "leftOnlyDifferentCheck");
        requireInjected(middleOnlyDifferentCheck, "middleOnlyDifferentCheck");
        requireInjected(rightOnlyDifferentCheck, "rightOnlyDifferentCheck");
        requireInjected(leftOnlyMissingCheck, "leftOnlyMissingCheck");
        requireInjected(middleOnlyMissingCheck, "middleOnlyMissingCheck");
        requireInjected(rightOnlyMissingCheck, "rightOnlyMissingCheck");

        ToggleGroup toggleGroup = new ToggleGroup();
        includeRadio.setToggleGroup(toggleGroup);
        excludeRadio.setToggleGroup(toggleGroup);
        includeRadio.setSelected(true);
    }

    public void bind(boolean threeWay) {
        this.threeWay = threeWay;
        setThreeWayControlsVisible(threeWay);
        registerListeners();
        updateCanSubmit();
    }

    public BooleanProperty canSubmitProperty() {
        return canSubmit;
    }

    public Optional<String> buildExpression() {
        return ComparisonResultFilterExpressionBuilder.build(snapshotSelection());
    }

    private void registerListeners() {
        includeRadio.selectedProperty().addListener((obs, oldValue, newValue) -> updateCanSubmit());
        excludeRadio.selectedProperty().addListener((obs, oldValue, newValue) -> updateCanSubmit());

        for (CheckBox checkBox : new CheckBox[] {
            identicalCheck,
            differentCheck,
            leftOnlyCheck,
            rightOnlyCheck,
            skippedCheck,
            middleOnlyCheck,
            leftOnlyDifferentCheck,
            middleOnlyDifferentCheck,
            rightOnlyDifferentCheck,
            leftOnlyMissingCheck,
            middleOnlyMissingCheck,
            rightOnlyMissingCheck
        }) {
            checkBox.selectedProperty().addListener((obs, oldValue, newValue) -> updateCanSubmit());
        }
    }

    private void setThreeWayControlsVisible(boolean visible) {
        setVisibleAndManaged(middleOnlyCheck, visible);
        setVisibleAndManaged(leftOnlyDifferentCheck, visible);
        setVisibleAndManaged(middleOnlyDifferentCheck, visible);
        setVisibleAndManaged(rightOnlyDifferentCheck, visible);
        setVisibleAndManaged(leftOnlyMissingCheck, visible);
        setVisibleAndManaged(middleOnlyMissingCheck, visible);
        setVisibleAndManaged(rightOnlyMissingCheck, visible);
    }

    private static void setVisibleAndManaged(CheckBox control, boolean visible) {
        control.setVisible(visible);
        control.setManaged(visible);
    }

    private void updateCanSubmit() {
        canSubmit.set(buildExpression().isPresent());
    }

    private ComparisonResultFilterSelection snapshotSelection() {
        return new ComparisonResultFilterSelection(
            includeRadio.isSelected(),
            threeWay,
            identicalCheck.isSelected(),
            differentCheck.isSelected(),
            leftOnlyCheck.isSelected(),
            rightOnlyCheck.isSelected(),
            skippedCheck.isSelected(),
            middleOnlyCheck.isSelected(),
            leftOnlyDifferentCheck.isSelected(),
            middleOnlyDifferentCheck.isSelected(),
            rightOnlyDifferentCheck.isSelected(),
            leftOnlyMissingCheck.isSelected(),
            middleOnlyMissingCheck.isSelected(),
            rightOnlyMissingCheck.isSelected()
        );
    }

    private static void requireInjected(Object field, String fieldName) {
        Objects.requireNonNull(field, () -> "Missing @FXML injection for " + fieldName);
    }
}
