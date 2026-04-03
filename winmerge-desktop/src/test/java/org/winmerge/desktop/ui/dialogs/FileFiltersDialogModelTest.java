package org.winmerge.desktop.ui.dialogs;

import java.util.concurrent.atomic.AtomicBoolean;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

class FileFiltersDialogModelTest {
    @Test
    void addsAndDeletesFiltersWithDeleteHandler() {
        FileFiltersDialogModel model = new FileFiltersDialogModel();
        model.setDeleteHandler(entry -> true);

        model.addFilter("A", "desc", "path/A.flt", true);
        assertEquals(1, model.filters().size());

        boolean deleted = model.deleteFilterAt(0);
        assertTrue(deleted);
        assertTrue(model.filters().isEmpty());
    }

    @Test
    void reportsDeleteFailureWithoutRemovingItem() {
        FileFiltersDialogModel model = new FileFiltersDialogModel();
        model.addFilter("A", "desc", "path/A.flt", true);
        model.setDeleteHandler(entry -> false);

        boolean deleted = model.deleteFilterAt(0);

        assertFalse(deleted);
        assertEquals(1, model.filters().size());
    }

    @Test
    void validatesMaskExpression() {
        assertTrue(FileFiltersDialogModel.isMaskValid("*.txt|fe:Size > 100KB"));
        assertFalse(FileFiltersDialogModel.isMaskValid(""));
        assertFalse(FileFiltersDialogModel.isMaskValid("   "));
    }

    @Test
    void cancelsDirWatcherOnClose() {
        FileFiltersDialogModel model = new FileFiltersDialogModel();
        AtomicBoolean cancelled = new AtomicBoolean(false);
        model.setDirWatcher(() -> cancelled.set(true));

        model.cancelDirWatcher();

        assertTrue(cancelled.get());
    }
}
