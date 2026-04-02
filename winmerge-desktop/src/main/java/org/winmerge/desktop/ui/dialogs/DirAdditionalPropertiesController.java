package org.winmerge.desktop.ui.dialogs;

import java.util.ArrayList;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Objects;
import java.util.Set;

import javafx.collections.FXCollections;
import javafx.fxml.FXML;
import javafx.scene.control.ListCell;
import javafx.scene.control.ListView;
import javafx.scene.control.SelectionMode;
import javafx.scene.control.TreeCell;
import javafx.scene.control.TreeItem;
import javafx.scene.control.TreeView;
import javafx.scene.input.KeyCode;

public final class DirAdditionalPropertiesController {
    @FXML
    private TreeView<DirPropertyNode> propertiesTree;

    @FXML
    private ListView<DirPropertyNode> selectedPropertiesList;

    @FXML
    private void initialize() {
        requireInjected(propertiesTree, "propertiesTree");
        requireInjected(selectedPropertiesList, "selectedPropertiesList");

        propertiesTree.setShowRoot(false);
        selectedPropertiesList.getSelectionModel().setSelectionMode(SelectionMode.MULTIPLE);
        propertiesTree.setCellFactory(
            view -> new TreeCell<>() {
                @Override
                protected void updateItem(DirPropertyNode item, boolean empty) {
                    super.updateItem(item, empty);
                    setText(empty || item == null ? null : item.displayName());
                }
            }
        );
        selectedPropertiesList.setCellFactory(
            view -> new ListCell<>() {
                @Override
                protected void updateItem(DirPropertyNode item, boolean empty) {
                    super.updateItem(item, empty);
                    setText(empty || item == null ? null : item.displayName());
                }
            }
        );

        propertiesTree.setOnMouseClicked(event -> {
            if (event.getClickCount() == 2) {
                toggleCurrentTreeNode();
            }
        });
        propertiesTree.setOnKeyPressed(event -> {
            if (event.getCode() == KeyCode.SPACE) {
                toggleCurrentTreeNode();
                event.consume();
            }
        });

        selectedPropertiesList.setOnKeyPressed(event -> {
            if (event.getCode() == KeyCode.DELETE) {
                onDelete();
                event.consume();
            }
        });
    }

    public void bind(List<DirPropertyNode> nodes) {
        Objects.requireNonNull(nodes, "nodes");

        TreeItem<DirPropertyNode> root = new TreeItem<>();
        for (DirPropertyNode node : nodes) {
            root.getChildren().add(toTreeItem(node));
        }
        propertiesTree.setRoot(root);

        List<DirPropertyNode> selected = new ArrayList<>();
        collectSelectedLeaves(nodes, selected);
        selectedPropertiesList.setItems(FXCollections.observableArrayList(selected));

        if (!root.getChildren().isEmpty()) {
            propertiesTree.getSelectionModel().select(root.getChildren().get(0));
        }
    }

    public List<String> buildResult() {
        Set<String> canonicalNames = new LinkedHashSet<>();
        for (DirPropertyNode node : selectedPropertiesList.getItems()) {
            if (!node.canonicalName().isBlank()) {
                canonicalNames.add(node.canonicalName());
            }
        }
        return List.copyOf(canonicalNames);
    }

    @FXML
    private void onAdd() {
        TreeItem<DirPropertyNode> selected = propertiesTree.getSelectionModel().getSelectedItem();
        if (selected == null) {
            return;
        }
        DirPropertyNode node = selected.getValue();
        if (node == null || !node.isLeaf()) {
            return;
        }
        addNodeSelection(node);
    }

    @FXML
    private void onDelete() {
        List<DirPropertyNode> selectedItems = new ArrayList<>(selectedPropertiesList.getSelectionModel().getSelectedItems());
        if (selectedItems.isEmpty()) {
            return;
        }
        selectedPropertiesList.getItems().removeAll(selectedItems);
        for (DirPropertyNode node : selectedItems) {
            node.setSelected(false);
        }
    }

    @FXML
    private void onDeleteAll() {
        for (DirPropertyNode node : selectedPropertiesList.getItems()) {
            node.setSelected(false);
        }
        selectedPropertiesList.getItems().clear();
    }

    private void toggleCurrentTreeNode() {
        TreeItem<DirPropertyNode> selected = propertiesTree.getSelectionModel().getSelectedItem();
        if (selected == null) {
            return;
        }
        DirPropertyNode node = selected.getValue();
        if (node == null || !node.isLeaf()) {
            return;
        }

        if (node.selected()) {
            selectedPropertiesList.getItems().remove(node);
            node.setSelected(false);
        } else {
            addNodeSelection(node);
        }
    }

    private void addNodeSelection(DirPropertyNode node) {
        if (node.selected()) {
            return;
        }
        node.setSelected(true);
        selectedPropertiesList.getItems().add(node);
    }

    private TreeItem<DirPropertyNode> toTreeItem(DirPropertyNode node) {
        TreeItem<DirPropertyNode> item = new TreeItem<>(node);
        for (DirPropertyNode child : node.children()) {
            item.getChildren().add(toTreeItem(child));
        }
        return item;
    }

    private void collectSelectedLeaves(List<DirPropertyNode> nodes, List<DirPropertyNode> selected) {
        for (DirPropertyNode node : nodes) {
            if (node.isLeaf() && node.selected()) {
                selected.add(node);
            }
            collectSelectedLeaves(node.children(), selected);
        }
    }

    private static void requireInjected(Object field, String fieldName) {
        Objects.requireNonNull(field, () -> "Missing @FXML injection for " + fieldName);
    }
}
