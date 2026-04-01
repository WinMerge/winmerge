package org.winmerge.desktop.ui.options;

import java.io.IOException;
import java.util.List;
import java.util.Objects;

import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.control.Tab;
import javafx.scene.control.TabPane;
import org.winmerge.desktop.ui.options.tabs.OptionsTabController;

public final class OptionsDialogController {
    private static final List<TabDescriptor> TABS = List.of(
        new TabDescriptor("General", "/org/winmerge/desktop/ui/options/tabs/PropGeneralTab.fxml"),
        new TabDescriptor("Editor", "/org/winmerge/desktop/ui/options/tabs/PropEditorTab.fxml"),
        new TabDescriptor("Editor Compare/Merge", "/org/winmerge/desktop/ui/options/tabs/PropEditorCompareMergeTab.fxml"),
        new TabDescriptor("Editor Syntax", "/org/winmerge/desktop/ui/options/tabs/PropEditorSyntaxTab.fxml"),
        new TabDescriptor("Compare", "/org/winmerge/desktop/ui/options/tabs/PropCompareTab.fxml"),
        new TabDescriptor("Compare Binary", "/org/winmerge/desktop/ui/options/tabs/PropCompareBinaryTab.fxml"),
        new TabDescriptor("Compare Folder", "/org/winmerge/desktop/ui/options/tabs/PropCompareFolderTab.fxml"),
        new TabDescriptor("Compare Image", "/org/winmerge/desktop/ui/options/tabs/PropCompareImageTab.fxml"),
        new TabDescriptor("Compare Table", "/org/winmerge/desktop/ui/options/tabs/PropCompareTableTab.fxml"),
        new TabDescriptor("Compare Web Page", "/org/winmerge/desktop/ui/options/tabs/PropCompareWebPageTab.fxml"),
        new TabDescriptor("Directory Colors", "/org/winmerge/desktop/ui/options/tabs/PropDirColorsTab.fxml"),
        new TabDescriptor("Merge Colors", "/org/winmerge/desktop/ui/options/tabs/PropMergeColorsTab.fxml"),
        new TabDescriptor("Marker Colors", "/org/winmerge/desktop/ui/options/tabs/PropMarkerColorsTab.fxml"),
        new TabDescriptor("Syntax Colors", "/org/winmerge/desktop/ui/options/tabs/PropSyntaxColorsTab.fxml"),
        new TabDescriptor("Text Colors", "/org/winmerge/desktop/ui/options/tabs/PropTextColorsTab.fxml"),
        new TabDescriptor("System Colors", "/org/winmerge/desktop/ui/options/tabs/PropSysColorsTab.fxml"),
        new TabDescriptor("Color Schemes", "/org/winmerge/desktop/ui/options/tabs/PropColorSchemesTab.fxml"),
        new TabDescriptor("Archive", "/org/winmerge/desktop/ui/options/tabs/PropArchiveTab.fxml"),
        new TabDescriptor("Backups", "/org/winmerge/desktop/ui/options/tabs/PropBackupsTab.fxml"),
        new TabDescriptor("Codepage", "/org/winmerge/desktop/ui/options/tabs/PropCodepageTab.fxml"),
        new TabDescriptor("Message Boxes", "/org/winmerge/desktop/ui/options/tabs/PropMessageBoxesTab.fxml"),
        new TabDescriptor("Project", "/org/winmerge/desktop/ui/options/tabs/PropProjectTab.fxml"),
        new TabDescriptor("Registry", "/org/winmerge/desktop/ui/options/tabs/PropRegistryTab.fxml"),
        new TabDescriptor("Shell", "/org/winmerge/desktop/ui/options/tabs/PropShellTab.fxml"),
        new TabDescriptor("Line Filters", "/org/winmerge/desktop/ui/options/tabs/LineFiltersTab.fxml"),
        new TabDescriptor("Substitution Filters", "/org/winmerge/desktop/ui/options/tabs/SubstitutionFiltersTab.fxml")
    );

    @FXML
    private TabPane optionsTabPane;

    @FXML
    private void initialize() {
        Objects.requireNonNull(optionsTabPane, "Missing @FXML injection for optionsTabPane");
    }

    public void bind(AppSettings settings) {
        Objects.requireNonNull(settings, "settings");
        optionsTabPane.getTabs().clear();
        for (TabDescriptor descriptor : TABS) {
            optionsTabPane.getTabs().add(loadTab(descriptor, settings));
        }
    }

    private Tab loadTab(TabDescriptor descriptor, AppSettings settings) {
        FXMLLoader loader = new FXMLLoader(getClass().getResource(descriptor.fxmlPath()));
        final Parent content;
        try {
            content = loader.load();
        } catch (IOException ioException) {
            throw new IllegalStateException("Unable to load options tab FXML: " + descriptor.fxmlPath(), ioException);
        }

        Object controller = loader.getController();
        if (!(controller instanceof OptionsTabController optionsTabController)) {
            throw new IllegalStateException("Tab controller does not implement OptionsTabController: " + descriptor.fxmlPath());
        }
        optionsTabController.bind(settings);

        Tab tab = new Tab(descriptor.title(), content);
        tab.setClosable(false);
        return tab;
    }

    private record TabDescriptor(String title, String fxmlPath) {
    }
}
