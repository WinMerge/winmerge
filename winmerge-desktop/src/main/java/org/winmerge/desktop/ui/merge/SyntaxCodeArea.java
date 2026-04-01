package org.winmerge.desktop.ui.merge;

import java.nio.file.Path;
import java.util.Collections;
import java.util.List;
import java.util.Objects;

import javafx.scene.layout.BorderPane;
import org.fxmisc.flowless.VirtualizedScrollPane;
import org.fxmisc.richtext.CodeArea;
import org.fxmisc.richtext.LineNumberFactory;

public final class SyntaxCodeArea extends BorderPane {
    private static final List<String> DIFF_CLASS = List.of("diff-line");
    private static final List<String> ACTIVE_DIFF_CLASS = List.of("diff-line-active");

    private final CodeArea codeArea;
    private final TextMateGrammarParser grammarParser;
    private Path sourcePath;

    public SyntaxCodeArea(TextMateGrammarParser grammarParser) {
        this.grammarParser = Objects.requireNonNull(grammarParser, "grammarParser");
        this.codeArea = new CodeArea();
        this.codeArea.getStyleClass().add("merge-code-area");
        this.codeArea.setParagraphGraphicFactory(LineNumberFactory.get(this.codeArea));
        this.setCenter(new VirtualizedScrollPane<>(this.codeArea));
    }

    public void setDocument(Path sourcePath, String text) {
        this.sourcePath = Objects.requireNonNull(sourcePath, "sourcePath");
        codeArea.replaceText(Objects.requireNonNull(text, "text"));
        codeArea.setStyleSpans(0, grammarParser.computeStyleSpans(sourcePath, text));
    }

    public void applyDiffStyles(List<DiffChunk> chunks, boolean isLeftSide, int activeDiffIndex) {
        clearParagraphStyles();
        for (int i = 0; i < chunks.size(); i++) {
            DiffChunk chunk = chunks.get(i);
            int startLine = isLeftSide ? chunk.leftStartLine() : chunk.rightStartLine();
            int endLine = isLeftSide ? chunk.leftEndLine() : chunk.rightEndLine();
            if (startLine == endLine) {
                continue;
            }
            List<String> style = i == activeDiffIndex ? ACTIVE_DIFF_CLASS : DIFF_CLASS;
            for (int line = startLine; line < endLine; line++) {
                if (line >= 0 && line < codeArea.getParagraphs().size()) {
                    codeArea.setParagraphStyle(line, style);
                }
            }
        }
    }

    public void focusDiff(DiffChunk chunk, boolean isLeftSide) {
        int line = isLeftSide ? chunk.leftStartLine() : chunk.rightStartLine();
        if (line >= 0 && line < codeArea.getParagraphs().size()) {
            codeArea.showParagraphAtTop(line);
            codeArea.moveTo(line, 0);
            codeArea.requestFocus();
        }
    }

    public Path sourcePath() {
        return sourcePath;
    }

    private void clearParagraphStyles() {
        for (int i = 0; i < codeArea.getParagraphs().size(); i++) {
            codeArea.setParagraphStyle(i, Collections.emptyList());
        }
    }
}
