package org.winmerge.core.io;

import java.util.concurrent.Callable;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Future;

public interface ConcurrencyService extends AutoCloseable {
    static ConcurrencyService createDefault() {
        return new JavaConcurrencyService();
    }

    ExecutorService executor();

    <T> Future<T> submit(Callable<T> callable);

    Future<?> submit(Runnable runnable);

    void shutdown();

    @Override
    default void close() {
        shutdown();
    }
}
