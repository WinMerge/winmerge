package org.winmerge.core.io;

import java.util.concurrent.Callable;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

public final class JavaConcurrencyService implements ConcurrencyService {
    private final ExecutorService executorService;

    public JavaConcurrencyService() {
        int cores = Math.max(2, Runtime.getRuntime().availableProcessors());
        executorService = Executors.newFixedThreadPool(cores);
    }

    public JavaConcurrencyService(ExecutorService executorService) {
        this.executorService = executorService;
    }

    @Override
    public ExecutorService executor() {
        return executorService;
    }

    @Override
    public <T> Future<T> submit(Callable<T> callable) {
        return executorService.submit(callable);
    }

    @Override
    public Future<?> submit(Runnable runnable) {
        return executorService.submit(runnable);
    }

    @Override
    public void shutdown() {
        executorService.shutdown();
    }
}
