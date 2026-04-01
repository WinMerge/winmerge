package org.winmerge.desktop.ui.dir;

import java.io.IOException;
import java.nio.file.FileSystems;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.PathMatcher;
import java.nio.file.attribute.FileTime;
import java.time.Instant;
import java.util.ArrayList;
import java.util.Collections;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.TreeSet;
import java.util.function.Predicate;
import java.util.stream.Stream;

public final class DirDocModel {
    public enum DirStatus {
        ONLY_LEFT("Only Left", "dir-status-only-left"),
        ONLY_RIGHT("Only Right", "dir-status-only-right"),
        DIFFERENT("Different", "dir-status-different"),
        IDENTICAL("Identical", "dir-status-identical");

        private final String label;
        private final String styleClass;

        DirStatus(String label, String styleClass) {
            this.label = label;
            this.styleClass = styleClass;
        }

        public String label() {
            return label;
        }

        public String styleClass() {
            return styleClass;
        }
    }

    public record DirItem(
        String name,
        String relativePath,
        DirStatus status,
        Path leftPath,
        Path rightPath,
        Instant leftModifiedAt,
        Instant rightModifiedAt,
        Long leftSizeBytes,
        Long rightSizeBytes
    ) {
        public DirItem {
            Objects.requireNonNull(name, "name");
            Objects.requireNonNull(relativePath, "relativePath");
            Objects.requireNonNull(status, "status");
        }

        public boolean isComparableFile() {
            return leftPath != null && rightPath != null;
        }
    }

    private final Path leftRoot;
    private final Path rightRoot;
    private final String pathFilter;
    private final List<DirItem> items;

    private DirDocModel(Path leftRoot, Path rightRoot, String pathFilter, List<DirItem> items) {
        this.leftRoot = leftRoot;
        this.rightRoot = rightRoot;
        this.pathFilter = pathFilter;
        this.items = items;
    }

    public static DirDocModel load(Path leftRoot, Path rightRoot, String pathFilter) throws IOException {
        Objects.requireNonNull(leftRoot, "leftRoot");
        Objects.requireNonNull(rightRoot, "rightRoot");

        if (!Files.isDirectory(leftRoot)) {
            throw new IllegalArgumentException("Left path must be an existing directory: " + leftRoot);
        }
        if (!Files.isDirectory(rightRoot)) {
            throw new IllegalArgumentException("Right path must be an existing directory: " + rightRoot);
        }

        String normalizedFilter = pathFilter == null || pathFilter.isBlank() ? "*.*" : pathFilter.trim();
        Predicate<Path> fileFilter = buildPathFilter(normalizedFilter);

        Map<String, FileSnapshot> leftFiles = collectFiles(leftRoot, fileFilter);
        Map<String, FileSnapshot> rightFiles = collectFiles(rightRoot, fileFilter);

        TreeSet<String> orderedPaths = new TreeSet<>();
        orderedPaths.addAll(leftFiles.keySet());
        orderedPaths.addAll(rightFiles.keySet());

        List<DirItem> comparedItems = new ArrayList<>(orderedPaths.size());
        for (String relativePath : orderedPaths) {
            FileSnapshot left = leftFiles.get(relativePath);
            FileSnapshot right = rightFiles.get(relativePath);

            DirStatus status = determineStatus(left, right);
            Path leftPath = left == null ? null : left.path();
            Path rightPath = right == null ? null : right.path();

            comparedItems.add(
                new DirItem(
                    fileNameFor(relativePath),
                    relativePath,
                    status,
                    leftPath,
                    rightPath,
                    left == null ? null : left.modifiedAt().toInstant(),
                    right == null ? null : right.modifiedAt().toInstant(),
                    left == null ? null : left.size(),
                    right == null ? null : right.size()
                )
            );
        }

        return new DirDocModel(
            leftRoot,
            rightRoot,
            normalizedFilter,
            Collections.unmodifiableList(comparedItems)
        );
    }

    public Path leftRoot() {
        return leftRoot;
    }

    public Path rightRoot() {
        return rightRoot;
    }

    public String pathFilter() {
        return pathFilter;
    }

    public List<DirItem> items() {
        return items;
    }

    public long countByStatus(DirStatus status) {
        Objects.requireNonNull(status, "status");
        return items.stream().filter(item -> item.status() == status).count();
    }

    private static Map<String, FileSnapshot> collectFiles(Path root, Predicate<Path> fileFilter) throws IOException {
        Map<String, FileSnapshot> byRelativePath = new LinkedHashMap<>();
        try (Stream<Path> stream = Files.walk(root)) {
            stream
                .filter(Files::isRegularFile)
                .forEach(
                    file -> {
                        Path relative = root.relativize(file).normalize();
                        if (!fileFilter.test(relative)) {
                            return;
                        }
                        try {
                            byRelativePath.put(toKey(relative), new FileSnapshot(file, Files.size(file), Files.getLastModifiedTime(file)));
                        } catch (IOException ignored) {
                            // Keep the row and mark it as different later if metadata cannot be read.
                            byRelativePath.put(toKey(relative), new FileSnapshot(file, null, null));
                        }
                    }
                );
        }
        return byRelativePath;
    }

    private static DirStatus determineStatus(FileSnapshot left, FileSnapshot right) {
        if (left == null) {
            return DirStatus.ONLY_RIGHT;
        }
        if (right == null) {
            return DirStatus.ONLY_LEFT;
        }
        return areFilesIdentical(left, right) ? DirStatus.IDENTICAL : DirStatus.DIFFERENT;
    }

    private static boolean areFilesIdentical(FileSnapshot left, FileSnapshot right) {
        if (left.size() != null && right.size() != null && !left.size().equals(right.size())) {
            return false;
        }
        try {
            return Files.mismatch(left.path(), right.path()) == -1;
        } catch (IOException ignored) {
            return false;
        }
    }

    private static Predicate<Path> buildPathFilter(String rawFilter) {
        if (rawFilter == null || rawFilter.isBlank()) {
            return path -> true;
        }
        String normalized = rawFilter.trim();
        if ("*.*".equals(normalized) || "*".equals(normalized)) {
            return path -> true;
        }

        String[] rawPatterns = normalized.split("[;,|]");
        List<PathMatcher> matchers = new ArrayList<>();
        for (String rawPattern : rawPatterns) {
            String pattern = normalizeFilterPattern(rawPattern);
            if (pattern.isEmpty()) {
                continue;
            }
            matchers.add(FileSystems.getDefault().getPathMatcher("glob:" + pattern));
        }

        if (matchers.isEmpty()) {
            return path -> true;
        }
        return path -> {
            Path fileName = path.getFileName() == null ? path : path.getFileName();
            for (PathMatcher matcher : matchers) {
                if (matcher.matches(fileName) || matcher.matches(path)) {
                    return true;
                }
            }
            return false;
        };
    }

    private static String normalizeFilterPattern(String rawPattern) {
        if (rawPattern == null) {
            return "";
        }
        String pattern = rawPattern.trim();
        if (pattern.isEmpty()) {
            return "";
        }
        if ("*.*".equals(pattern)) {
            return "*";
        }
        if (!pattern.contains("*") && !pattern.contains("?")) {
            if (pattern.startsWith(".")) {
                return "*" + pattern;
            }
            if (!pattern.contains(".")) {
                return "*." + pattern;
            }
        }
        return pattern;
    }

    private static String toKey(Path relativePath) {
        return relativePath.toString().replace('\\', '/');
    }

    private static String fileNameFor(String relativePath) {
        Path path = Path.of(relativePath);
        Path fileName = path.getFileName();
        return fileName == null ? relativePath : fileName.toString();
    }

    private record FileSnapshot(Path path, Long size, FileTime modifiedAt) {
        private FileSnapshot {
            Objects.requireNonNull(path, "path");
        }
    }
}
