package org.winmerge.desktop.ui.dialogs;

import java.io.IOException;
import java.util.Objects;
import java.util.ResourceBundle;

import javafx.fxml.FXMLLoader;
import javafx.scene.control.ButtonType;
import javafx.scene.control.Dialog;
import javafx.scene.control.DialogPane;
import javafx.scene.layout.VBox;
import javafx.stage.Window;
import org.winmerge.desktop.i18n.I18n;

public final class TestFilterDialog extends Dialog<ButtonType> {
    public TestFilterDialog(Window owner, TestFilterModel model) {
        Objects.requireNonNull(model, "model");

        if (owner != null) {
            initOwner(owner);
        }

        FXMLLoader loader = new FXMLLoader(
            getClass().getResource("/org/winmerge/desktop/ui/dialogs/TestFilterDialogPane.fxml"),
            ResourceBundle.getBundle("i18n.WinMerge")
        );
        final VBox content;
        try {
            content = loader.load();
        } catch (IOException ioException) {
            throw new IllegalStateException("Unable to load test-filter dialog FXML", ioException);
        }

        TestFilterDialogController controller = loader.getController();
        controller.bind(model);

        DialogPane dialogPane = new DialogPane();
        dialogPane.setContent(content);
        dialogPane.getButtonTypes().setAll(ButtonType.CLOSE);

        setDialogPane(dialogPane);
        setTitle(I18n.tr("IDS_FILTER_TITLE"));
    }
}
