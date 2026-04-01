package org.winmerge.desktop.ui.dialogs;

import java.util.Optional;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

class ComparisonResultFilterExpressionBuilderTest {
    @Test
    void returnsEmptyWhenNoConditionSelected() {
        ComparisonResultFilterSelection selection = new ComparisonResultFilterSelection(
            true,
            false,
            false,
            false,
            false,
            false,
            false,
            false,
            false,
            false,
            false,
            false,
            false,
            false
        );

        Optional<String> expression = ComparisonResultFilterExpressionBuilder.build(selection);
        assertTrue(expression.isEmpty());
    }

    @Test
    void buildsTwoWayIncludeExpression() {
        ComparisonResultFilterSelection selection = new ComparisonResultFilterSelection(
            true,
            false,
            true,
            false,
            true,
            true,
            false,
            false,
            false,
            false,
            false,
            false,
            false,
            false
        );

        String expression = ComparisonResultFilterExpressionBuilder.build(selection).orElseThrow();
        assertEquals(
            "(Identical) or (LeftExists and not RightExists) or (not LeftExists and RightExists)",
            expression
        );
    }

    @Test
    void buildsTwoWayExcludeExpression() {
        ComparisonResultFilterSelection selection = new ComparisonResultFilterSelection(
            false,
            false,
            false,
            true,
            false,
            false,
            false,
            false,
            false,
            false,
            false,
            false,
            false,
            false
        );

        String expression = ComparisonResultFilterExpressionBuilder.build(selection).orElseThrow();
        assertEquals("not (Different)", expression);
    }

    @Test
    void buildsThreeWayConditions() {
        ComparisonResultFilterSelection selection = new ComparisonResultFilterSelection(
            true,
            true,
            false,
            false,
            true,
            false,
            false,
            true,
            false,
            true,
            false,
            false,
            false,
            true
        );

        String expression = ComparisonResultFilterExpressionBuilder.build(selection).orElseThrow();
        assertEquals(
            "(LeftExists and not MiddleExists and not RightExists)"
                + " or (not LeftExists and MiddleExists and not RightExists)"
                + " or (not DifferentLeftMiddle and DifferentMiddleRight and DifferentLeftRight)"
                + " or (LeftExists and MiddleExists and not RightExists)",
            expression
        );
    }
}
