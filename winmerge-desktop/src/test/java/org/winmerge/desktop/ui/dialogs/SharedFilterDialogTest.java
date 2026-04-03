package org.winmerge.desktop.ui.dialogs;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;

class SharedFilterDialogTest {
    @Test
    void resolvesSharedSelection() {
        assertEquals(SharedFilterDialog.FilterType.SHARED, SharedFilterDialog.resolveFilterType(true));
    }

    @Test
    void resolvesPrivateSelection() {
        assertEquals(SharedFilterDialog.FilterType.PRIVATE, SharedFilterDialog.resolveFilterType(false));
    }
}
