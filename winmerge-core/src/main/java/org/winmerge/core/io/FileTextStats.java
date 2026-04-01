package org.winmerge.core.io;

public final class FileTextStats {
    private int crCount;
    private int lfCount;
    private int crlfCount;
    private int zeroCount;

    public FileTextStats() {
        clear();
    }

    public void clear() {
        crCount = 0;
        lfCount = 0;
        crlfCount = 0;
        zeroCount = 0;
    }

    public int getCrCount() {
        return crCount;
    }

    public void setCrCount(int crCount) {
        this.crCount = crCount;
    }

    public int getLfCount() {
        return lfCount;
    }

    public void setLfCount(int lfCount) {
        this.lfCount = lfCount;
    }

    public int getCrlfCount() {
        return crlfCount;
    }

    public void setCrlfCount(int crlfCount) {
        this.crlfCount = crlfCount;
    }

    public int getZeroCount() {
        return zeroCount;
    }

    public void setZeroCount(int zeroCount) {
        this.zeroCount = zeroCount;
    }
}
