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
    private static final double HEX_CELL_WIDTH = 22.0;
    private static final double HEX_GROUP_GAP = 10.0;
    private static final double ASCII_CELL_WIDTH = 11.0;
    private static final double MIN_OFFSET_WIDTH = 72.0;
    private static final double MIN_HEX_CELL_WIDTH = 18.0;
    private static final double MIN_HEX_GROUP_GAP = 6.0;
    private static final double MIN_ASCII_CELL_WIDTH = 8.0;
    private static final double RIGHT_PADDING = 8.0;

    private static final Color COLOR_BACKGROUND = Color.web("#ffffff");
    private static final Color COLOR_HEADER_BACKGROUND = Color.web("#f4f6f8");
    private static final Color COLOR_GRID_LINE = Color.web("#d0d7de");
    private static final Color COLOR_ROW_ALT = Color.web("#f8fafc");
    private static final Color COLOR_TEXT = Color.web("#1f2933");
    private static final Color COLOR_PLACEHOLDER = Color.web("#9aa5b1");
    private static final Color COLOR_DIFF = Color.web("#ffe8cc");

    private static final Font HEADER_FONT = Font.font("Consolas", 11);
    private static final Font BODY_FONT = Font.font("Consolas", 12);
    private static final Font COMPACT_HEADER_FONT = Font.font("Consolas", 10);
    private static final Font COMPACT_BODY_FONT = Font.font("Consolas", 11);

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

        LayoutMetrics layout = LayoutMetrics.forWidth(width);
        drawHeader(gc, width, layout);
        if (model == null) {
            gc.setFont(layout.bodyFont());
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
            drawRow(gc, row, y, layout);
            row++;
            y += ROW_HEIGHT;
        }
    }

    private void drawHeader(GraphicsContext gc, double width, LayoutMetrics layout) {
        gc.setFill(COLOR_HEADER_BACKGROUND);
        gc.fillRect(0, 0, width, HEADER_HEIGHT);
        gc.setStroke(COLOR_GRID_LINE);
        gc.strokeLine(0, HEADER_HEIGHT - 0.5, width, HEADER_HEIGHT - 0.5);

        gc.setFont(layout.headerFont());
        gc.setFill(COLOR_TEXT);
        gc.fillText("Offset", OFFSET_X, 16);
        gc.fillText("Hex", layout.hexStartX(), 16);
        gc.fillText("ASCII", layout.asciiStartX(), 16);
    }

    private void drawRow(GraphicsContext gc, int rowIndex, double y, LayoutMetrics layout) {
        double baselineY = y + 14;
        int rowStartByte = rowIndex * HexDocModel.BYTES_PER_ROW;

        gc.setFont(layout.bodyFont());
        gc.setFill(COLOR_TEXT);
        gc.fillText(model.formatOffsetForRow(rowIndex), OFFSET_X, baselineY);

        for (int byteInRow = 0; byteInRow < HexDocModel.BYTES_PER_ROW; byteInRow++) {
            int byteIndex = rowStartByte + byteInRow;
            boolean hasByte = model.hasByte(side, byteIndex);
            boolean diff = model.isDifferentByte(byteIndex);
            double hexX = layout.hexCellX(byteInRow);
            double asciiX = layout.asciiCellX(byteInRow);

            if (diff) {
                gc.setFill(COLOR_DIFF);
                gc.fillRect(hexX - 2, y + 2, Math.max(8.0, layout.hexCellWidth() - 2), ROW_HEIGHT - 4);
                gc.fillRect(asciiX - 1, y + 2, Math.max(6.0, layout.asciiCellWidth()), ROW_HEIGHT - 4);
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

    private record LayoutMetrics(
        double offsetWidth,
        double hexCellWidth,
        double hexGroupGap,
        double asciiCellWidth,
        Font headerFont,
        Font bodyFont
    ) {
        static LayoutMetrics forWidth(double canvasWidth) {
            double availableContentWidth = Math.max(0.0, canvasWidth - OFFSET_X - RIGHT_PADDING);

            double offsetWidth = OFFSET_WIDTH;
            double hexCellWidth = HEX_CELL_WIDTH;
            double hexGroupGap = HEX_GROUP_GAP;
            double asciiCellWidth = ASCII_CELL_WIDTH;

            double requiredContentWidth = requiredWidth(offsetWidth, hexCellWidth, hexGroupGap, asciiCellWidth);
            double deficit = Math.max(0.0, requiredContentWidth - availableContentWidth);

            if (deficit > 0.0) {
                double asciiShrinkCapacity = (ASCII_CELL_WIDTH - MIN_ASCII_CELL_WIDTH) * HexDocModel.BYTES_PER_ROW;
                double asciiShrink = Math.min(deficit, asciiShrinkCapacity);
                asciiCellWidth -= asciiShrink / HexDocModel.BYTES_PER_ROW;
                deficit -= asciiShrink;
            }

            if (deficit > 0.0) {
                double hexShrinkCapacity = (HEX_CELL_WIDTH - MIN_HEX_CELL_WIDTH) * HexDocModel.BYTES_PER_ROW;
                double hexShrink = Math.min(deficit, hexShrinkCapacity);
                hexCellWidth -= hexShrink / HexDocModel.BYTES_PER_ROW;
                deficit -= hexShrink;
            }

            if (deficit > 0.0) {
                double offsetShrinkCapacity = OFFSET_WIDTH - MIN_OFFSET_WIDTH;
                double offsetShrink = Math.min(deficit, offsetShrinkCapacity);
                offsetWidth -= offsetShrink;
                deficit -= offsetShrink;
            }

            if (deficit > 0.0) {
                double gapShrinkCapacity = HEX_GROUP_GAP - MIN_HEX_GROUP_GAP;
                double gapShrink = Math.min(deficit, gapShrinkCapacity);
                hexGroupGap -= gapShrink;
            }

            boolean compact = hexCellWidth <= 19.0 || asciiCellWidth <= 9.0;
            return new LayoutMetrics(
                offsetWidth,
                hexCellWidth,
                hexGroupGap,
                asciiCellWidth,
                compact ? COMPACT_HEADER_FONT : HEADER_FONT,
                compact ? COMPACT_BODY_FONT : BODY_FONT
            );
        }

        private static double requiredWidth(
            double offsetWidth,
            double hexCellWidth,
            double hexGroupGap,
            double asciiCellWidth
        ) {
            return offsetWidth
                + (HexDocModel.BYTES_PER_ROW * hexCellWidth)
                + hexGroupGap
                + (HexDocModel.BYTES_PER_ROW * asciiCellWidth);
        }

        double hexStartX() {
            return OFFSET_X + offsetWidth;
        }

        double asciiStartX() {
            return hexStartX() + (HexDocModel.BYTES_PER_ROW * hexCellWidth) + hexGroupGap;
        }

        double hexCellX(int byteInRow) {
            double groupGap = byteInRow >= 8 ? hexGroupGap : 0.0;
            return hexStartX() + (byteInRow * hexCellWidth) + groupGap;
        }

        double asciiCellX(int byteInRow) {
            return asciiStartX() + (byteInRow * asciiCellWidth);
        }
    }
}
