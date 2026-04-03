package org.winmerge.desktop.ui.dialogs;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;

class SubstitutionFiltersDialogModelTest {
    @Test
    void enablingRegexDisablesWholeWordFlag() {
        SubstitutionFiltersDialogModel model = new SubstitutionFiltersDialogModel();
        SubstitutionFiltersDialogModel.SubstitutionFilterEntry entry = model.addEntry(
            "foo",
            "bar",
            false,
            false,
            true,
            true
        );

        entry.setUseRegExp(true);

        assertFalse(entry.isMatchWholeWordOnly());
    }

    @Test
    void clearRemovesAllEntries() {
        SubstitutionFiltersDialogModel model = new SubstitutionFiltersDialogModel();
        model.addEntry("a", "b", false, false, false, true);
        model.addEntry("c", "d", false, false, false, true);

        model.clearEntries();

        assertEquals(0, model.entries().size());
    }
}
