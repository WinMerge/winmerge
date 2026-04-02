package org.winmerge.desktop.ui.dialogs;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertTrue;

class PatchDialogTest {
    @Test
    void rejectsInvalidSourcePathsAndContextLines() throws IOException {
        Path left = Files.createTempFile("patch-left", ".txt");
        Path right = Files.createTempFile("patch-right", ".txt");
        Path missing = left.resolveSibling("missing-source.txt");

        assertFalse(PatchDialogLogic.canSubmit(missing.toString(), right.toString(), "", "3"));
        assertFalse(PatchDialogLogic.canSubmit(left.toString(), missing.toString(), "", "3"));
        assertFalse(PatchDialogLogic.canSubmit(left.toString(), right.toString(), "", "-1"));
        assertFalse(PatchDialogLogic.canSubmit(left.toString(), right.toString(), "", "abc"));
        assertTrue(PatchDialogLogic.canSubmit(left.toString(), right.toString(), "", "3"));
    }

    @Test
    void rejectsRelativeResultPathWhenProvided() throws IOException {
        Path left = Files.createTempFile("patch-left", ".txt");
        Path right = Files.createTempFile("patch-right", ".txt");

        assertFalse(PatchDialogLogic.canSubmit(left.toString(), right.toString(), "relative.patch", "3"));
    }

    @Test
    void buildsResultForValidInputsAndGeneratesTempPathWhenMissing() throws IOException {
        Path left = Files.createTempFile("patch-left", ".txt");
        Path right = Files.createTempFile("patch-right", ".txt");

        PatchDialogResult result = PatchDialogLogic
            .buildResult(
                left.toString(),
                right.toString(),
                "",
                "HTML",
                "5",
                true,
                false,
                true,
                true
            )
            .orElseThrow();

        assertEquals(left.toString(), result.leftPath());
        assertEquals(right.toString(), result.rightPath());
        assertNotNull(result.resultPath());
        assertTrue(Path.of(result.resultPath()).isAbsolute());
        assertEquals("HTML", result.patchStyle());
        assertEquals(5, result.contextLines());
        assertTrue(result.copyToClipboard());
        assertFalse(result.appendToFile());
        assertTrue(result.openInEditor());
        assertTrue(result.includeCommandLine());
    }

    @Test
    void defaultsDisableIncludeCommandLine() {
        PatchDialogRequest request = PatchDialogRequest.defaults(null);
        assertFalse(request.includeCommandLine());
        assertEquals("Normal", request.patchStyle());
    }
}
