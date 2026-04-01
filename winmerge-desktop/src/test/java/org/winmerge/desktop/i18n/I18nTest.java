package org.winmerge.desktop.i18n;

import static org.junit.jupiter.api.Assertions.assertEquals;

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
}
