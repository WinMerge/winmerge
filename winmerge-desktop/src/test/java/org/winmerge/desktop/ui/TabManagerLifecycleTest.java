package org.winmerge.desktop.ui;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertTrue;

class TabManagerLifecycleTest {
    @Test
    void disposeControllerClosesAutoCloseableControllers() {
        RecordingCloseable closeable = new RecordingCloseable();
        TabManager.disposeController(closeable);
        assertTrue(closeable.closed);
    }

    @Test
    void disposeControllerIgnoresNonCloseableControllers() {
        TabManager.disposeController(new Object());
    }

    private static final class RecordingCloseable implements AutoCloseable {
        private boolean closed;

        @Override
        public void close() {
            closed = true;
        }
    }
}
