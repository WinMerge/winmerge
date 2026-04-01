package org.winmerge.core.filter;

import org.junit.jupiter.api.Test;

import java.util.regex.PatternSyntaxException;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

class FilterAndSubstitutionTest {
    @Test
    void matchesAndClonesFilterLists() {
        FilterList filters = new FilterList();
        filters.addRegExp("foo.*bar");

        assertTrue(filters.match("foo-123-bar"));

        FilterList clone = new FilterList();
        clone.cloneFrom(filters);
        assertTrue(clone.match("foo-xyz-bar"));
    }

    @Test
    void optionallyThrowsForInvalidRegex() {
        FilterList filters = new FilterList();
        assertThrows(PatternSyntaxException.class, () -> filters.addRegExp("(", true));
    }

    @Test
    void appliesSubstitutionsWithEscapes() {
        SubstitutionList substitutions = new SubstitutionList();
        substitutions.add("foo", "bar", true, false);
        substitutions.add("bar", "line\\nend", true, false);

        assertEquals("line\nend", substitutions.subst("foo"));
    }
}
