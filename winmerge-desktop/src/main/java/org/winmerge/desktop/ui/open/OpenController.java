package org.winmerge.desktop.ui.open;

import java.io.File;
import java.nio.file.InvalidPathException;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;
import java.util.Optional;
import java.util.function.Consumer;

import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.control.CheckBox;
import javafx.scene.control.ComboBox;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;
import javafx.stage.DirectoryChooser;
import javafx.stage.FileChooser;
import javafx.stage.Window;
import org.winmerge.desktop.ui.TabManager;

public final class OpenController {
    @FXML
    private TextField leftPathField;

    @FXML
    private TextField rightPathField;

    @FXML
    private TextField middlePathField;

    @FXML
    private Label middlePathLabel;

    @FXML
    private Button middleBrowseButton;

    @FXML
    private CheckBox threeWayCheckBox;

    @FXML
    private ComboBox<String> compareModeCombo;

    @FXML
    private ComboBox<String> filterCombo;

    @FXML
    private Button compareButton;

    private TabManager tabManager;
    private Runnable closeRequest = () -> { };
    private Consumer<String> statusListener = message -> { };

    @FXML
    private void initialize() {
        requireInjected(leftPathField, "leftPathField");
        requireInjected(rightPathField, "rightPathField");
        requireInjected(middlePathField, "middlePathField");
        requireInjected(middlePathLabel, "middlePathLabel");
        requireInjected(middleBrowseButton, "middleBrowseButton");
        requireInjected(threeWayCheckBox, "threeWayCheckBox");
        requireInjected(compareModeCombo, "compareModeCombo");
        requireInjected(filterCombo, "filterCombo");
        requireInjected(compareButton, "compareButton");

        compareModeCombo.getItems().setAll("Files", "Folders");
        compareModeCombo.getSelectionModel().selectFirst();

        filterCombo.setEditable(true);
        filterCombo.getItems().setAll("*.*", "*.txt", "*.java", "*.xml", "*.json");
        filterCombo.getSelectionModel().selectFirst();

        threeWayCheckBox.setSelected(false);
        threeWayCheckBox.setDisable(true);
        threeWayCheckBox.setText("Enable 3-way mode (coming soon)");
        setThreeWayVisible(false);
    }

    public void configure(TabManager tabManager, Runnable closeRequest, Consumer<String> statusListener) {
        this.tabManager = Objects.requireNonNull(tabManager, "tabManager");
        this.closeRequest = Objects.requireNonNull(closeRequest, "closeRequest");
        this.statusListener = Objects.requireNonNull(statusListener, "statusListener");
    }

    @FXML
    private void onBrowseLeft() {
        selectPath(leftPathField, "Select left path");
    }

    @FXML
    private void onBrowseRight() {
        selectPath(rightPathField, "Select right path");
    }

    @FXML
    private void onBrowseMiddle() {
        selectPath(middlePathField, "Select middle path");
    }

    @FXML
    private void onCompare() {
        if (tabManager == null) {
            statusListener.accept("Open pane is not initialized yet.");
            return;
        }

        String left = leftPathField.getText().trim();
        String right = rightPathField.getText().trim();
        if (left.isEmpty() || right.isEmpty()) {
            statusListener.accept("Select both left and right paths before comparing.");
            return;
        }

        if (threeWayCheckBox.isSelected()) {
            statusListener.accept("3-way comparisons are not supported yet.");
            return;
        }
        Optional<Path> middlePath = Optional.empty();

        String filterValue = Optional.ofNullable(filterCombo.getEditor().getText())
            .orElse("*.*")
            .trim();
        if (filterValue.isEmpty()) {
            filterValue = "*.*";
        }

        TabManager.CompareTarget compareTarget = "Folders".equals(compareModeCombo.getValue())
            ? TabManager.CompareTarget.FOLDERS
            : TabManager.CompareTarget.FILES;

        try {
            TabManager.ComparisonRequest request = new TabManager.ComparisonRequest(
                Paths.get(left),
                Paths.get(right),
                middlePath,
                compareTarget,
                filterValue
            );
            tabManager.openComparison(request, statusListener);
            closeRequest.run();
        } catch (InvalidPathException invalidPathException) {
            statusListener.accept("Invalid path: " + invalidPathException.getInput());
        } catch (UnsupportedOperationException unsupportedOperationException) {
            statusListener.accept(unsupportedOperationException.getMessage());
        } catch (IllegalStateException illegalStateException) {
            statusListener.accept("Failed to open comparison: " + illegalStateException.getMessage());
        }
    }

    @FXML
    private void onCancel() {
        closeRequest.run();
        statusListener.accept("Open cancelled.");
    }

    private void setThreeWayVisible(boolean visible) {
        middlePathLabel.setVisible(visible);
        middlePathLabel.setManaged(visible);
        middlePathField.setVisible(visible);
        middlePathField.setManaged(visible);
        middleBrowseButton.setVisible(visible);
        middleBrowseButton.setManaged(visible);
    }

    private void selectPath(TextField targetField, String title) {
        Window owner = targetField.getScene() == null ? null : targetField.getScene().getWindow();
        File selected = "Folders".equals(compareModeCombo.getValue())
            ? chooseDirectory(owner, title)
            : chooseFile(owner, title);
        if (selected != null) {
            targetField.setText(selected.getAbsolutePath());
        }
    }

    private File chooseDirectory(Window owner, String title) {
        DirectoryChooser chooser = new DirectoryChooser();
        chooser.setTitle(title);
        configureInitialDirectory(chooser, owner);
        return chooser.showDialog(owner);
    }

    private File chooseFile(Window owner, String title) {
        FileChooser chooser = new FileChooser();
        chooser.setTitle(title);

        List<String> filters = parseFilterPatterns(filterCombo.getEditor().getText());
        if (!filters.isEmpty()) {
            chooser.getExtensionFilters().add(new FileChooser.ExtensionFilter("Filtered files", filters));
        }
        chooser.getExtensionFilters().add(new FileChooser.ExtensionFilter("All files", "*.*"));

        configureInitialDirectory(chooser, owner);
        return chooser.showOpenDialog(owner);
    }

    private void configureInitialDirectory(FileChooser chooser, Window owner) {
        File initialDirectory = findInitialDirectory();
        if (initialDirectory != null) {
            chooser.setInitialDirectory(initialDirectory);
        }
    }

    private void configureInitialDirectory(DirectoryChooser chooser, Window owner) {
        File initialDirectory = findInitialDirectory();
        if (initialDirectory != null) {
            chooser.setInitialDirectory(initialDirectory);
        }
    }

    private File findInitialDirectory() {
        for (TextField field : List.of(leftPathField, rightPathField, middlePathField)) {
            String raw = field.getText();
            if (raw == null || raw.isBlank()) {
                continue;
            }
            try {
                Path path = Paths.get(raw.trim());
                Path candidate = java.nio.file.Files.isDirectory(path) ? path : path.getParent();
                if (candidate != null && java.nio.file.Files.isDirectory(candidate)) {
                    return candidate.toFile();
                }
            } catch (InvalidPathException ignored) {
                // Ignore malformed values while trying other fields.
            }
        }
        return null;
    }

    private static List<String> parseFilterPatterns(String filterText) {
        String normalized = filterText == null ? "" : filterText.trim();
        if (normalized.isEmpty() || "*.*".equals(normalized)) {
            return List.of();
        }

        String[] rawParts = normalized.split("[;,|]");
        List<String> patterns = new ArrayList<>(rawParts.length);
        for (String part : rawParts) {
            String pattern = part.trim();
            if (pattern.isEmpty()) {
                continue;
            }
            if (!pattern.contains("*")) {
                pattern = "*" + (pattern.startsWith(".") ? pattern : "." + pattern);
            }
            patterns.add(pattern);
        }
        return patterns;
    }

    private static void requireInjected(Object field, String fieldName) {
        Objects.requireNonNull(field, () -> "Missing @FXML injection for " + fieldName);
    }
}
