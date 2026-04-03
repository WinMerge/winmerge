package org.winmerge.desktop.ui.dialogs;

import java.time.LocalDate;
import java.util.List;
import java.util.Optional;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

class FilterConditionDialogControllerTest {
    @Test
    void operatorsForNumericFieldIncludeBetween() {
        List<FilterConditionDialogLogic.OperatorOption> operators =
            FilterConditionDialogLogic.operatorsForField("Size", "Size");

        assertTrue(operators.stream().anyMatch(option -> "isWithin(%1, %2, %3)".equals(option.expressionTemplate())));
    }

    @Test
    void buildResultRequiresSecondValueForBetweenOperator() {
        FilterConditionRequest request = new FilterConditionRequest(
            "Size",
            "Size",
            "isWithin(%1, %2, %3)",
            "10",
            "",
            false
        );

        Optional<FilterConditionResult> result = FilterConditionDialogLogic.buildResult(
            request,
            new FilterConditionDialogLogic.OperatorOption("IDS_FILTER_OP_BETWEEN", "isWithin(%1, %2, %3)"),
            "10",
            "",
            null,
            null,
            false
        );

        assertTrue(result.isEmpty());
    }

    @Test
    void buildDateResultFormatsIsoDates() {
        FilterConditionRequest request = new FilterConditionRequest(
            "DateStr",
            "toDateStr(%1)",
            "isWithin(%1, %2, %3)",
            "",
            "",
            false
        );

        FilterConditionResult result = FilterConditionDialogLogic.buildResult(
            request,
            new FilterConditionDialogLogic.OperatorOption("IDS_FILTER_OP_BETWEEN", "isWithin(%1, %2, %3)"),
            "",
            "",
            LocalDate.of(2026, 1, 1),
            LocalDate.of(2026, 1, 31),
            false
        ).orElseThrow();

        assertEquals("isWithin(toDateStr(%1), \"2026-01-01\", \"2026-01-31\")", result.expression());
    }

    @Test
    void buildStringResultAddsCaseSensitivePrefix() {
        FilterConditionRequest request = new FilterConditionRequest(
            "Name",
            "Name",
            "%1 contains %2",
            "temp",
            "",
            true
        );

        FilterConditionResult result = FilterConditionDialogLogic.buildResult(
            request,
            new FilterConditionDialogLogic.OperatorOption("IDS_FILTER_OP_CONTAINS", "%1 contains %2"),
            "temp",
            "",
            null,
            null,
            true
        ).orElseThrow();

        assertEquals("@cs Name contains \"temp\"", result.expression());
    }
}
