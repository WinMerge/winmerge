package org.winmerge.desktop.ui.dialogs;

public record OpenTableRequest(
    String fileType,
    String csvDelimiter,
    String dsvDelimiter,
    String customDelimiter,
    String quoteCharacter,
    boolean allowNewlinesInQuotes
) {
    public OpenTableRequest {
        fileType = OpenTableDialogLogic.normalizeFileType(fileType);
        csvDelimiter = normalizeCharValue(csvDelimiter, ",");
        dsvDelimiter = normalizeCharValue(dsvDelimiter, ";");
        customDelimiter = normalizeCharValue(customDelimiter, "|");
        quoteCharacter = normalizeCharValue(quoteCharacter, "\"");
    }

    private static String normalizeCharValue(String value, String fallback) {
        if (value == null || value.isBlank()) {
            return fallback;
        }
        return value.trim();
    }
}
