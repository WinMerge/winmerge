package org.winmerge.desktop.ui.hex;

import java.util.Objects;

import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.paint.Color;
import javafx.scene.text.Font;
import javafx.scene.text.FontSmoothingType;

public final class HexGridCanvas extends Canvas {
    public static final double HEADER_HEIGHT = 24.0;
    public static final double ROW_HEIGHT = 20.0;

    private static final double OFFSET_X = 8.0;
    private static final double OFFSET_WIDTH = 86.0;
    private static final double HEX_START_X = OFFSET_X + OFFSET_WIDTH;
    private static final double HEX_CELL_WIDTH = 22.0;
    private static final double HEX_GROUP_GAP = 10.0;
    private static final double ASCII_START_X = HEX_START_X + (HexDocModel.BYTES_PER_ROW * HEX_CELL_WIDTH) + HEX_GROUP_GAP;
    private static final double ASCII_CELL_WIDTH = 11.0;

    private static final Color COLOR_BACKGROUND = Color.web("#ffffff");
    private static final Color COLOR_HEADER_BACKGROUND = Color.web("#f4f6f8");
    private static final Color COLOR_GRID_LINE = Color.web("#d0d7de");
    private static final Color COLOR_ROW_ALT = Color.web("#f8fafc");
    private static final Color COLOR_TEXT = Color.web("#1f2933");
    private static final Color COLOR_PLACEHOLDER = Color.web("#9aa5b1");
    private static final Color COLOR_DIFF = Color.web("#ffe8cc");

    private static final Font HEADER_FONT = Font.font("Consolas", 11);
    private static final Font BODY_FONT = Font.font("Consolas", 12);

    private HexDocModel model;
    private HexDocModel.Side side = HexDocModel.Side.LEFT;
    private double firstVisibleRow;
    private String placeholderText = "Open two files to show hex diff.";

    public HexGridCanvas() {
        widthProperty().addListener((obs, oldValue, newValue) -> redraw());
        heightProperty().addListener((obs, oldValue, newValue) -> redraw());
        setFocusTraversable(false);
    }

    public void setModel(HexDocModel model) {
        this.model = model;
        redraw();
    }

    public void setPlaceholderText(String placeholderText) {
        this.placeholderText = Objects.requireNonNull(placeholderText, "placeholderText");
        if (model == null) {
            redraw();
        }
    }

    public void setSide(HexDocModel.Side side) {
        this.side = Objects.requireNonNull(side, "side");
        redraw();
    }

    public void setFirstVisibleRow(double firstVisibleRow) {
        this.firstVisibleRow = Math.max(0.0, firstVisibleRow);
        redraw();
    }

    private void redraw() {
        GraphicsContext gc = getGraphicsContext2D();
        gc.setFontSmoothingType(FontSmoothingType.GRAY);

        double width = getWidth();
        double height = getHeight();
        gc.setFill(COLOR_BACKGROUND);
        gc.fillRect(0, 0, width, height);

        if (width <= 0 || height <= 0) {
            return;
        }

        drawHeader(gc, width);
        if (model == null) {
            gc.setFont(BODY_FONT);
            gc.setFill(COLOR_PLACEHOLDER);
            gc.fillText(placeholderText, OFFSET_X, HEADER_HEIGHT + ROW_HEIGHT);
            return;
        }

        int startRow = (int) Math.floor(firstVisibleRow);
        double rowFraction = firstVisibleRow - startRow;
        double y = HEADER_HEIGHT - (rowFraction * ROW_HEIGHT);

        int row = startRow;
        while (y < height && row < model.rowCount()) {
            boolean alt = (row % 2) == 1;
            if (alt) {
                gc.setFill(COLOR_ROW_ALT);
                gc.fillRect(0, y, width, ROW_HEIGHT);
            }
            drawRow(gc, row, y);
            row++;
            y += ROW_HEIGHT;
        }
    }

    private void drawHeader(GraphicsContext gc, double width) {
        gc.setFill(COLOR_HEADER_BACKGROUND);
        gc.fillRect(0, 0, width, HEADER_HEIGHT);
        gc.setStroke(COLOR_GRID_LINE);
        gc.strokeLine(0, HEADER_HEIGHT - 0.5, width, HEADER_HEIGHT - 0.5);

        gc.setFont(HEADER_FONT);
        gc.setFill(COLOR_TEXT);
        gc.fillText("Offset", OFFSET_X, 16);
        gc.fillText("Hex", HEX_START_X, 16);
        gc.fillText("ASCII", ASCII_START_X, 16);
    }

    private void drawRow(GraphicsContext gc, int rowIndex, double y) {
        double baselineY = y + 14;
        int rowStartByte = rowIndex * HexDocModel.BYTES_PER_ROW;

        gc.setFont(BODY_FONT);
        gc.setFill(COLOR_TEXT);
        gc.fillText(model.formatOffsetForRow(rowIndex), OFFSET_X, baselineY);

        for (int byteInRow = 0; byteInRow < HexDocModel.BYTES_PER_ROW; byteInRow++) {
            int byteIndex = rowStartByte + byteInRow;
            boolean hasByte = model.hasByte(side, byteIndex);
            boolean diff = model.isDifferentByte(byteIndex);
            double hexX = hexCellX(byteInRow);
            double asciiX = asciiCellX(byteInRow);

            if (diff) {
                gc.setFill(COLOR_DIFF);
                gc.fillRect(hexX - 2, y + 2, HEX_CELL_WIDTH - 2, ROW_HEIGHT - 4);
                gc.fillRect(asciiX - 1, y + 2, ASCII_CELL_WIDTH, ROW_HEIGHT - 4);
            }

            if (hasByte) {
                int value = model.unsignedByte(side, byteIndex);
                gc.setFill(COLOR_TEXT);
                gc.fillText(String.format("%02X", value), hexX, baselineY);
                gc.fillText(String.valueOf(toAscii(value)), asciiX, baselineY);
            } else if (diff) {
                gc.setFill(COLOR_PLACEHOLDER);
                gc.fillText("--", hexX, baselineY);
                gc.fillText(".", asciiX, baselineY);
            }
        }
    }

    private static char toAscii(int value) {
        if (value >= 32 && value <= 126) {
            return (char) value;
        }
        return '.';
    }

    private static double hexCellX(int byteInRow) {
        double groupGap = byteInRow >= 8 ? HEX_GROUP_GAP : 0;
        return HEX_START_X + (byteInRow * HEX_CELL_WIDTH) + groupGap;
    }

    private static double asciiCellX(int byteInRow) {
        return ASCII_START_X + (byteInRow * ASCII_CELL_WIDTH);
    }
}
