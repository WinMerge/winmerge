package org.winmerge.desktop.ui.dir;

import java.nio.file.Path;
import java.time.Instant;
import java.time.ZoneId;
import java.time.format.DateTimeFormatter;
import java.util.List;
import java.util.Objects;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.function.Consumer;

import javafx.beans.property.ReadOnlyObjectWrapper;
import javafx.beans.property.ReadOnlyStringWrapper;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.collections.transformation.FilteredList;
import javafx.collections.transformation.SortedList;
import javafx.concurrent.Task;
import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.control.ComboBox;
import javafx.scene.control.Label;
import javafx.scene.control.ProgressIndicator;
import javafx.scene.control.TableCell;
import javafx.scene.control.TableColumn;
import javafx.scene.control.TableRow;
import javafx.scene.control.TableView;
import javafx.scene.input.MouseButton;
import javafx.scene.layout.StackPane;
import org.winmerge.desktop.ui.TabManager;

public final class DirController {
    private static final DateTimeFormatter DATE_TIME_FORMATTER =
        DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss").withZone(ZoneId.systemDefault());

    private static final String FILTER_ALL = "All";
    private static final String FILTER_ONLY_LEFT = "Only Left";
    private static final String FILTER_ONLY_RIGHT = "Only Right";
    private static final String FILTER_DIFFERENT = "Different";
    private static final String FILTER_IDENTICAL = "Identical";

    @FXML
    private ComboBox<String> statusFilterCombo;

    @FXML
    private Button refreshButton;

    @FXML
    private Label leftPathLabel;

    @FXML
    private Label rightPathLabel;

    @FXML
    private TableView<DirDocModel.DirItem> dirTable;

    @FXML
    private TableColumn<DirDocModel.DirItem, String> nameColumn;

    @FXML
    private TableColumn<DirDocModel.DirItem, String> pathColumn;

    @FXML
    private TableColumn<DirDocModel.DirItem, String> statusColumn;

    @FXML
    private TableColumn<DirDocModel.DirItem, Instant> leftDateColumn;

    @FXML
    private TableColumn<DirDocModel.DirItem, Instant> rightDateColumn;

    @FXML
    private TableColumn<DirDocModel.DirItem, Long> leftSizeColumn;

    @FXML
    private TableColumn<DirDocModel.DirItem, Long> rightSizeColumn;

    @FXML
    private Label itemCountLabel;

    @FXML
    private Label comparisonSummaryLabel;

    @FXML
    private StackPane loadingOverlay;

    @FXML
    private ProgressIndicator loadingIndicator;

    private final ExecutorService ioExecutor = Executors.newSingleThreadExecutor(
        runnable -> {
            Thread thread = new Thread(runnable, "dir-compare-loader");
            thread.setDaemon(true);
            return thread;
        }
    );
    private final ObservableList<DirDocModel.DirItem> masterItems = FXCollections.observableArrayList();
    private final FilteredList<DirDocModel.DirItem> filteredItems =
        new FilteredList<>(masterItems, item -> true);
    private final List<String> allStatusClasses = List.of(
        DirDocModel.DirStatus.ONLY_LEFT.styleClass(),
        DirDocModel.DirStatus.ONLY_RIGHT.styleClass(),
        DirDocModel.DirStatus.DIFFERENT.styleClass(),
        DirDocModel.DirStatus.IDENTICAL.styleClass()
    );

    private Consumer<String> statusListener = message -> { };
    private TabManager tabManager;
    private DirDocModel model;
    private Path currentLeftPath;
    private Path currentRightPath;
    private String currentPathFilter = "*.*";

    @FXML
    private void initialize() {
        requireInjected(statusFilterCombo, "statusFilterCombo");
        requireInjected(refreshButton, "refreshButton");
        requireInjected(leftPathLabel, "leftPathLabel");
        requireInjected(rightPathLabel, "rightPathLabel");
        requireInjected(dirTable, "dirTable");
        requireInjected(nameColumn, "nameColumn");
        requireInjected(pathColumn, "pathColumn");
        requireInjected(statusColumn, "statusColumn");
        requireInjected(leftDateColumn, "leftDateColumn");
        requireInjected(rightDateColumn, "rightDateColumn");
        requireInjected(leftSizeColumn, "leftSizeColumn");
        requireInjected(rightSizeColumn, "rightSizeColumn");
        requireInjected(itemCountLabel, "itemCountLabel");
        requireInjected(comparisonSummaryLabel, "comparisonSummaryLabel");
        requireInjected(loadingOverlay, "loadingOverlay");
        requireInjected(loadingIndicator, "loadingIndicator");

        configureColumns();
        configureTable();
        configureFilter();
        setLoading(false);
        updateSummary();
    }

    public void configure(TabManager tabManager, Consumer<String> statusListener) {
        this.tabManager = Objects.requireNonNull(tabManager, "tabManager");
        this.statusListener = Objects.requireNonNull(statusListener, "statusListener");
    }

    public void loadDirectories(Path leftPath, Path rightPath, String pathFilter) {
        Objects.requireNonNull(leftPath, "leftPath");
        Objects.requireNonNull(rightPath, "rightPath");
        currentLeftPath = leftPath;
        currentRightPath = rightPath;
        currentPathFilter = (pathFilter == null || pathFilter.isBlank()) ? "*.*" : pathFilter.trim();

        leftPathLabel.setText(leftPath.toString());
        rightPathLabel.setText(rightPath.toString());
        setLoading(true);
        statusListener.accept("Scanning directories...");

        Task<DirDocModel> loadTask = new Task<>() {
            @Override
            protected DirDocModel call() throws Exception {
                return DirDocModel.load(leftPath, rightPath, currentPathFilter);
            }
        };

        loadTask.setOnSucceeded(event -> {
            model = loadTask.getValue();
            masterItems.setAll(model.items());
            applyStatusFilter(statusFilterCombo.getValue());
            setLoading(false);
            statusListener.accept("Opened directory diff with " + model.items().size() + " item(s).");
        });

        loadTask.setOnFailed(event -> {
            setLoading(false);
            Throwable failure = loadTask.getException();
            statusListener.accept("Failed to scan directories: " + (failure == null ? "unknown error" : failure.getMessage()));
        });

        ioExecutor.submit(loadTask);
    }

    @FXML
    private void onRefresh() {
        if (currentLeftPath == null || currentRightPath == null) {
            statusListener.accept("No directory comparison is currently loaded.");
            return;
        }
        loadDirectories(currentLeftPath, currentRightPath, currentPathFilter);
    }

    private void configureColumns() {
        nameColumn.setCellValueFactory(cell -> new ReadOnlyStringWrapper(cell.getValue().name()));
        pathColumn.setCellValueFactory(cell -> new ReadOnlyStringWrapper(cell.getValue().relativePath()));
        statusColumn.setCellValueFactory(cell -> new ReadOnlyStringWrapper(cell.getValue().status().label()));
        leftDateColumn.setCellValueFactory(cell -> new ReadOnlyObjectWrapper<>(cell.getValue().leftModifiedAt()));
        rightDateColumn.setCellValueFactory(cell -> new ReadOnlyObjectWrapper<>(cell.getValue().rightModifiedAt()));
        leftSizeColumn.setCellValueFactory(cell -> new ReadOnlyObjectWrapper<>(cell.getValue().leftSizeBytes()));
        rightSizeColumn.setCellValueFactory(cell -> new ReadOnlyObjectWrapper<>(cell.getValue().rightSizeBytes()));

        leftDateColumn.setCellFactory(column -> new NullableDateTableCell());
        rightDateColumn.setCellFactory(column -> new NullableDateTableCell());
        leftSizeColumn.setCellFactory(column -> new NullableSizeTableCell());
        rightSizeColumn.setCellFactory(column -> new NullableSizeTableCell());

        leftDateColumn.setComparator((left, right) -> compareNullable(left, right));
        rightDateColumn.setComparator((left, right) -> compareNullable(left, right));
        leftSizeColumn.setComparator((left, right) -> compareNullable(left, right));
        rightSizeColumn.setComparator((left, right) -> compareNullable(left, right));
    }

    private void configureTable() {
        SortedList<DirDocModel.DirItem> sortedItems = new SortedList<>(filteredItems);
        sortedItems.comparatorProperty().bind(dirTable.comparatorProperty());
        dirTable.setItems(sortedItems);
        dirTable.setRowFactory(
            tableView -> {
                TableRow<DirDocModel.DirItem> row = new TableRow<>();
                row.itemProperty().addListener((obs, oldItem, newItem) -> applyRowStyle(row, newItem));
                row.setOnMouseClicked(
                    event -> {
                        if (event.getButton() != MouseButton.PRIMARY || event.getClickCount() != 2 || row.isEmpty()) {
                            return;
                        }
                        openFileComparison(row.getItem());
                    }
                );
                return row;
            }
        );
    }

    private void configureFilter() {
        statusFilterCombo.getItems().setAll(
            FILTER_ALL,
            FILTER_ONLY_LEFT,
            FILTER_ONLY_RIGHT,
            FILTER_DIFFERENT,
            FILTER_IDENTICAL
        );
        statusFilterCombo.getSelectionModel().selectFirst();
        statusFilterCombo.valueProperty().addListener((obs, oldValue, newValue) -> applyStatusFilter(newValue));
    }

    private void applyStatusFilter(String selectedFilter) {
        DirDocModel.DirStatus selectedStatus = switch (selectedFilter) {
            case FILTER_ONLY_LEFT -> DirDocModel.DirStatus.ONLY_LEFT;
            case FILTER_ONLY_RIGHT -> DirDocModel.DirStatus.ONLY_RIGHT;
            case FILTER_DIFFERENT -> DirDocModel.DirStatus.DIFFERENT;
            case FILTER_IDENTICAL -> DirDocModel.DirStatus.IDENTICAL;
            default -> null;
        };

        filteredItems.setPredicate(item -> selectedStatus == null || item.status() == selectedStatus);
        updateSummary();
    }

    private void applyRowStyle(TableRow<DirDocModel.DirItem> row, DirDocModel.DirItem item) {
        row.getStyleClass().removeAll(allStatusClasses);
        if (item != null) {
            row.getStyleClass().add(item.status().styleClass());
        }
    }

    private void updateSummary() {
        int visibleItems = filteredItems.size();
        int totalItems = masterItems.size();
        itemCountLabel.setText("Rows: " + visibleItems + " / " + totalItems);

        if (model == null) {
            comparisonSummaryLabel.setText("Ready");
            return;
        }
        comparisonSummaryLabel.setText(
            "Only Left: " + model.countByStatus(DirDocModel.DirStatus.ONLY_LEFT)
                + " | Only Right: " + model.countByStatus(DirDocModel.DirStatus.ONLY_RIGHT)
                + " | Different: " + model.countByStatus(DirDocModel.DirStatus.DIFFERENT)
                + " | Identical: " + model.countByStatus(DirDocModel.DirStatus.IDENTICAL)
        );
    }

    private void openFileComparison(DirDocModel.DirItem item) {
        if (tabManager == null) {
            statusListener.accept("Directory pane is not initialized yet.");
            return;
        }
        if (item == null || !item.isComparableFile()) {
            statusListener.accept("Only matching files can be opened from the directory list.");
            return;
        }

        try {
            tabManager.openComparison(
                new TabManager.ComparisonRequest(
                    item.leftPath(),
                    item.rightPath(),
                    java.util.Optional.empty(),
                    TabManager.CompareTarget.FILES,
                    currentPathFilter
                ),
                statusListener
            );
        } catch (IllegalStateException illegalStateException) {
            statusListener.accept("Failed to open file diff: " + illegalStateException.getMessage());
        }
    }

    private void setLoading(boolean loading) {
        loadingOverlay.setVisible(loading);
        loadingOverlay.setManaged(loading);
        loadingIndicator.setVisible(loading);
        loadingIndicator.setManaged(loading);
        refreshButton.setDisable(loading);
    }

    private static <T extends Comparable<? super T>> int compareNullable(T left, T right) {
        if (left == right) {
            return 0;
        }
        if (left == null) {
            return 1;
        }
        if (right == null) {
            return -1;
        }
        return left.compareTo(right);
    }

    private static void requireInjected(Object field, String fieldName) {
        Objects.requireNonNull(field, () -> "Missing @FXML injection for " + fieldName);
    }

    private static final class NullableDateTableCell extends TableCell<DirDocModel.DirItem, Instant> {
        @Override
        protected void updateItem(Instant item, boolean empty) {
            super.updateItem(item, empty);
            setText(empty || item == null ? "" : DATE_TIME_FORMATTER.format(item));
        }
    }

    private static final class NullableSizeTableCell extends TableCell<DirDocModel.DirItem, Long> {
        @Override
        protected void updateItem(Long item, boolean empty) {
            super.updateItem(item, empty);
            setText(empty || item == null ? "" : String.format("%,d", item));
        }
    }
}
