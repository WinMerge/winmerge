package org.winmerge.desktop.i18n;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.Locale;

import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.Test;

class I18nTest {

    @AfterEach
    void resetLocale() {
        Locale.setDefault(Locale.ENGLISH);
        I18n.resetForTests();
    }

    @Test
    void shouldResolveAppTitleAliasKey() {
        Locale.setDefault(Locale.ENGLISH);
        I18n.resetForTests();

        assertEquals("WinMerge", I18n.tr("IDS_APP_TITLE"));
    }

    @Test
    void shouldFallbackToKeyForMissingEntry() {
        Locale.setDefault(Locale.ENGLISH);
        I18n.resetForTests();

        assertEquals("missing_key", I18n.tr("missing_key"));
    }

    @Test
    void shouldFormatPositionalArguments() {
        Locale.setDefault(Locale.ENGLISH);
        I18n.resetForTests();

        assertEquals("Version 1.2.3", I18n.tr("IDS_VERSION_FMT", "1.2.3"));
    }

    @Test
    void shouldLoadLocalizedBundles() {
        I18n.setLocale(Locale.GERMAN);
        assertEquals("Allgemein", I18n.tr("IDS_OPTIONSPG_GENERAL"));
    }

    @Test
    void shouldFormatPersianStringsWithLiteralBraces() {
        I18n.setLocale(Locale.forLanguageTag("fa"));

        String filesAffected = assertDoesNotThrow(() -> I18n.tr("IDS_FILES_AFFECTED_FMT", "5"));
        String filesAffected2 = assertDoesNotThrow(() -> I18n.tr("IDS_FILES_AFFECTED_FMT2", "5", "9"));
        String replaced = assertDoesNotThrow(() -> I18n.tr("IDS_NUM_REPLACED", "3"));
        String selectedFiles = assertDoesNotThrow(() -> I18n.tr("IDS_DIFF_SELECTEDFILES", "7"));

        assertTrue(filesAffected.contains("5"));
        assertTrue(filesAffected2.contains("5"));
        assertTrue(filesAffected2.contains("9"));
        assertTrue(filesAffected.trim().startsWith("{"));
        assertTrue(filesAffected.trim().endsWith("}"));
        assertTrue(filesAffected2.trim().startsWith("{"));
        assertTrue(filesAffected2.trim().endsWith("}"));
        assertTrue(replaced.contains("3"));
        assertTrue(replaced.contains("{"));
        assertTrue(replaced.contains("}"));
        assertTrue(selectedFiles.contains("7"));
        assertTrue(selectedFiles.trim().startsWith("{"));
        assertTrue(selectedFiles.trim().endsWith("}"));
    }
}
