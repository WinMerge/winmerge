package org.winmerge.desktop.ui;

import java.io.IOException;
import java.lang.reflect.Field;
import java.util.List;
import java.util.concurrent.atomic.AtomicBoolean;

import javafx.fxml.FXMLLoader;
import javafx.scene.control.ButtonBase;
import javafx.scene.control.ButtonType;
import javafx.scene.control.MenuBar;
import javafx.scene.control.MenuItem;
import javafx.scene.control.Tab;
import javafx.scene.control.TabPane;
import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Test;
import org.winmerge.desktop.ui.options.AppSettings;
import org.winmerge.desktop.ui.options.OptionsDialog;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertTrue;

class OptionsDialogFxIntegrationTest {
    @BeforeAll
    static void setupFxToolkit() {
        FxThreadTestSupport.initializeToolkit();
    }

    @Test
    void toolsOptionsMenuItemDispatchesAction() {
        AtomicBoolean invoked = new AtomicBoolean(false);

        FxThreadTestSupport.runOnFxThread(() -> {
            try {
                FXMLLoader loader = new FXMLLoader(
                    OptionsDialogFxIntegrationTest.class.getResource("/org/winmerge/desktop/ui/MenuBar.fxml")
                );
                MenuBar ignored = loader.load();
                MenuBarController controller = loader.getController();

                ActionDispatcher dispatcher = new ActionDispatcher();
                dispatcher.register(ActionId.TOOLS_OPTIONS, () -> invoked.set(true));
                dispatcher.register(ActionId.FILE_OPEN, () -> {
                });
                dispatcher.register(ActionId.FILE_EXIT, () -> {
                });
                dispatcher.register(ActionId.EDIT_GO_TO, () -> {
                });
                dispatcher.register(ActionId.HELP_ABOUT, () -> {
                });
                controller.bind(dispatcher);

                MenuItem toolsOptionsMenuItem = (MenuItem) readField(controller, "toolsOptionsMenuItem");
                toolsOptionsMenuItem.fire();
            } catch (IOException exception) {
                throw new RuntimeException(exception);
            }
        });

        assertTrue(invoked.get());
    }

    @Test
    void optionsDialogLoadsAllTabsAndApplyButtonCallback() {
        AtomicBoolean applyInvoked = new AtomicBoolean(false);

        FxThreadTestSupport.runOnFxThread(() -> {
            AppSettings settings = new AppSettings();
            OptionsDialog dialog = new OptionsDialog(null, settings, () -> applyInvoked.set(true));
            TabPane tabPane = (TabPane) dialog.getDialogPane().getContent();
            assertNotNull(tabPane);

            List<Tab> tabs = tabPane.getTabs();
            assertEquals(26, tabs.size());
            assertTrue(tabs.stream().allMatch(tab -> tab.getContent() != null));

            ButtonType applyType = dialog.getDialogPane().getButtonTypes().stream()
                .filter(buttonType -> "Apply".equals(buttonType.getText()))
                .findFirst()
                .orElseThrow();
            ButtonBase applyButton = (ButtonBase) dialog.getDialogPane().lookupButton(applyType);
            applyButton.fire();
        });

        assertTrue(applyInvoked.get());
    }

    private static Object readField(Object target, String fieldName) {
        try {
            Field field = target.getClass().getDeclaredField(fieldName);
            field.setAccessible(true);
            return field.get(target);
        } catch (ReflectiveOperationException exception) {
            throw new RuntimeException(exception);
        }
    }
}
