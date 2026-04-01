package org.winmerge.core.io;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;

class PathContextTest {
    @Test
    void normalizesAndAccessesPaths() {
        PathContext context = new PathContext();
        context.setLeft("C:/tmp/project/", true);
        context.setRight("C:/tmp/project-right/", true);

        assertEquals("C:/tmp/project", context.getLeft(true));
        assertEquals("C:/tmp/project-right", context.getRight(true));
        assertEquals("C:/tmp/project/", context.getLeft(false));
    }

    @Test
    void insertsMiddleAndShiftsExistingRight() {
        PathContext context = new PathContext("left", "right");
        context.setMiddle("middle", true);

        assertEquals(3, context.getSize());
        assertEquals("left", context.getLeft(true));
        assertEquals("middle", context.getMiddle(true));
        assertEquals("right", context.getRight(true));
    }
}
