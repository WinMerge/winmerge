package org.winmerge.desktop.ui.dir;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Map;
import java.util.function.Function;
import java.util.stream.Collectors;

import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.io.TempDir;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

class DirDocModelTest {
    @TempDir
    Path tempDir;

    @Test
    void classifiesRowsAcrossOnlyLeftOnlyRightDifferentAndIdentical() throws IOException {
        Path left = Files.createDirectory(tempDir.resolve("left"));
        Path right = Files.createDirectory(tempDir.resolve("right"));

        writeFile(left.resolve("only-left.txt"), "left");
        writeFile(right.resolve("only-right.txt"), "right");
        writeFile(left.resolve("same.txt"), "shared");
        writeFile(right.resolve("same.txt"), "shared");
        writeFile(left.resolve("different.txt"), "left-value");
        writeFile(right.resolve("different.txt"), "right-value");

        DirDocModel model = DirDocModel.load(left, right, "*.*");
        Map<String, DirDocModel.DirItem> byPath = model.items().stream()
            .collect(Collectors.toMap(DirDocModel.DirItem::relativePath, Function.identity()));

        assertEquals(4, model.items().size());
        assertEquals(DirDocModel.DirStatus.ONLY_LEFT, byPath.get("only-left.txt").status());
        assertEquals(DirDocModel.DirStatus.ONLY_RIGHT, byPath.get("only-right.txt").status());
        assertEquals(DirDocModel.DirStatus.IDENTICAL, byPath.get("same.txt").status());
        assertEquals(DirDocModel.DirStatus.DIFFERENT, byPath.get("different.txt").status());
        assertTrue(byPath.get("same.txt").isComparableFile());
        assertFalse(byPath.get("only-left.txt").isComparableFile());
    }

    @Test
    void appliesFilePatternFilterBeforeComparison() throws IOException {
        Path left = Files.createDirectory(tempDir.resolve("left-filter"));
        Path right = Files.createDirectory(tempDir.resolve("right-filter"));

        writeFile(left.resolve("keep.txt"), "same");
        writeFile(right.resolve("keep.txt"), "same");
        writeFile(left.resolve("skip.bin"), "left");
        writeFile(right.resolve("skip.bin"), "right");

        DirDocModel model = DirDocModel.load(left, right, "*.txt");

        assertEquals(1, model.items().size());
        assertEquals("keep.txt", model.items().get(0).relativePath());
        assertEquals(DirDocModel.DirStatus.IDENTICAL, model.items().get(0).status());
    }

    private static void writeFile(Path path, String content) throws IOException {
        if (path.getParent() != null) {
            Files.createDirectories(path.getParent());
        }
        Files.writeString(path, content);
    }
}
