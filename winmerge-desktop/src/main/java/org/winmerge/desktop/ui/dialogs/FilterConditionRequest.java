package org.winmerge.desktop.ui.dialogs;

public record FilterConditionRequest(
    String field,
    String lhsExpression,
    String initialOperatorExpression,
    String initialValue1,
    String initialValue2,
    boolean initialCaseSensitive
) {
    public FilterConditionRequest {
        field = field == null ? "" : field;
        lhsExpression = lhsExpression == null ? field : lhsExpression;
        initialOperatorExpression = initialOperatorExpression == null ? "" : initialOperatorExpression;
        initialValue1 = initialValue1 == null ? "" : initialValue1;
        initialValue2 = initialValue2 == null ? "" : initialValue2;
    }
}
