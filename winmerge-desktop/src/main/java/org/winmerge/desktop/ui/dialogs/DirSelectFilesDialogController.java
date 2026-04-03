package org.winmerge.desktop.ui.dialogs;

import java.util.ArrayList;
import java.util.List;
import java.util.Objects;
import java.util.Optional;

import javafx.fxml.FXML;
import javafx.scene.Node;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.layout.HBox;
import org.winmerge.core.io.PathContext;

public final class DirSelectFilesDialogController {
    @FXML
    private Label pane1Label;

    @FXML
    private Button pane1Option1;

    @FXML
    private Button pane1Option2;

    @FXML
    private Button pane1Option3;

    @FXML
    private Label pane2Label;

    @FXML
    private Button pane2Option1;

    @FXML
    private Button pane2Option2;

    @FXML
    private Button pane2Option3;

    @FXML
    private HBox pane3Container;

    @FXML
    private Label pane3Label;

    @FXML
    private Button pane3Option1;

    @FXML
    private Button pane3Option2;

    @FXML
    private Button pane3Option3;

    private final List<Integer> selectedButtons = new ArrayList<>();
    private Button[] optionButtons;
    private DirSelectFilesRequest request;

    @FXML
    private void initialize() {
        requireInjected(pane1Label, "pane1Label");
        requireInjected(pane1Option1, "pane1Option1");
        requireInjected(pane1Option2, "pane1Option2");
        requireInjected(pane1Option3, "pane1Option3");
        requireInjected(pane2Label, "pane2Label");
        requireInjected(pane2Option1, "pane2Option1");
        requireInjected(pane2Option2, "pane2Option2");
        requireInjected(pane2Option3, "pane2Option3");
        requireInjected(pane3Container, "pane3Container");
        requireInjected(pane3Label, "pane3Label");
        requireInjected(pane3Option1, "pane3Option1");
        requireInjected(pane3Option2, "pane3Option2");
        requireInjected(pane3Option3, "pane3Option3");

        optionButtons = new Button[] {
            pane1Option1,
            pane1Option2,
            pane1Option3,
            pane2Option1,
            pane2Option2,
            pane2Option3,
            pane3Option1,
            pane3Option2,
            pane3Option3
        };

        for (int i = 0; i < optionButtons.length; i++) {
            optionButtons[i].setUserData(i);
        }
    }

    public void bind(DirSelectFilesRequest request) {
        this.request = Objects.requireNonNull(request, "request");

        bindPane(0, request.pane1(), pane1Label, new Button[] {pane1Option1, pane1Option2, pane1Option3});
        bindPane(1, request.pane2(), pane2Label, new Button[] {pane2Option1, pane2Option2, pane2Option3});

        // Critical parity guard: pane 3 is nullable for 2-pane comparisons.
        if (request.pane3() == null) {
            pane3Container.setVisible(false);
            pane3Container.setManaged(false);
            for (Button button : new Button[] {pane3Option1, pane3Option2, pane3Option3}) {
                button.setVisible(false);
                button.setManaged(false);
                button.setDisable(true);
            }
        } else {
            pane3Container.setVisible(true);
            pane3Container.setManaged(true);
            bindPane(2, request.pane3(), pane3Label, new Button[] {pane3Option1, pane3Option2, pane3Option3});
        }

        selectedButtons.clear();
        for (Integer buttonId : request.initialSelectedButtons()) {
            if (buttonId == null || buttonId < 0 || buttonId >= optionButtons.length) {
                continue;
            }
            if (request.pane3() == null && buttonId >= 6) {
                continue;
            }
            selectedButtons.add(buttonId);
            if (selectedButtons.size() == 3) {
                break;
            }
        }
        updateButtonCaptions();
    }

    public Optional<DirSelectFilesResult> buildResult() {
        if (request == null) {
            return Optional.empty();
        }

        List<String> selectedPaths = new ArrayList<>();
        for (Integer buttonId : selectedButtons) {
            String path = resolvePath(buttonId);
            if (path != null && !path.isBlank()) {
                selectedPaths.add(path);
            }
        }

        return Optional.of(new DirSelectFilesResult(new PathContext(selectedPaths), selectedButtons));
    }

    @FXML
    private void onChoiceButton(javafx.event.ActionEvent event) {
        Object source = event.getSource();
        if (!(source instanceof Button button)) {
            return;
        }
        Object userData = button.getUserData();
        if (!(userData instanceof Integer index)) {
            return;
        }

        if (selectedButtons.contains(index)) {
            selectedButtons.remove(index);
        } else {
            selectedButtons.add(index);
            if (selectedButtons.size() > 3) {
                selectedButtons.remove(0);
            }
        }
        updateButtonCaptions();
    }

    @FXML
    private void onReset() {
        selectedButtons.clear();
        updateButtonCaptions();
    }

    private void bindPane(int paneIndex, DirSelectFilesRequest.DirSelectFilesPane pane, Label label, Button[] buttons) {
        label.setText(pane.label());
        String[] paths = pane.filePaths();
        boolean[] available = pane.available();

        for (int optionIndex = 0; optionIndex < buttons.length; optionIndex++) {
            Button button = buttons[optionIndex];
            boolean exists = available[optionIndex];
            button.setVisible(exists);
            button.setManaged(exists);
            button.setText("");
            button.setAccessibleText(paths[optionIndex]);
            button.setUserData(paneIndex * 3 + optionIndex);
        }
    }

    private String resolvePath(int buttonId) {
        return DirSelectFilesDialogLogic.resolvePath(request, buttonId);
    }

    private void updateButtonCaptions() {
        for (Button button : optionButtons) {
            if (button.isVisible()) {
                button.setText("");
            }
        }
        int counter = 1;
        for (Integer buttonId : selectedButtons) {
            if (buttonId == null || buttonId < 0 || buttonId >= optionButtons.length) {
                continue;
            }
            Node button = optionButtons[buttonId];
            if (button.isVisible()) {
                ((Button) button).setText(Integer.toString(counter));
                counter++;
            }
        }
    }

    private static void requireInjected(Object field, String fieldName) {
        Objects.requireNonNull(field, () -> "Missing @FXML injection for " + fieldName);
    }
}
