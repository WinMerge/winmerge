package org.winmerge.core.diff;

public final class DiffRangeInfo extends DiffRange {
    private int next;
    private int prev;

    public DiffRangeInfo() {
        super();
        initLinks();
    }

    public DiffRangeInfo(DiffRange diffRange) {
        super(diffRange);
        initLinks();
    }

    public void initLinks() {
        next = -1;
        prev = -1;
    }

    public int next() {
        return next;
    }

    public void setNext(int next) {
        this.next = next;
    }

    public int prev() {
        return prev;
    }

    public void setPrev(int prev) {
        this.prev = prev;
    }
}
