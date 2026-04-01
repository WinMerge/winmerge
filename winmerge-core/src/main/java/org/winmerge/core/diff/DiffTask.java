package org.winmerge.core.diff;

import java.util.List;
import java.util.Objects;
import java.util.concurrent.Callable;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.Semaphore;
import java.util.concurrent.TimeUnit;
import java.util.function.Consumer;

public final class DiffTask implements Callable<DiffTask.DiffResult>, AutoCloseable {
    public enum ThreadState {
        NOT_STARTED,
        COMPARING,
        COMPLETED
    }

    public enum ThreadEvent {
        COLLECT_COMPLETED(2),
        COMPARE_PROGRESSED(1),
        COMPARE_COMPLETED(0);

        private final int code;

        ThreadEvent(int code) {
            this.code = code;
        }

        public int code() {
            return code;
        }
    }

    public interface Abortable {
        boolean shouldAbort();
    }

    public static final class DiffTaskData {
        private volatile Object context;
        private volatile ThreadState threadState = ThreadState.NOT_STARTED;
        private volatile ThreadState collectThreadState = ThreadState.NOT_STARTED;
        private volatile int threadCount = 1;
        private volatile boolean markedRescan = false;
        private volatile Abortable abortGate;
        private volatile Consumer<DiffTaskData> collectFunction;
        private volatile Consumer<DiffTaskData> compareFunction;

        private final List<Consumer<ThreadEvent>> listeners = new CopyOnWriteArrayList<>();
        private final CountDownLatch collectCompletedEvent = new CountDownLatch(1);
        private final Semaphore semaphore = new Semaphore(0);

        public Object context() {
            return context;
        }

        public void setContext(Object context) {
            this.context = context;
        }

        public ThreadState threadState() {
            return threadState;
        }

        private void setThreadState(ThreadState state) {
            this.threadState = state;
        }

        public ThreadState collectThreadState() {
            return collectThreadState;
        }

        private void setCollectThreadState(ThreadState state) {
            this.collectThreadState = state;
        }

        public int threadCount() {
            return threadCount;
        }

        public void setThreadCount(int threadCount) {
            if (threadCount < 1) {
                throw new IllegalArgumentException("threadCount must be >= 1");
            }
            this.threadCount = threadCount;
        }

        public boolean markedRescan() {
            return markedRescan;
        }

        public void setMarkedRescan(boolean markedRescan) {
            this.markedRescan = markedRescan;
        }

        public Abortable abortGate() {
            return abortGate;
        }

        private void setAbortGate(Abortable abortGate) {
            this.abortGate = abortGate;
        }

        public Consumer<DiffTaskData> collectFunction() {
            return collectFunction;
        }

        public void setCollectFunction(Consumer<DiffTaskData> collectFunction) {
            this.collectFunction = collectFunction;
        }

        public Consumer<DiffTaskData> compareFunction() {
            return compareFunction;
        }

        public void setCompareFunction(Consumer<DiffTaskData> compareFunction) {
            this.compareFunction = compareFunction;
        }

        public Semaphore semaphore() {
            return semaphore;
        }

        public CountDownLatch collectCompletedEvent() {
            return collectCompletedEvent;
        }

        public void addListener(Consumer<ThreadEvent> listener) {
            listeners.add(listener);
        }

        public void removeListener(Consumer<ThreadEvent> listener) {
            listeners.remove(listener);
        }

        private void notifyListeners(ThreadEvent event) {
            for (Consumer<ThreadEvent> listener : listeners) {
                listener.accept(event);
            }
        }
    }

    public record DiffResult(
            ThreadState compareThreadState,
            ThreadState collectThreadState,
            boolean aborted,
            boolean markedRescan) {
    }

    private final DiffTaskData taskData;
    private final ExecutorService executor;

    private volatile boolean aborting;
    private volatile boolean paused;

    public DiffTask() {
        this(new DiffTaskData(), Executors.newFixedThreadPool(2));
    }

    public DiffTask(DiffTaskData taskData, ExecutorService executor) {
        this.taskData = Objects.requireNonNull(taskData, "taskData");
        this.executor = Objects.requireNonNull(executor, "executor");
        this.aborting = false;
        this.paused = false;
        this.taskData.setAbortGate(this::shouldAbort);
    }

    public DiffTaskData data() {
        return taskData;
    }

    public void abort() {
        aborting = true;
    }

    public void pause() {
        paused = true;
    }

    public void resume() {
        paused = false;
    }

    public boolean isAborting() {
        return aborting;
    }

    public boolean isPaused() {
        return paused;
    }

    public boolean shouldAbort() {
        while (paused && !aborting) {
            try {
                TimeUnit.MILLISECONDS.sleep(100);
            } catch (InterruptedException interrupted) {
                Thread.currentThread().interrupt();
                return true;
            }
        }
        return aborting;
    }

    public void notifyCompareProgressed() {
        taskData.notifyListeners(ThreadEvent.COMPARE_PROGRESSED);
    }

    @Override
    public DiffResult call() throws Exception {
        taskData.setThreadState(ThreadState.COMPARING);
        taskData.setCollectThreadState(ThreadState.COMPARING);
        aborting = false;
        paused = false;

        Future<?> collectFuture = executor.submit(this::runCollect);
        Future<?> compareFuture = executor.submit(this::runCompare);

        collectFuture.get();
        compareFuture.get();

        return new DiffResult(taskData.threadState(), taskData.collectThreadState(), aborting, taskData.markedRescan());
    }

    @Override
    public void close() {
        executor.shutdownNow();
    }

    private void runCollect() {
        Consumer<DiffTaskData> collectFunction = taskData.collectFunction();
        if (collectFunction != null) {
            collectFunction.accept(taskData);
        }
        taskData.semaphore().release();
        taskData.setCollectThreadState(ThreadState.COMPLETED);
        taskData.notifyListeners(ThreadEvent.COLLECT_COMPLETED);
        taskData.collectCompletedEvent().countDown();
    }

    private void runCompare() {
        Consumer<DiffTaskData> compareFunction = taskData.compareFunction();
        if (compareFunction != null) {
            compareFunction.accept(taskData);
        }
        taskData.setThreadState(ThreadState.COMPLETED);
        taskData.notifyListeners(ThreadEvent.COMPARE_COMPLETED);
    }
}
