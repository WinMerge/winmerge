package org.winmerge.desktop.ui.merge;

import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertTrue;

class MergeLoadExecutorTest {
    @Test
    void closeCancelsRunningTaskAndShutsDownExecutor() throws Exception {
        MergeLoadExecutor executor = new MergeLoadExecutor("merge-load-test");
        CountDownLatch started = new CountDownLatch(1);
        CountDownLatch interrupted = new CountDownLatch(1);

        executor.submit(() -> {
            started.countDown();
            try {
                Thread.sleep(10_000);
            } catch (InterruptedException interruption) {
                interrupted.countDown();
                Thread.currentThread().interrupt();
            }
        });

        assertTrue(started.await(1, TimeUnit.SECONDS));
        executor.close();

        assertTrue(interrupted.await(1, TimeUnit.SECONDS));
        assertTrue(executor.isShutdown());
    }

    @Test
    void submittingNextTaskCancelsPreviousTask() throws Exception {
        MergeLoadExecutor executor = new MergeLoadExecutor("merge-load-test");
        CountDownLatch firstStarted = new CountDownLatch(1);
        CountDownLatch firstInterrupted = new CountDownLatch(1);
        CountDownLatch secondRan = new CountDownLatch(1);

        executor.submit(() -> {
            firstStarted.countDown();
            try {
                Thread.sleep(10_000);
            } catch (InterruptedException interruption) {
                firstInterrupted.countDown();
                Thread.currentThread().interrupt();
            }
        });

        assertTrue(firstStarted.await(1, TimeUnit.SECONDS));
        executor.submit(secondRan::countDown);

        assertTrue(firstInterrupted.await(1, TimeUnit.SECONDS));
        assertTrue(secondRan.await(1, TimeUnit.SECONDS));
        executor.close();
    }
}
