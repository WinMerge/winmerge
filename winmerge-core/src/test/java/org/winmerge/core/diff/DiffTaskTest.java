package org.winmerge.core.diff;

import org.junit.jupiter.api.Test;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.Executors;
import java.util.concurrent.FutureTask;
import java.util.concurrent.atomic.AtomicBoolean;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

class DiffTaskTest {
    @Test
    void runsCollectAndCompareWithEvents() throws Exception {
        DiffTask.DiffTaskData data = new DiffTask.DiffTaskData();
        List<DiffTask.ThreadEvent> events = new ArrayList<>();
        data.addListener(events::add);

        AtomicBoolean collectRan = new AtomicBoolean(false);
        AtomicBoolean compareRan = new AtomicBoolean(false);

        data.setCollectFunction(ctx -> {
            collectRan.set(true);
            ctx.semaphore().release();
        });
        data.setCompareFunction(ctx -> {
            compareRan.set(true);
            try {
                ctx.semaphore().acquire();
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        });

        try (DiffTask task = new DiffTask(data, Executors.newFixedThreadPool(2))) {
            DiffTask.DiffResult result = task.call();
            assertTrue(collectRan.get());
            assertTrue(compareRan.get());
            assertEquals(DiffTask.ThreadState.COMPLETED, result.collectThreadState());
            assertEquals(DiffTask.ThreadState.COMPLETED, result.compareThreadState());
            assertFalse(result.aborted());
            assertTrue(events.contains(DiffTask.ThreadEvent.COLLECT_COMPLETED));
            assertTrue(events.contains(DiffTask.ThreadEvent.COMPARE_COMPLETED));
        }
    }

    @Test
    void shouldAbortRespectsPauseAndAbort() throws Exception {
        DiffTask task = new DiffTask(new DiffTask.DiffTaskData(), Executors.newSingleThreadExecutor());
        task.pause();

        FutureTask<Boolean> shouldAbortTask = new FutureTask<>(task::shouldAbort);
        Thread t = new Thread(shouldAbortTask);
        t.start();

        Thread.sleep(120);
        assertFalse(shouldAbortTask.isDone());

        task.abort();
        assertTrue(shouldAbortTask.get());
        task.close();
    }

    @Test
    void compareProgressEventCanBeEmitted() {
        DiffTask.DiffTaskData data = new DiffTask.DiffTaskData();
        List<DiffTask.ThreadEvent> events = new ArrayList<>();
        data.addListener(events::add);
        try (DiffTask task = new DiffTask(data, Executors.newSingleThreadExecutor())) {
            task.notifyCompareProgressed();
            assertEquals(1, events.size());
            assertEquals(DiffTask.ThreadEvent.COMPARE_PROGRESSED, events.get(0));
        }
    }
}
