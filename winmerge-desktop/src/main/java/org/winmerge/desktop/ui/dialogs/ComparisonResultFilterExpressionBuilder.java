package org.winmerge.desktop.ui.dialogs;

import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

final class ComparisonResultFilterExpressionBuilder {
    private ComparisonResultFilterExpressionBuilder() {
    }

    static Optional<String> build(ComparisonResultFilterSelection selection) {
        List<String> conditions = new ArrayList<>();

        if (selection.identical()) {
            conditions.add("Identical");
        }
        if (selection.different()) {
            conditions.add("Different");
        }
        if (selection.skipped()) {
            conditions.add("Skipped");
        }

        if (selection.threeWay()) {
            if (selection.leftOnly()) {
                conditions.add("LeftExists and not MiddleExists and not RightExists");
            }
            if (selection.rightOnly()) {
                conditions.add("not LeftExists and not MiddleExists and RightExists");
            }
            if (selection.middleOnly()) {
                conditions.add("not LeftExists and MiddleExists and not RightExists");
            }
            if (selection.leftOnlyDifferent()) {
                conditions.add("DifferentLeftMiddle and not DifferentMiddleRight");
            }
            if (selection.middleOnlyDifferent()) {
                conditions.add("not DifferentLeftMiddle and DifferentMiddleRight and DifferentLeftRight");
            }
            if (selection.rightOnlyDifferent()) {
                conditions.add("not DifferentLeftMiddle and DifferentMiddleRight and not DifferentLeftRight");
            }
            if (selection.leftOnlyMissing()) {
                conditions.add("not LeftExists and MiddleExists and RightExists");
            }
            if (selection.middleOnlyMissing()) {
                conditions.add("LeftExists and not MiddleExists and RightExists");
            }
            if (selection.rightOnlyMissing()) {
                conditions.add("LeftExists and MiddleExists and not RightExists");
            }
        } else {
            if (selection.leftOnly()) {
                conditions.add("LeftExists and not RightExists");
            }
            if (selection.rightOnly()) {
                conditions.add("not LeftExists and RightExists");
            }
        }

        if (conditions.isEmpty()) {
            return Optional.empty();
        }

        boolean excludeMode = !selection.includeMode();
        if (conditions.size() == 1) {
            String single = conditions.get(0);
            if (excludeMode) {
                return Optional.of("not (" + single + ")");
            }
            return Optional.of(single);
        }

        StringBuilder expression = new StringBuilder();
        expression.append('(').append(conditions.get(0)).append(')');
        for (int i = 1; i < conditions.size(); i++) {
            expression.append(" or (").append(conditions.get(i)).append(')');
        }

        if (excludeMode) {
            return Optional.of("not (" + expression + ")");
        }
        return Optional.of(expression.toString());
    }
}
