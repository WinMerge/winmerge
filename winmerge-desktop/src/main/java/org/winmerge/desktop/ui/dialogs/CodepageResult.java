package org.winmerge.desktop.ui.dialogs;

public record CodepageResult(
    String loadCodepage,
    String saveCodepage,
    boolean includeBom,
    boolean affectLeft,
    boolean affectMiddle,
    boolean affectRight
) {
}
