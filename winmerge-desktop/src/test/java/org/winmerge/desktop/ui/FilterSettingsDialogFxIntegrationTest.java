package org.winmerge.desktop.ui;

import java.util.concurrent.atomic.AtomicBoolean;

import javafx.scene.Node;
import javafx.scene.control.ButtonBase;
import javafx.scene.control.ButtonType;
import javafx.scene.control.CheckBox;
import javafx.scene.control.ListView;
import javafx.scene.control.TabPane;
import javafx.scene.control.TableView;
import javafx.scene.control.TextField;
import javafx.scene.layout.StackPane;
import javafx.scene.layout.VBox;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Test;
import org.winmerge.desktop.ui.dialogs.FileFiltersDialogModel;
import org.winmerge.desktop.ui.dialogs.FilterSettingsDialog;
import org.winmerge.desktop.ui.dialogs.FilterSettingsModel;
import org.winmerge.desktop.ui.dialogs.LineFiltersDialogModel;
import org.winmerge.desktop.ui.dialogs.SubstitutionFiltersDialogModel;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertTrue;

class FilterSettingsDialogFxIntegrationTest {
    @BeforeAll
    static void setupFxToolkit() {
        FxThreadTestSupport.initializeToolkit();
    }

    @Test
    void filterSettingsDialogWiresTabsFlushesModelsAndCancelsWatcherOnClose() {
        FileFiltersDialogModel fileModel = new FileFiltersDialogModel();
        LineFiltersDialogModel lineModel = new LineFiltersDialogModel();
        SubstitutionFiltersDialogModel substitutionModel = new SubstitutionFiltersDialogModel();
        FilterSettingsModel model = new FilterSettingsModel(fileModel, lineModel, substitutionModel);
        AtomicBoolean watcherCancelled = new AtomicBoolean(false);
        fileModel.setDirWatcher(() -> watcherCancelled.set(true));

        FxThreadTestSupport.runOnFxThread(() -> {
            FilterSettingsDialog dialog = new FilterSettingsDialog(null, model);

            TabPane tabPane = (TabPane) dialog.getDialogPane().getContent();
            assertNotNull(tabPane);
            assertEquals(3, tabPane.getTabs().size());

            StackPane fileContainer = tabPane.getTabs().stream()
                .map(tab -> (StackPane) tab.getContent())
                .filter(container -> tryFindFirstNode(container, TextField.class) != null)
                .findFirst()
                .orElseThrow(() -> new IllegalStateException("Missing file filters tab content"));

            StackPane lineContainer = tabPane.getTabs().stream()
                .map(tab -> (StackPane) tab.getContent())
                .filter(container -> tryFindFirstNode(container, ListView.class) != null)
                .findFirst()
                .orElseThrow(() -> new IllegalStateException("Missing line filters tab content"));

            StackPane substitutionContainer = tabPane.getTabs().stream()
                .map(tab -> (StackPane) tab.getContent())
                .filter(container -> tryFindFirstNode(container, TableView.class) != null)
                .filter(container -> tryFindFirstNode(container, CheckBox.class) != null)
                .filter(container -> tryFindFirstNode(container, TextField.class) == null)
                .findFirst()
                .orElseThrow(() -> new IllegalStateException("Missing substitution filters tab content"));
            assertEquals(1, fileContainer.getChildren().size());
            assertEquals(1, lineContainer.getChildren().size());
            assertEquals(1, substitutionContainer.getChildren().size());
            assertTrue(fileContainer.getChildren().get(0) instanceof VBox);
            assertTrue(lineContainer.getChildren().get(0) instanceof VBox);
            assertTrue(substitutionContainer.getChildren().get(0) instanceof VBox);

            TextField maskField = findFirstNode(fileContainer, TextField.class);
            CheckBox lineIgnoreRegexCheck = findFirstNode(lineContainer, CheckBox.class);
            CheckBox substitutionEnabledCheck = findFirstNode(substitutionContainer, CheckBox.class);

            maskField.setText("*.log");
            lineIgnoreRegexCheck.setSelected(true);
            substitutionEnabledCheck.setSelected(true);

            ButtonBase okButton = (ButtonBase) dialog.getDialogPane().lookupButton(ButtonType.OK);
            okButton.fire();

            dialog.hide();

            assertEquals("*.log", fileModel.getMaskExpression());
            assertTrue(lineModel.ignoreRegExpErrors());
            assertTrue(substitutionModel.enabled());
        });

        assertTrue(watcherCancelled.get());
    }

    private static <T extends Node> T findFirstNode(Node root, Class<T> type) {
        T match = tryFindFirstNode(root, type);
        if (match != null) {
            return match;
        }
        throw new IllegalStateException("Missing node of type: " + type.getSimpleName());
    }

    private static <T extends Node> T tryFindFirstNode(Node root, Class<T> type) {
        if (type.isInstance(root)) {
            return type.cast(root);
        }
        if (root instanceof javafx.scene.Parent parent) {
            for (Node child : parent.getChildrenUnmodifiable()) {
                T match = tryFindFirstNode(child, type);
                if (match != null) {
                    return match;
                }
            }
        }
        return null;
    }
}
