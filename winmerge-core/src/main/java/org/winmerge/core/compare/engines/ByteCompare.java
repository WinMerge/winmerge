package org.winmerge.core.compare.engines;

import org.winmerge.core.compare.QuickCompareOptions;

public final class ByteCompare extends ByteComparator {
    private final QuickCompareOptions options;

    public ByteCompare() {
        this.options = null;
    }

    public ByteCompare(QuickCompareOptions options) {
        this.options = options;
    }

    @Override
    public CompareEngineResult compare(CompareEngineContext context) {
        if (options != null) {
            context.setQuickCompareOptions(options);
        }
        return super.compare(context);
    }
}
