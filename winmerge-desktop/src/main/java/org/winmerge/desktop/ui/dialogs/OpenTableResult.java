package org.winmerge.desktop.ui.dialogs;

public record OpenTableResult(
    String fileType,
    String csvDelimiter,
    String dsvDelimiter,
    String customDelimiter,
    String quoteCharacter,
    boolean allowNewlinesInQuotes
) {
}
