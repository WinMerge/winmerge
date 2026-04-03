package org.winmerge.desktop.ui.dialogs;

import java.io.IOException;
import java.util.Objects;
import java.util.ResourceBundle;

import javafx.fxml.FXMLLoader;
import javafx.scene.Node;
import javafx.scene.control.ButtonType;
import javafx.scene.control.Dialog;
import javafx.scene.control.DialogPane;
import javafx.scene.control.RadioButton;
import javafx.scene.control.ToggleGroup;
import javafx.scene.layout.VBox;
import javafx.stage.Window;
import org.winmerge.desktop.i18n.I18n;

public final class SharedFilterDialog extends Dialog<SharedFilterDialog.FilterType> {
    public enum FilterType {
        SHARED,
        PRIVATE
    }

    public SharedFilterDialog(Window owner, FilterType initialType) {
        Objects.requireNonNull(initialType, "initialType");

        if (owner != null) {
            initOwner(owner);
        }

        FXMLLoader loader = new FXMLLoader(
            getClass().getResource("/org/winmerge/desktop/ui/dialogs/SharedFilterDialogPane.fxml"),
            ResourceBundle.getBundle("i18n.WinMerge")
        );
        final VBox content;
        try {
            content = loader.load();
        } catch (IOException ioException) {
            throw new IllegalStateException("Unable to load shared-filter dialog FXML", ioException);
        }

        RadioButton sharedRadio = lookupRequiredRadio(content, "#sharedRadio");
        RadioButton privateRadio = lookupRequiredRadio(content, "#privateRadio");

        ToggleGroup group = new ToggleGroup();
        sharedRadio.setToggleGroup(group);
        privateRadio.setToggleGroup(group);
        sharedRadio.setSelected(initialType == FilterType.SHARED);
        privateRadio.setSelected(initialType == FilterType.PRIVATE);

        DialogPane dialogPane = new DialogPane();
        dialogPane.setContent(content);
        dialogPane.getButtonTypes().setAll(ButtonType.OK, ButtonType.CANCEL);

        setDialogPane(dialogPane);
        setTitle(I18n.tr("IDS_FILTER_TITLE"));
        setResultConverter(buttonType -> buttonType == ButtonType.OK ? resolveFilterType(sharedRadio.isSelected()) : null);
    }

    static FilterType resolveFilterType(boolean sharedSelected) {
        return sharedSelected ? FilterType.SHARED : FilterType.PRIVATE;
    }

    private static RadioButton lookupRequiredRadio(VBox root, String selector) {
        Node node = root.lookup(selector);
        if (!(node instanceof RadioButton radioButton)) {
            throw new IllegalStateException("Missing radio button: " + selector);
        }
        return radioButton;
    }
}
