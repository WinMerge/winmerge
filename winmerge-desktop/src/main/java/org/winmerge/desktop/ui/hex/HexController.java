package org.winmerge.desktop.ui.hex;

import java.io.IOException;
import java.nio.file.Path;
import java.util.Objects;
import java.util.function.Consumer;

import javafx.fxml.FXML;
import javafx.scene.control.Label;
import javafx.scene.control.ScrollBar;
import javafx.scene.input.ScrollEvent;
import javafx.scene.layout.StackPane;

public final class HexController {
    @FXML
    private Label leftPathLabel;

    @FXML
    private Label rightPathLabel;

    @FXML
    private StackPane leftCanvasContainer;

    @FXML
    private StackPane rightCanvasContainer;

    @FXML
    private HexGridCanvas leftCanvas;

    @FXML
    private HexGridCanvas rightCanvas;

    @FXML
    private ScrollBar leftScrollBar;

    @FXML
    private ScrollBar rightScrollBar;

    private Consumer<String> statusListener = message -> { };
    private HexDocModel model;
    private boolean syncingScroll;

    @FXML
    private void initialize() {
        requireInjected(leftPathLabel, "leftPathLabel");
        requireInjected(rightPathLabel, "rightPathLabel");
        requireInjected(leftCanvasContainer, "leftCanvasContainer");
        requireInjected(rightCanvasContainer, "rightCanvasContainer");
        requireInjected(leftCanvas, "leftCanvas");
        requireInjected(rightCanvas, "rightCanvas");
        requireInjected(leftScrollBar, "leftScrollBar");
        requireInjected(rightScrollBar, "rightScrollBar");

        leftCanvas.setSide(HexDocModel.Side.LEFT);
        rightCanvas.setSide(HexDocModel.Side.RIGHT);

        leftCanvas.widthProperty().bind(leftCanvasContainer.widthProperty());
        leftCanvas.heightProperty().bind(leftCanvasContainer.heightProperty());
        rightCanvas.widthProperty().bind(rightCanvasContainer.widthProperty());
        rightCanvas.heightProperty().bind(rightCanvasContainer.heightProperty());

        leftScrollBar.valueProperty().addListener((obs, oldValue, newValue) -> applySharedScroll(newValue.doubleValue(), true));
        rightScrollBar.valueProperty().addListener((obs, oldValue, newValue) -> applySharedScroll(newValue.doubleValue(), false));

        leftCanvas.heightProperty().addListener((obs, oldValue, newValue) -> updateScrollMetrics());
        rightCanvas.heightProperty().addListener((obs, oldValue, newValue) -> updateScrollMetrics());

        leftCanvas.addEventFilter(ScrollEvent.SCROLL, this::onCanvasScroll);
        rightCanvas.addEventFilter(ScrollEvent.SCROLL, this::onCanvasScroll);

        updateScrollMetrics();
    }

    public void setStatusListener(Consumer<String> statusListener) {
        this.statusListener = Objects.requireNonNull(statusListener, "statusListener");
    }

    public void loadFiles(Path leftPath, Path rightPath) throws IOException {
        Objects.requireNonNull(leftPath, "leftPath");
        Objects.requireNonNull(rightPath, "rightPath");

        leftPathLabel.setText(leftPath.toString());
        rightPathLabel.setText(rightPath.toString());

        model = HexDocModel.load(leftPath, rightPath);
        leftCanvas.setModel(model);
        rightCanvas.setModel(model);

        updateScrollMetrics();
        setSharedScrollValue(0.0);

        String diffStatus = model.hasDiffs() ? "Differences highlighted." : "Files are identical.";
        statusListener.accept("Opened binary hex view. " + diffStatus);
    }

    private void onCanvasScroll(ScrollEvent event) {
        if (event.getDeltaY() == 0) {
            return;
        }
        double lines = -event.getDeltaY() / 36.0;
        if (Math.abs(lines) < 0.01) {
            lines = event.getDeltaY() < 0 ? 1 : -1;
        }
        setSharedScrollValue(leftScrollBar.getValue() + lines);
        event.consume();
    }

    private void updateScrollMetrics() {
        int totalRows = model == null ? 1 : model.rowCount();
        double viewportHeight = Math.max(0.0, Math.min(leftCanvas.getHeight(), rightCanvas.getHeight()));
        double bodyHeight = Math.max(0.0, viewportHeight - HexGridCanvas.HEADER_HEIGHT);
        double visibleRows = Math.max(1.0, Math.floor(bodyHeight / HexGridCanvas.ROW_HEIGHT));
        double maxScroll = Math.max(0.0, totalRows - visibleRows);

        configureScrollBar(leftScrollBar, visibleRows, maxScroll);
        configureScrollBar(rightScrollBar, visibleRows, maxScroll);
        setSharedScrollValue(Math.min(leftScrollBar.getValue(), maxScroll));
    }

    private void configureScrollBar(ScrollBar scrollBar, double visibleRows, double maxScroll) {
        scrollBar.setMin(0.0);
        scrollBar.setMax(maxScroll);
        scrollBar.setVisibleAmount(visibleRows);
        scrollBar.setUnitIncrement(1.0);
        scrollBar.setBlockIncrement(Math.max(1.0, visibleRows - 1.0));
        scrollBar.setDisable(maxScroll <= 0.0);
    }

    private void applySharedScroll(double value, boolean sourceIsLeft) {
        if (syncingScroll) {
            return;
        }
        syncingScroll = true;
        try {
            double clamped = clamp(value, 0.0, leftScrollBar.getMax());
            if (sourceIsLeft) {
                rightScrollBar.setValue(clamped);
            } else {
                leftScrollBar.setValue(clamped);
            }
            leftCanvas.setFirstVisibleRow(clamped);
            rightCanvas.setFirstVisibleRow(clamped);
        } finally {
            syncingScroll = false;
        }
    }

    private void setSharedScrollValue(double value) {
        double clamped = clamp(value, 0.0, leftScrollBar.getMax());
        leftScrollBar.setValue(clamped);
        rightScrollBar.setValue(clamped);
        leftCanvas.setFirstVisibleRow(clamped);
        rightCanvas.setFirstVisibleRow(clamped);
    }

    private static double clamp(double value, double min, double max) {
        if (value < min) {
            return min;
        }
        if (value > max) {
            return max;
        }
        return value;
    }

    private static void requireInjected(Object field, String fieldName) {
        Objects.requireNonNull(field, () -> "Missing @FXML injection for " + fieldName);
    }
}
