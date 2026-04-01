package org.winmerge.desktop.ui.merge;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Objects;
import java.util.function.Consumer;

import javafx.concurrent.Task;
import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.ProgressIndicator;
import javafx.scene.layout.StackPane;

public class MergeEditController implements AutoCloseable {
    private static final long LARGE_FILE_BYTES = 5L * 1024L * 1024L;

    @FXML
    private StackPane leftEditorContainer;

    @FXML
    private StackPane rightEditorContainer;

    @FXML
    private Button previousDiffButton;

    @FXML
    private Button nextDiffButton;

    @FXML
    private Button mergeLeftToRightButton;

    @FXML
    private Label diffCounterLabel;

    @FXML
    private Label leftPathLabel;

    @FXML
    private Label rightPathLabel;

    @FXML
    private StackPane loadingOverlay;

    @FXML
    private ProgressIndicator loadingIndicator;

    @FXML
    private LocationController locationPaneViewController;

    private final TextMateGrammarParser grammarParser = new TextMateGrammarParser();
    private final MergeLoadExecutor loadExecutor = new MergeLoadExecutor("merge-file-loader");

    private final SyntaxCodeArea leftEditor = new SyntaxCodeArea(grammarParser);
    private final SyntaxCodeArea rightEditor = new SyntaxCodeArea(grammarParser);
    private Consumer<String> statusListener = message -> { };

    private MergeDocModel model;
    private int currentDiffIndex = -1;
    private volatile Task<MergeDocModel> activeLoadTask;
    private volatile boolean disposed;

    @FXML
    private void initialize() {
        requireInjected(leftEditorContainer, "leftEditorContainer");
        requireInjected(rightEditorContainer, "rightEditorContainer");
        requireInjected(previousDiffButton, "previousDiffButton");
        requireInjected(nextDiffButton, "nextDiffButton");
        requireInjected(mergeLeftToRightButton, "mergeLeftToRightButton");
        requireInjected(diffCounterLabel, "diffCounterLabel");
        requireInjected(leftPathLabel, "leftPathLabel");
        requireInjected(rightPathLabel, "rightPathLabel");
        requireInjected(loadingOverlay, "loadingOverlay");
        requireInjected(loadingIndicator, "loadingIndicator");
        requireInjected(locationPaneViewController, "locationPaneViewController");

        leftEditorContainer.getChildren().setAll(leftEditor);
        rightEditorContainer.getChildren().setAll(rightEditor);
        locationPaneViewController.setDiffSelectionListener(this::goToDiff);
        updateControls();
    }

    public void setStatusListener(Consumer<String> statusListener) {
        this.statusListener = Objects.requireNonNull(statusListener, "statusListener");
    }

    public boolean isNavigationReady() {
        return model != null;
    }

    public int fileCount() {
        return 2;
    }

    public int lineCountForFile(int fileIndex) {
        if (model == null) {
            return 0;
        }
        return fileIndex == 0 ? model.leftLineCount() : model.rightLineCount();
    }

    public int diffCount() {
        return model == null ? 0 : model.diffChunks().size();
    }

    public void navigateToLine(int fileIndex, int oneBasedLine) {
        if (model == null || oneBasedLine <= 0) {
            return;
        }
        int lineIndex = oneBasedLine - 1;
        if (fileIndex == 0) {
            leftEditor.focusLine(lineIndex);
            statusListener.accept("Navigated to left line " + oneBasedLine + ".");
            return;
        }
        rightEditor.focusLine(lineIndex);
        statusListener.accept("Navigated to right line " + oneBasedLine + ".");
    }

    public void navigateToDiff(int oneBasedDiff) {
        if (oneBasedDiff <= 0) {
            return;
        }
        goToDiff(oneBasedDiff - 1);
    }

    public void loadFiles(Path leftPath, Path rightPath) {
        Objects.requireNonNull(leftPath, "leftPath");
        Objects.requireNonNull(rightPath, "rightPath");
        if (disposed) {
            statusListener.accept("Comparison tab is closing; load skipped.");
            return;
        }

        cancelActiveLoad();

        leftPathLabel.setText(leftPath.toString());
        rightPathLabel.setText(rightPath.toString());

        boolean showSpinner = isLargeFile(leftPath) || isLargeFile(rightPath);
        setLoading(showSpinner);
        statusListener.accept("Loading files...");

        Task<MergeDocModel> loadTask = new Task<>() {
            @Override
            protected MergeDocModel call() throws Exception {
                return MergeDocModel.load(leftPath, rightPath);
            }
        };
        activeLoadTask = loadTask;

        loadTask.setOnSucceeded(event -> {
            activeLoadTask = null;
            if (disposed || loadTask.isCancelled()) {
                return;
            }
            model = loadTask.getValue();
            currentDiffIndex = model.hasDiffs() ? 0 : -1;
            refreshView();
            setLoading(false);
            if (model.usedGuardedDiffFallback()) {
                statusListener.accept(
                    "Loaded diff with fallback range due to file size guard (" + model.diffChunks().size() + " block)."
                );
            } else {
                statusListener.accept("Loaded diff: " + model.diffChunks().size() + " change block(s).");
            }
        });

        loadTask.setOnFailed(event -> {
            activeLoadTask = null;
            if (disposed || loadTask.isCancelled()) {
                return;
            }
            Throwable failure = loadTask.getException();
            setLoading(false);
            statusListener.accept("Failed to load files: " + failure.getMessage());
        });

        loadTask.setOnCancelled(event -> {
            activeLoadTask = null;
            if (disposed) {
                return;
            }
            setLoading(false);
            statusListener.accept("File load cancelled.");
        });

        loadExecutor.submit(loadTask);
    }

    @FXML
    private void onPreviousDiff() {
        if (model == null || !model.hasDiffs()) {
            return;
        }
        goToDiff(model.previousDiffIndex(currentDiffIndex));
    }

    @FXML
    private void onNextDiff() {
        if (model == null || !model.hasDiffs()) {
            return;
        }
        goToDiff(model.nextDiffIndex(currentDiffIndex));
    }

    @FXML
    private void onMergeLeftToRight() {
        if (model == null || !model.hasDiffs() || currentDiffIndex < 0) {
            return;
        }
        model = model.mergeLeftChunkToRight(currentDiffIndex);
        if (model.hasDiffs()) {
            currentDiffIndex = Math.min(currentDiffIndex, model.diffChunks().size() - 1);
        } else {
            currentDiffIndex = -1;
        }
        refreshView();
        statusListener.accept("Merged current block from left to right.");
    }

    private void refreshView() {
        if (model == null) {
            updateControls();
            return;
        }

        leftEditor.setDocument(model.leftPath(), model.leftText());
        rightEditor.setDocument(model.rightPath(), model.rightText());
        leftEditor.applyDiffStyles(model.diffChunks(), true, currentDiffIndex);
        rightEditor.applyDiffStyles(model.diffChunks(), false, currentDiffIndex);

        locationPaneViewController.setDiffs(
            model.diffChunks(),
            model.leftLineCount(),
            model.rightLineCount()
        );
        locationPaneViewController.setActiveDiffIndex(currentDiffIndex);

        if (currentDiffIndex >= 0 && currentDiffIndex < model.diffChunks().size()) {
            DiffChunk chunk = model.diffChunks().get(currentDiffIndex);
            leftEditor.focusDiff(chunk, true);
            rightEditor.focusDiff(chunk, false);
        }
        updateControls();
    }

    private void goToDiff(int index) {
        if (model == null || !model.hasDiffs() || index < 0 || index >= model.diffChunks().size()) {
            return;
        }
        currentDiffIndex = index;
        refreshView();
        statusListener.accept(
            "Navigated to change block " + (currentDiffIndex + 1) + " of " + model.diffChunks().size() + "."
        );
    }

    private void setLoading(boolean loading) {
        loadingOverlay.setVisible(loading);
        loadingOverlay.setManaged(loading);
        loadingIndicator.setVisible(loading);
        loadingIndicator.setManaged(loading);
    }

    private void updateControls() {
        boolean hasDiffs = model != null && model.hasDiffs();
        previousDiffButton.setDisable(!hasDiffs);
        nextDiffButton.setDisable(!hasDiffs);
        mergeLeftToRightButton.setDisable(!hasDiffs || currentDiffIndex < 0);

        if (!hasDiffs) {
            diffCounterLabel.setText("No differences");
            locationPaneViewController.setActiveDiffIndex(-1);
            return;
        }
        diffCounterLabel.setText("Diff " + (currentDiffIndex + 1) + " / " + model.diffChunks().size());
    }

    private static boolean isLargeFile(Path path) {
        try {
            return Files.size(path) > LARGE_FILE_BYTES;
        } catch (IOException ignored) {
            return false;
        }
    }

    private static void requireInjected(Object field, String fieldName) {
        Objects.requireNonNull(field, () -> "Missing @FXML injection for " + fieldName);
    }

    public void dispose() {
        if (disposed) {
            return;
        }
        disposed = true;
        cancelActiveLoad();
        loadExecutor.close();
    }

    @Override
    public void close() {
        dispose();
    }

    private void cancelActiveLoad() {
        Task<MergeDocModel> task = activeLoadTask;
        if (task != null) {
            task.cancel(true);
        }
        loadExecutor.cancelActive();
        activeLoadTask = null;
    }
}
