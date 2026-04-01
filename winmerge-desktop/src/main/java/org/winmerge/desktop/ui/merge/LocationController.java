package org.winmerge.desktop.ui.merge;

import java.util.List;
import java.util.Objects;
import java.util.function.IntConsumer;

import javafx.fxml.FXML;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.AnchorPane;
import javafx.scene.paint.Color;

public class LocationController {
    @FXML
    private AnchorPane rootPane;

    @FXML
    private Canvas mapCanvas;

    private List<DiffChunk> diffChunks = List.of();
    private int maxLines = 1;
    private int activeDiffIndex = -1;
    private IntConsumer diffSelectionListener;

    @FXML
    private void initialize() {
        Objects.requireNonNull(rootPane, "rootPane");
        Objects.requireNonNull(mapCanvas, "mapCanvas");
        mapCanvas.widthProperty().bind(rootPane.widthProperty());
        mapCanvas.heightProperty().bind(rootPane.heightProperty());
        mapCanvas.widthProperty().addListener((obs, oldValue, newValue) -> redraw());
        mapCanvas.heightProperty().addListener((obs, oldValue, newValue) -> redraw());
        mapCanvas.addEventHandler(MouseEvent.MOUSE_CLICKED, this::onMapClicked);
        redraw();
    }

    public void setDiffs(List<DiffChunk> diffChunks, int leftLineCount, int rightLineCount) {
        this.diffChunks = List.copyOf(Objects.requireNonNull(diffChunks, "diffChunks"));
        this.maxLines = Math.max(1, Math.max(leftLineCount, rightLineCount));
        redraw();
    }

    public void setActiveDiffIndex(int activeDiffIndex) {
        this.activeDiffIndex = activeDiffIndex;
        redraw();
    }

    public void setDiffSelectionListener(IntConsumer diffSelectionListener) {
        this.diffSelectionListener = diffSelectionListener;
    }

    private void onMapClicked(MouseEvent event) {
        if (diffChunks.isEmpty() || mapCanvas.getHeight() <= 0) {
            return;
        }
        double normalizedY = event.getY() / mapCanvas.getHeight();
        int mappedLine = (int) Math.floor(normalizedY * maxLines);
        int selectedIndex = closestDiffIndex(mappedLine);
        if (selectedIndex >= 0 && diffSelectionListener != null) {
            diffSelectionListener.accept(selectedIndex);
        }
    }

    private int closestDiffIndex(int line) {
        int selectedIndex = -1;
        int selectedDistance = Integer.MAX_VALUE;
        for (int i = 0; i < diffChunks.size(); i++) {
            DiffChunk chunk = diffChunks.get(i);
            int chunkStart = Math.min(chunk.leftStartLine(), chunk.rightStartLine());
            int chunkEnd = Math.max(chunk.leftEndLine(), chunk.rightEndLine());
            int distance = line < chunkStart
                ? chunkStart - line
                : Math.max(0, line - chunkEnd);
            if (distance < selectedDistance) {
                selectedDistance = distance;
                selectedIndex = i;
            }
        }
        return selectedIndex;
    }

    private void redraw() {
        GraphicsContext gc = mapCanvas.getGraphicsContext2D();
        double width = mapCanvas.getWidth();
        double height = mapCanvas.getHeight();
        gc.setFill(Color.web("#eef2f6"));
        gc.fillRect(0, 0, width, height);

        if (diffChunks.isEmpty()) {
            gc.setFill(Color.web("#a3adb8"));
            gc.fillRect(width / 2.0 - 1, 6, 2, Math.max(0, height - 12));
            return;
        }

        for (int i = 0; i < diffChunks.size(); i++) {
            DiffChunk chunk = diffChunks.get(i);
            int startLine = Math.min(chunk.leftStartLine(), chunk.rightStartLine());
            int endLine = Math.max(chunk.leftEndLine(), chunk.rightEndLine());
            double y = (startLine / (double) maxLines) * height;
            double h = Math.max(2.0, ((endLine - startLine) / (double) maxLines) * height);
            gc.setFill(i == activeDiffIndex ? Color.web("#d9480f") : Color.web("#f97316"));
            gc.fillRect(4, y, Math.max(0, width - 8), h);
        }
    }
}
