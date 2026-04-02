package org.winmerge.desktop.ui.dialogs;

import java.util.List;
import java.util.Objects;
import java.util.concurrent.CompletableFuture;
import java.util.function.Supplier;

public record CodepageRequest(
    int fileCount,
    String loadCodepage,
    String saveCodepage,
    boolean includeBom,
    boolean affectLeft,
    boolean affectMiddle,
    boolean affectRight,
    Supplier<CompletableFuture<List<String>>> codepagesLoader
) {
    private static final List<String> DEFAULT_CODEPAGES = List.of(
        "UTF-8",
        "UTF-16LE",
        "UTF-16BE",
        "windows-1252",
        "ISO-8859-1"
    );

    public CodepageRequest {
        fileCount = Math.max(2, fileCount);
        loadCodepage = normalizeCodepage(loadCodepage);
        saveCodepage = normalizeCodepage(saveCodepage);

        if (codepagesLoader == null) {
            codepagesLoader = () -> CompletableFuture.completedFuture(DEFAULT_CODEPAGES);
        }
    }

    private static String normalizeCodepage(String value) {
        return value == null || value.isBlank() ? "UTF-8" : value.trim();
    }

    public static CodepageRequest defaults(int fileCount) {
        return new CodepageRequest(fileCount, "UTF-8", "UTF-8", false, true, fileCount > 2, true, null);
    }

    public Supplier<CompletableFuture<List<String>>> safeCodepagesLoader() {
        Supplier<CompletableFuture<List<String>>> loader = codepagesLoader;
        return () -> {
            CompletableFuture<List<String>> future = loader.get();
            if (future == null) {
                return CompletableFuture.completedFuture(DEFAULT_CODEPAGES);
            }
            return future.thenApply(list -> {
                List<String> values = list == null || list.isEmpty() ? DEFAULT_CODEPAGES : List.copyOf(list);
                return values.stream().filter(Objects::nonNull).map(String::trim).filter(s -> !s.isEmpty()).toList();
            });
        };
    }

    public List<String> defaultCodepages() {
        return DEFAULT_CODEPAGES;
    }
}
