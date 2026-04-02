package org.winmerge.desktop.ui.dialogs;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

class OpenTableDialogTest {
    @Test
    void validatesDelimiterAsSingleCharacter() {
        assertFalse(OpenTableDialogLogic.canSubmit("CSV", ",,", ";", "|", "\""));
        assertFalse(OpenTableDialogLogic.canSubmit("CUSTOM", ",", ";", "::", "\""));
        assertTrue(OpenTableDialogLogic.canSubmit("DSV", ",", ";", "|", "\""));
    }

    @Test
    void buildsResultForSelectedType() {
        OpenTableResult result = OpenTableDialogLogic
            .buildResult("CUSTOM", ",", ";", "|", "\"", true)
            .orElseThrow();

        assertEquals(OpenTableDialogLogic.FILE_TYPE_CUSTOM, result.fileType());
        assertEquals(",", result.csvDelimiter());
        assertEquals(";", result.dsvDelimiter());
        assertEquals("|", result.customDelimiter());
        assertEquals("\"", result.quoteCharacter());
        assertTrue(result.allowNewlinesInQuotes());
    }
}
