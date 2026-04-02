package org.winmerge.desktop.ui.dialogs;

import java.util.Optional;

final class OpenTableDialogLogic {
    static final String FILE_TYPE_CSV = "CSV";
    static final String FILE_TYPE_DSV = "DSV";
    static final String FILE_TYPE_CUSTOM = "CUSTOM";

    private OpenTableDialogLogic() {
    }

    static String normalizeFileType(String fileType) {
        if (FILE_TYPE_DSV.equalsIgnoreCase(fileType)) {
            return FILE_TYPE_DSV;
        }
        if (FILE_TYPE_CUSTOM.equalsIgnoreCase(fileType)) {
            return FILE_TYPE_CUSTOM;
        }
        return FILE_TYPE_CSV;
    }

    static Optional<OpenTableResult> buildResult(
        String fileType,
        String csvDelimiter,
        String dsvDelimiter,
        String customDelimiter,
        String quoteCharacter,
        boolean allowNewlinesInQuotes
    ) {
        String type = normalizeFileType(fileType);
        String validation = validationMessage(type, csvDelimiter, dsvDelimiter, customDelimiter, quoteCharacter);
        if (!validation.isEmpty()) {
            return Optional.empty();
        }

        return Optional.of(
            new OpenTableResult(
                type,
                csvDelimiter.trim(),
                dsvDelimiter.trim(),
                customDelimiter.trim(),
                quoteCharacter.trim(),
                allowNewlinesInQuotes
            )
        );
    }

    static String validationMessage(
        String fileType,
        String csvDelimiter,
        String dsvDelimiter,
        String customDelimiter,
        String quoteCharacter
    ) {
        String activeDelimiter = switch (normalizeFileType(fileType)) {
            case FILE_TYPE_DSV -> dsvDelimiter;
            case FILE_TYPE_CUSTOM -> customDelimiter;
            default -> csvDelimiter;
        };

        if (!isSingleCharacter(activeDelimiter)) {
            return "IDS_OPEN_TABLE_DELIMITER_SINGLE";
        }
        if (!isSingleCharacter(quoteCharacter)) {
            return "IDS_OPEN_TABLE_QUOTE_SINGLE";
        }
        return "";
    }

    static boolean canSubmit(
        String fileType,
        String csvDelimiter,
        String dsvDelimiter,
        String customDelimiter,
        String quoteCharacter
    ) {
        return validationMessage(fileType, csvDelimiter, dsvDelimiter, customDelimiter, quoteCharacter).isEmpty();
    }

    private static boolean isSingleCharacter(String value) {
        return value != null && value.trim().length() == 1;
    }
}
