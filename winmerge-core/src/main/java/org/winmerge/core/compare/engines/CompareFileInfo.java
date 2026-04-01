package org.winmerge.core.compare.engines;

import org.winmerge.core.io.FileTextEncoding;

public final class CompareFileInfo {
    public static final long FILE_SIZE_NONE = -1L;
    public static final long TIME_VALUE_NONE = Long.MIN_VALUE;

    private boolean exists;
    private long size;
    private long modifiedTimeMillis;
    private FileTextEncoding encoding;

    public CompareFileInfo() {
        clear();
    }

    public void clear() {
        exists = false;
        size = FILE_SIZE_NONE;
        modifiedTimeMillis = TIME_VALUE_NONE;
        encoding = new FileTextEncoding();
    }

    public boolean exists() {
        return exists;
    }

    public void setExists(boolean exists) {
        this.exists = exists;
    }

    public long size() {
        return size;
    }

    public void setSize(long size) {
        this.size = size;
    }

    public long modifiedTimeMillis() {
        return modifiedTimeMillis;
    }

    public void setModifiedTimeMillis(long modifiedTimeMillis) {
        this.modifiedTimeMillis = modifiedTimeMillis;
    }

    public FileTextEncoding encoding() {
        return encoding;
    }

    public void setEncoding(FileTextEncoding encoding) {
        this.encoding = encoding;
    }
}
