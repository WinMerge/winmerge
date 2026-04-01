package org.winmerge.desktop.ui.dialogs;

record ComparisonResultFilterSelection(
    boolean includeMode,
    boolean threeWay,
    boolean identical,
    boolean different,
    boolean leftOnly,
    boolean rightOnly,
    boolean skipped,
    boolean middleOnly,
    boolean leftOnlyDifferent,
    boolean middleOnlyDifferent,
    boolean rightOnlyDifferent,
    boolean leftOnlyMissing,
    boolean middleOnlyMissing,
    boolean rightOnlyMissing
) {
}
