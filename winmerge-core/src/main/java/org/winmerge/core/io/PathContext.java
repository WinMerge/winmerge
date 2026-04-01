package org.winmerge.core.io;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

public final class PathContext implements Iterable<String> {
    private int files;
    private final PathInfo[] path;

    public PathContext() {
        files = 0;
        path = new PathInfo[] {new PathInfo(), new PathInfo(), new PathInfo()};
    }

    public PathContext(String left) {
        this();
        files = 1;
        path[0].setPath(left);
    }

    public PathContext(String left, String right) {
        this();
        files = 2;
        path[0].setPath(left);
        path[1].setPath(right);
    }

    public PathContext(String left, String middle, String right) {
        this();
        files = 3;
        path[0].setPath(left);
        path[1].setPath(middle);
        path[2].setPath(right);
    }

    public PathContext(List<String> paths) {
        this();
        files = clampFileCount(paths.size());
        for (int i = 0; i < files; i++) {
            path[i].setPath(paths.get(i));
        }
    }

    public String getAt(int index) {
        ensureValidIndex(index);
        return path[index].getPath(true);
    }

    public String getElement(int index) {
        ensureValidIndex(index);
        return path[index].getRef();
    }

    public void setAt(int index, String newElement) {
        ensureValidIndex(index);
        path[index].setPath(newElement);
    }

    public String getLeft(boolean normalized) {
        if (files == 0) {
            return "";
        }
        return path[0].getPath(normalized);
    }

    public String getRight(boolean normalized) {
        if (files < 2) {
            return "";
        }
        return path[files - 1].getPath(normalized);
    }

    public String getMiddle(boolean normalized) {
        if (files < 3) {
            return "";
        }
        return path[1].getPath(normalized);
    }

    public String getPath(int index, boolean normalized) {
        ensureArrayIndex(index);
        return path[index].getPath(normalized);
    }

    public void setLeft(String newPath, boolean normalized) {
        if (files == 0) {
            files = 1;
        }
        path[0].setPath(newPath);
        if (normalized) {
            path[0].normalizePath();
        }
    }

    public void setRight(String newPath, boolean normalized) {
        if (files < 2) {
            files = 2;
        }
        path[files - 1].setPath(newPath);
        if (normalized) {
            path[files - 1].normalizePath();
        }
    }

    public void setMiddle(String newPath, boolean normalized) {
        if (files < 3) {
            files = 3;
            path[2].setPath(path[1].getRef());
        }
        path[1].setPath(newPath);
        if (normalized) {
            path[1].normalizePath();
        }
    }

    public void setPath(int index, String newPath, boolean normalized) {
        ensureArrayIndex(index);
        if (index >= files) {
            files = index + 1;
        }
        path[index].setPath(newPath);
        if (normalized) {
            path[index].normalizePath();
        }
    }

    public void setSize(int files) {
        this.files = clampFileCount(files);
    }

    public int getSize() {
        return files;
    }

    public void removeAll() {
        files = 0;
        path[0].setPath("");
        path[1].setPath("");
        path[2].setPath("");
    }

    public void swap(int fromIndex, int toIndex) {
        if (fromIndex >= 0 && fromIndex < files && toIndex >= 0 && toIndex < files) {
            String from = path[fromIndex].getRef();
            path[fromIndex].setPath(path[toIndex].getRef());
            path[toIndex].setPath(from);
        }
    }

    @Override
    public Iterator<String> iterator() {
        List<String> values = new ArrayList<>(files);
        for (int i = 0; i < files; i++) {
            values.add(path[i].getPath(true));
        }
        return values.iterator();
    }

    private void ensureValidIndex(int index) {
        if (index < 0 || index >= files) {
            throw new IllegalArgumentException("index out of range for active path count");
        }
    }

    private void ensureArrayIndex(int index) {
        if (index < 0 || index >= path.length) {
            throw new IllegalArgumentException("index out of range for path array");
        }
    }

    private int clampFileCount(int candidate) {
        return Math.max(0, Math.min(candidate, path.length));
    }
}
