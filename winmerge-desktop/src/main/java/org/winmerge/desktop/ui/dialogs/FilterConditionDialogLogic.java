package org.winmerge.desktop.ui.dialogs;

import java.time.LocalDate;
import java.util.List;
import java.util.Optional;
import java.util.Set;

import org.winmerge.desktop.i18n.I18n;

final class FilterConditionDialogLogic {
    private static final Set<String> NUMERIC_FIELDS = Set.of(
        "Size",
        "TotalSize",
        "Files",
        "Items",
        "Differences",
        "IgnoredDiffs"
    );

    private FilterConditionDialogLogic() {
    }

    static List<OperatorOption> operatorsForField(String field, String lhsExpression) {
        if (isNumericField(field, lhsExpression)) {
            return List.of(
                new OperatorOption("IDS_FILTER_OP_EQUALS", "%1 = %2"),
                new OperatorOption("IDS_FILTER_OP_NOT_EQUALS", "%1 != %2"),
                new OperatorOption("IDS_FILTER_OP_LESS_THAN", "%1 < %2"),
                new OperatorOption("IDS_FILTER_OP_LESS_EQUAL", "%1 <= %2"),
                new OperatorOption("IDS_FILTER_OP_GREATER_EQUAL", "%1 >= %2"),
                new OperatorOption("IDS_FILTER_OP_GREATER_THAN", "%1 > %2"),
                new OperatorOption("IDS_FILTER_OP_BETWEEN", "isWithin(%1, %2, %3)"),
                new OperatorOption("IDS_FILTER_OP_NOT_BETWEEN", "not isWithin(%1, %2, %3)")
            );
        }
        if ("Content".equals(field)) {
            return List.of(
                new OperatorOption("IDS_FILTER_OP_CONTAINS", "%1 contains %2"),
                new OperatorOption("IDS_FILTER_OP_NOT_CONTAINS", "%1 not contains %2"),
                new OperatorOption("IDS_FILTER_OP_RECONTAINS", "%1 recontains %2"),
                new OperatorOption("IDS_FILTER_OP_NOT_RECONTAINS", "%1 not recontains %2")
            );
        }
        return List.of(
            new OperatorOption("IDS_FILTER_OP_EQUALS", "%1 = %2"),
            new OperatorOption("IDS_FILTER_OP_NOT_EQUALS", "%1 != %2"),
            new OperatorOption("IDS_FILTER_OP_LIKE", "%1 like %2"),
            new OperatorOption("IDS_FILTER_OP_NOT_LIKE", "%1 not like %2"),
            new OperatorOption("IDS_FILTER_OP_MATCHES", "%1 matches %2"),
            new OperatorOption("IDS_FILTER_OP_NOT_MATCHES", "%1 not matches %2"),
            new OperatorOption("IDS_FILTER_OP_CONTAINS", "%1 contains %2"),
            new OperatorOption("IDS_FILTER_OP_NOT_CONTAINS", "%1 not contains %2"),
            new OperatorOption("IDS_FILTER_OP_RECONTAINS", "%1 recontains %2"),
            new OperatorOption("IDS_FILTER_OP_NOT_RECONTAINS", "%1 not recontains %2")
        );
    }

    static boolean usesSecondValue(String operatorExpression) {
        return operatorExpression != null && operatorExpression.contains("%3");
    }

    static boolean isDateField(String field, String lhsExpression) {
        return "DateStr".equals(field) || "toDateStr(%1)".equals(lhsExpression);
    }

    static boolean isNumericField(String field, String lhsExpression) {
        return NUMERIC_FIELDS.contains(field) || "lineCount(%1)".equals(lhsExpression);
    }

    static Optional<FilterConditionResult> buildResult(
        FilterConditionRequest request,
        OperatorOption operator,
        String value1,
        String value2,
        LocalDate dateValue1,
        LocalDate dateValue2,
        boolean caseSensitive
    ) {
        if (request == null || operator == null) {
            return Optional.empty();
        }

        String lhs = request.lhsExpression().isBlank() ? request.field() : request.lhsExpression();
        if (lhs.isBlank()) {
            return Optional.empty();
        }

        String normalizedValue1;
        String normalizedValue2 = "";
        if (isDateField(request.field(), request.lhsExpression())) {
            if (dateValue1 == null) {
                return Optional.empty();
            }
            normalizedValue1 = dateValue1.toString();
            if (usesSecondValue(operator.expressionTemplate())) {
                if (dateValue2 == null) {
                    return Optional.empty();
                }
                normalizedValue2 = dateValue2.toString();
            }
        } else {
            normalizedValue1 = value1 == null ? "" : value1.trim();
            if (normalizedValue1.isEmpty()) {
                return Optional.empty();
            }
            if (usesSecondValue(operator.expressionTemplate())) {
                normalizedValue2 = value2 == null ? "" : value2.trim();
                if (normalizedValue2.isEmpty()) {
                    return Optional.empty();
                }
            }
        }

        String expression = operator.expressionTemplate()
            .replace("%1", lhs)
            .replace("%2", formatValue(request, normalizedValue1));
        if (usesSecondValue(operator.expressionTemplate())) {
            expression = expression.replace("%3", formatValue(request, normalizedValue2));
        }

        if (caseSensitive && !isNumericField(request.field(), request.lhsExpression())) {
            expression = "@cs " + expression;
        }

        return Optional.of(new FilterConditionResult(
            expression,
            operator.expressionTemplate(),
            normalizedValue1,
            normalizedValue2,
            caseSensitive
        ));
    }

    private static String formatValue(FilterConditionRequest request, String value) {
        if (isNumericField(request.field(), request.lhsExpression())) {
            return value;
        }
        return '"' + value.replace("\"", "\"\"") + '"';
    }

    record OperatorOption(String labelKey, String expressionTemplate) {
        String label() {
            return I18n.tr(labelKey);
        }
    }
}
