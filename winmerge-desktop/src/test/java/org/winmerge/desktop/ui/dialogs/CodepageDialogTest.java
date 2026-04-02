package org.winmerge.desktop.ui.dialogs;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

class CodepageDialogTest {
    @Test
    void requiresAtLeastOneAffectedPaneAndSelectedCodepages() {
        assertFalse(CodepageDialogLogic.canSubmit("UTF-8", "UTF-8", false, false, false));
        assertFalse(CodepageDialogLogic.canSubmit("", "UTF-8", true, false, false));
        assertTrue(CodepageDialogLogic.canSubmit("UTF-8", "UTF-16LE", true, false, false));
    }

    @Test
    void buildsResultFromDialogState() {
        CodepageResult result = CodepageDialogLogic
            .buildResult(" UTF-8 ", " UTF-16LE ", true, true, false, true)
            .orElseThrow();

        assertEquals("UTF-8", result.loadCodepage());
        assertEquals("UTF-16LE", result.saveCodepage());
        assertTrue(result.includeBom());
        assertTrue(result.affectLeft());
        assertFalse(result.affectMiddle());
        assertTrue(result.affectRight());
    }
}
