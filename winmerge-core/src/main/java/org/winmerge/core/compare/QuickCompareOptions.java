package org.winmerge.core.compare;

public class QuickCompareOptions extends CompareOptions {
    private boolean stopAfterFirstDiff;

    public QuickCompareOptions() {
        stopAfterFirstDiff = false;
    }

    public QuickCompareOptions(CompareOptions compareOptions) {
        super(compareOptions);
        stopAfterFirstDiff = false;
    }

    public boolean isStopAfterFirstDiff() {
        return stopAfterFirstDiff;
    }

    public void setStopAfterFirstDiff(boolean stopAfterFirstDiff) {
        this.stopAfterFirstDiff = stopAfterFirstDiff;
    }
}
