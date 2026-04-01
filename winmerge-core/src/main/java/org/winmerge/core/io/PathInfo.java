package org.winmerge.core.io;

public final class PathInfo {
    private String path;

    public PathInfo() {
        path = "";
    }

    public String getPath(boolean normalized) {
        if (normalized) {
            return path;
        }
        if (path.isEmpty() || path.endsWith("/")) {
            return path;
        }
        return path + "/";
    }

    public String getRef() {
        return path;
    }

    public void setPath(String path) {
        this.path = path;
    }

    public void normalizePath() {
        path = normalize(path);
    }

    static String normalize(String rawPath) {
        if (rawPath == null || rawPath.isEmpty()) {
            return "";
        }
        String normalized = rawPath.replace('\\', '/');
        while (normalized.endsWith("/") && normalized.length() > 1 && !normalized.matches("^[A-Za-z]:/$")) {
            normalized = normalized.substring(0, normalized.length() - 1);
        }
        return normalized;
    }
}
