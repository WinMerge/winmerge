package org.winmerge.desktop.ui.merge;

import java.nio.file.Path;
import java.util.List;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

class MergeDocModelTest {
    @Test
    void computesDiffChunksForLineReplacement() {
        MergeDocModel model = MergeDocModel.fromTexts(
            Path.of("left.java"),
            Path.of("right.java"),
            "a\nb\nc\n",
            "a\nx\nc\n"
        );

        assertTrue(model.hasDiffs());
        assertEquals(List.of(new DiffChunk(1, 2, 1, 2)), model.diffChunks());
    }

    @Test
    void supportsMergeLeftChunkToRight() {
        MergeDocModel model = MergeDocModel.fromTexts(
            Path.of("left.java"),
            Path.of("right.java"),
            "a\nleft-only\nc\n",
            "a\nright-only\nc\n"
        );

        MergeDocModel merged = model.mergeLeftChunkToRight(0);
        assertFalse(merged.hasDiffs());
        assertEquals(model.leftText(), merged.rightText());
    }

    @Test
    void navigatesDiffIndicesWithWraparound() {
        MergeDocModel model = MergeDocModel.fromTexts(
            Path.of("left.java"),
            Path.of("right.java"),
            "a\nb\nc\nd\n",
            "x\nb\ny\nd\n"
        );

        assertEquals(0, model.nextDiffIndex(-1));
        assertEquals(1, model.nextDiffIndex(0));
        assertEquals(0, model.nextDiffIndex(1));
        assertEquals(1, model.previousDiffIndex(0));
        assertEquals(0, model.previousDiffIndex(1));
    }
}
