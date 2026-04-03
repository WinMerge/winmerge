package org.winmerge.desktop.ui.dialogs;

public record FilterConditionResult(
    String expression,
    String operatorExpression,
    String value1,
    String value2,
    boolean caseSensitive
) {
}
