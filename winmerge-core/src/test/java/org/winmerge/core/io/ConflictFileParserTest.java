package org.winmerge.core.io;

import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.io.TempDir;

import java.nio.file.Files;
import java.nio.file.Path;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

class ConflictFileParserTest {
    @TempDir
    Path tempDir;

    @Test
    void detectsConflictMarkerPresence() throws Exception {
        Path conflict = tempDir.resolve("conflict.txt");
        Path plain = tempDir.resolve("plain.txt");
        Files.writeString(conflict, "<<<<<<< mine\nabc\n=======\ndef\n>>>>>>> theirs\n");
        Files.writeString(plain, "no conflict marker\n");

        assertTrue(ConflictFileParser.isConflictFile(conflict));
        assertFalse(ConflictFileParser.isConflictFile(plain));
    }

    @Test
    void parsesTwoWayConflictIntoWorkingAndNewRevision() throws Exception {
        Path conflict = tempDir.resolve("two-way.txt");
        Files.writeString(conflict, """
                common
                <<<<<<< mine
                left-value
                =======
                right-value
                >>>>>>> revision-2
                tail
                """);

        Path working = tempDir.resolve("working.txt");
        Path theirs = tempDir.resolve("theirs.txt");
        Path base = tempDir.resolve("base.txt");

        ConflictFileParser.ParseResult result = ConflictFileParser.parseConflictFile(conflict, working, theirs, base, 0);

        assertTrue(result.parsed());
        assertFalse(result.threeWay());
        assertFalse(result.nestedConflicts());
        assertEquals("revision-2", result.revision());
        assertEquals("common\nleft-value\ntail\n", Files.readString(working));
        assertEquals("common\nright-value\ntail\n", Files.readString(theirs));
        assertEquals("common\ntail\n", Files.readString(base));
    }

    @Test
    void parsesThreeWayConflictWithBaseSection() throws Exception {
        Path conflict = tempDir.resolve("three-way.txt");
        Files.writeString(conflict, """
                one
                <<<<<<< mine
                mine-line
                ||||||| base
                base-line
                =======
                theirs-line
                >>>>>>> revision-3
                two
                """);

        Path working = tempDir.resolve("working-3.txt");
        Path theirs = tempDir.resolve("theirs-3.txt");
        Path base = tempDir.resolve("base-3.txt");

        ConflictFileParser.ParseResult result = ConflictFileParser.parseConflictFile(conflict, working, theirs, base, 0);

        assertTrue(result.parsed());
        assertTrue(result.threeWay());
        assertEquals("one\nmine-line\ntwo\n", Files.readString(working));
        assertEquals("one\ntheirs-line\ntwo\n", Files.readString(theirs));
        assertEquals("one\nbase-line\ntwo\n", Files.readString(base));
    }

    @Test
    void flagsNestedConflicts() throws Exception {
        Path conflict = tempDir.resolve("nested.txt");
        Files.writeString(conflict, """
                <<<<<<< mine
                nested-start
                <<<<<<< nested
                nested-body
                >>>>>>> nested
                =======
                theirs
                >>>>>>> outer
                """);

        Path working = tempDir.resolve("working-nested.txt");
        Path theirs = tempDir.resolve("theirs-nested.txt");
        Path base = tempDir.resolve("base-nested.txt");

        ConflictFileParser.ParseResult result = ConflictFileParser.parseConflictFile(conflict, working, theirs, base, 0);

        assertTrue(result.parsed());
        assertTrue(result.nestedConflicts());
        assertEquals("""
                nested-start
                <<<<<<< nested
                nested-body
                >>>>>>> nested
                """, Files.readString(working));
        assertEquals("theirs\n", Files.readString(theirs));
        assertEquals("", Files.readString(base));
    }
}
