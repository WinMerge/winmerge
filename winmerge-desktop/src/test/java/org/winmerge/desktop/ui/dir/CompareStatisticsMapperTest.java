package org.winmerge.desktop.ui.dir;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;

import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.io.TempDir;
import org.winmerge.desktop.ui.dialogs.CompareStatisticsDialogModel;

import static org.junit.jupiter.api.Assertions.assertEquals;

class CompareStatisticsMapperTest {
    @TempDir
    Path tempDir;

    @Test
    void mapsDirModelStatusCountsToDialogModel() throws IOException {
        Path left = Files.createDirectory(tempDir.resolve("left"));
        Path right = Files.createDirectory(tempDir.resolve("right"));

        writeFile(left.resolve("only-left.txt"), "left");
        writeFile(right.resolve("only-right.txt"), "right");
        writeFile(left.resolve("different.txt"), "left-value");
        writeFile(right.resolve("different.txt"), "right-value");
        writeFile(left.resolve("same.txt"), "shared");
        writeFile(right.resolve("same.txt"), "shared");

        DirDocModel model = DirDocModel.load(left, right, "*.*");
        CompareStatisticsDialogModel stats = CompareStatisticsMapper.fromModel(model);

        assertEquals(left.toString(), stats.leftRootPath());
        assertEquals(right.toString(), stats.rightRootPath());
        assertEquals(1, stats.onlyLeftFiles());
        assertEquals(1, stats.onlyRightFiles());
        assertEquals(1, stats.differentFiles());
        assertEquals(1, stats.identicalFiles());
        assertEquals(4, stats.totalFiles());
    }

    private static void writeFile(Path path, String content) throws IOException {
        if (path.getParent() != null) {
            Files.createDirectories(path.getParent());
        }
        Files.writeString(path, content);
    }
}
