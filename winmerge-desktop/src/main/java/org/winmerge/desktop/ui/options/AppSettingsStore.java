package org.winmerge.desktop.ui.options;

import java.util.LinkedHashMap;
import java.util.Map;
import java.util.Objects;

import org.winmerge.core.config.ConfigurationStore;

public final class AppSettingsStore {
    private static final Map<String, Boolean> BOOLEAN_DEFAULTS = new LinkedHashMap<>();
    private static final Map<String, Integer> INTEGER_DEFAULTS = new LinkedHashMap<>();
    private static final Map<String, String> STRING_DEFAULTS = new LinkedHashMap<>();

    static {
        BOOLEAN_DEFAULTS.put(AppSettings.KEY_AUTOMATIC_RESCAN, true);
        BOOLEAN_DEFAULTS.put("options.general.scrollToFirst", false);
        BOOLEAN_DEFAULTS.put("options.general.scrollToFirstInlineDiff", false);
        BOOLEAN_DEFAULTS.put("options.general.verifyOpenPaths", false);
        BOOLEAN_DEFAULTS.put("options.general.askMultiWindowClose", false);
        BOOLEAN_DEFAULTS.put("options.general.preserveFileTimes", false);
        BOOLEAN_DEFAULTS.put("options.general.showSelectFolderOnStartup", false);
        BOOLEAN_DEFAULTS.put("options.general.closeWithOk", true);
        BOOLEAN_DEFAULTS.put("options.editor.syntaxHighlight", false);
        BOOLEAN_DEFAULTS.put("options.editor.allowMixedEol", false);
        BOOLEAN_DEFAULTS.put("options.editorSyntax.enableCustomExtensions", false);
        BOOLEAN_DEFAULTS.put("options.compare.ignoreBlankLines", false);
        BOOLEAN_DEFAULTS.put("options.compare.filterCommentLines", false);
        BOOLEAN_DEFAULTS.put("options.compare.ignoreCase", false);
        BOOLEAN_DEFAULTS.put("options.compare.ignoreNumbers", false);
        BOOLEAN_DEFAULTS.put("options.compare.ignoreEol", true);
        BOOLEAN_DEFAULTS.put("options.compare.ignoreCodepage", true);
        BOOLEAN_DEFAULTS.put("options.compare.ignoreMissingTrailingEol", true);
        BOOLEAN_DEFAULTS.put("options.compare.ignoreLineBreaks", true);
        BOOLEAN_DEFAULTS.put("options.compare.movedBlocks", false);
        BOOLEAN_DEFAULTS.put("options.compare.alignSimilarLines", false);
        BOOLEAN_DEFAULTS.put("options.compare.indentHeuristic", true);
        BOOLEAN_DEFAULTS.put("options.compare.blankOutIgnoredChanges", false);
        BOOLEAN_DEFAULTS.put("options.compareMerge.viewLineDifferences", false);
        BOOLEAN_DEFAULTS.put("options.compareMerge.breakOnWords", false);
        BOOLEAN_DEFAULTS.put("options.compareFolder.stopAfterFirst", false);
        BOOLEAN_DEFAULTS.put("options.compareFolder.ignoreSmallTimeDiff", false);
        BOOLEAN_DEFAULTS.put("options.compareFolder.includeUniqueFolders", false);
        BOOLEAN_DEFAULTS.put("options.compareFolder.includeSubdirs", false);
        BOOLEAN_DEFAULTS.put("options.compareFolder.ignoreReparsePoints", false);
        BOOLEAN_DEFAULTS.put("options.compareImage.enableInDirectoryCompare", false);
        BOOLEAN_DEFAULTS.put("options.compareTable.allowNewlinesInQuotes", false);
        BOOLEAN_DEFAULTS.put("options.compareWebPage.userDataFolderPerPane", false);
        BOOLEAN_DEFAULTS.put("options.dirColors.useColors", false);
        BOOLEAN_DEFAULTS.put("options.syntaxColors.keywordBold", false);
        BOOLEAN_DEFAULTS.put("options.textColors.customColors", false);
        BOOLEAN_DEFAULTS.put("options.sysColors.enableHook", false);
        BOOLEAN_DEFAULTS.put("options.archive.enableSupport", false);
        BOOLEAN_DEFAULTS.put("options.archive.probeType", false);
        BOOLEAN_DEFAULTS.put("options.backups.createForFolderCompare", false);
        BOOLEAN_DEFAULTS.put("options.backups.createForFileCompare", false);
        BOOLEAN_DEFAULTS.put("options.backups.appendBak", false);
        BOOLEAN_DEFAULTS.put("options.backups.appendTime", false);
        BOOLEAN_DEFAULTS.put("options.codepage.detectCodepage", false);
        BOOLEAN_DEFAULTS.put("options.codepage.detectCodepage2", false);
        BOOLEAN_DEFAULTS.put("options.registry.useRecycleBin", true);
        BOOLEAN_DEFAULTS.put("options.registry.useSystemTempPath", true);
        BOOLEAN_DEFAULTS.put("options.shell.contextMenuEnabled", false);
        BOOLEAN_DEFAULTS.put("options.shell.advancedContextMenu", false);
        BOOLEAN_DEFAULTS.put("options.shell.compareAsSubmenu", false);
        BOOLEAN_DEFAULTS.put("options.lineFilters.ignoreRegExp", false);
        BOOLEAN_DEFAULTS.put("options.substitutionFilters.enabled", false);
        BOOLEAN_DEFAULTS.put("options.messageBoxes.resetOnStartup", false);
        BOOLEAN_DEFAULTS.put("options.project.includeFileFiltersOnOpen", false);
        BOOLEAN_DEFAULTS.put("options.project.includeCompareOptionsOnLoad", false);
        BOOLEAN_DEFAULTS.put("options.project.includeHiddenItemsOnSave", false);

        INTEGER_DEFAULTS.put(AppSettings.KEY_EDITOR_TAB_SIZE, 4);
        INTEGER_DEFAULTS.put("options.general.singleInstanceMode", 0);
        INTEGER_DEFAULTS.put("options.general.closeWindowWithEscMode", 1);
        INTEGER_DEFAULTS.put("options.general.autoCompleteSource", 0);
        INTEGER_DEFAULTS.put("options.general.autoReloadMode", 0);
        INTEGER_DEFAULTS.put("options.editor.tabType", 0);
        INTEGER_DEFAULTS.put("options.editor.renderingMode", 0);
        INTEGER_DEFAULTS.put("options.compare.ignoreWhitespaceMode", 0);
        INTEGER_DEFAULTS.put("options.compare.diffAlgorithm", 0);
        INTEGER_DEFAULTS.put("options.compareMerge.copyGranularity", 0);
        INTEGER_DEFAULTS.put("options.compareMerge.breakType", 0);
        INTEGER_DEFAULTS.put("options.compareFolder.compareMethod", 0);
        INTEGER_DEFAULTS.put("options.compareFolder.expandSubdirsMode", 0);
        INTEGER_DEFAULTS.put("options.compareFolder.quickCompareLimit", 4);
        INTEGER_DEFAULTS.put("options.compareFolder.binaryCompareLimit", 4);
        INTEGER_DEFAULTS.put("options.compareFolder.compareThreads", 1);
        INTEGER_DEFAULTS.put("options.compareFolder.renameMoveDetection", 0);
        INTEGER_DEFAULTS.put("options.compareFolder.renameMoveMergeMode", 0);
        INTEGER_DEFAULTS.put("options.compareImage.ocrResultType", 0);
        INTEGER_DEFAULTS.put("options.compareWebPage.userDataFolderType", 0);
        INTEGER_DEFAULTS.put("options.colorSchemes.mode", 0);
        INTEGER_DEFAULTS.put("options.backups.locationMode", 0);
        INTEGER_DEFAULTS.put("options.codepage.defaultMode", 0);
        INTEGER_DEFAULTS.put("options.codepage.defaultCustom", 65001);
        INTEGER_DEFAULTS.put("options.codepage.autodetectType", 0);
        INTEGER_DEFAULTS.put("options.registry.userDataLocation", 0);

        STRING_DEFAULTS.put("options.general.selectedLanguage", "");
        STRING_DEFAULTS.put("options.compareBinary.filePatterns", "*.bin");
        STRING_DEFAULTS.put("options.compareImage.filePatterns", "*.png;*.jpg;*.jpeg;*.bmp;*.gif;*.tif;*.tiff;*.webp");
        STRING_DEFAULTS.put("options.compareTable.csvPatterns", "*.csv");
        STRING_DEFAULTS.put("options.compareTable.csvDelimiter", ",");
        STRING_DEFAULTS.put("options.compareTable.tsvPatterns", "*.tsv");
        STRING_DEFAULTS.put("options.compareTable.dsvPatterns", "*.dsv");
        STRING_DEFAULTS.put("options.compareTable.dsvDelimiter", ";");
        STRING_DEFAULTS.put("options.compareTable.quoteChar", "\"");
        STRING_DEFAULTS.put("options.compareWebPage.urlPatternInclude", "*");
        STRING_DEFAULTS.put("options.compareWebPage.urlPatternExclude", "");
        STRING_DEFAULTS.put("options.compareMerge.breakSeparators", ".,;:");
        STRING_DEFAULTS.put("options.editorSyntax.extensionOverrides", "");
        STRING_DEFAULTS.put("options.compareFolder.additionalCondition", "");
        STRING_DEFAULTS.put("options.compareFolder.renameMoveKey", "");
        STRING_DEFAULTS.put("options.colorSchemes.lightScheme", "Default");
        STRING_DEFAULTS.put("options.colorSchemes.darkScheme", "Default Dark");
        STRING_DEFAULTS.put("options.dirColors.itemEqualColor", "#FFFFFF");
        STRING_DEFAULTS.put("options.dirColors.itemDiffColor", "#EFCB05");
        STRING_DEFAULTS.put("options.mergeColors.diffColor", "#EFCB05");
        STRING_DEFAULTS.put("options.markerColors.marker0Color", "#FFF7B2");
        STRING_DEFAULTS.put("options.syntaxColors.keywordColor", "#0000FF");
        STRING_DEFAULTS.put("options.textColors.regularBackgroundColor", "#FFFFFF");
        STRING_DEFAULTS.put("options.sysColors.hookColors", "");
        STRING_DEFAULTS.put("options.backups.globalFolder", "");
        STRING_DEFAULTS.put("options.registry.externalEditorCommand", "");
        STRING_DEFAULTS.put("options.registry.userFilterPath", "");
        STRING_DEFAULTS.put("options.registry.customTempPath", "");
        STRING_DEFAULTS.put("options.shell.jumpListTasks", "newText,newTable,newBinary,newImage,newWebPage,clipboard,options");
        STRING_DEFAULTS.put("options.lineFilters.patterns", "");
        STRING_DEFAULTS.put("options.substitutionFilters.entries", "");
        STRING_DEFAULTS.put("options.messageBoxes.overridesJson", "");
        STRING_DEFAULTS.put("options.project.matrixJson", "");
    }

    private final ConfigurationStore configurationStore;

    public AppSettingsStore(ConfigurationStore configurationStore) {
        this.configurationStore = Objects.requireNonNull(configurationStore, "configurationStore");
    }

    public AppSettings load() {
        AppSettings settings = new AppSettings();
        for (Map.Entry<String, Boolean> entry : BOOLEAN_DEFAULTS.entrySet()) {
            settings.setBoolean(
                entry.getKey(),
                entry.getValue(),
                configurationStore.getBoolean(entry.getKey(), entry.getValue())
            );
        }
        for (Map.Entry<String, Integer> entry : INTEGER_DEFAULTS.entrySet()) {
            settings.setInteger(
                entry.getKey(),
                entry.getValue(),
                configurationStore.getInt(entry.getKey(), entry.getValue())
            );
        }
        for (Map.Entry<String, String> entry : STRING_DEFAULTS.entrySet()) {
            settings.setString(
                entry.getKey(),
                entry.getValue(),
                configurationStore.get(entry.getKey()).orElse(entry.getValue())
            );
        }
        return settings;
    }

    public void save(AppSettings settings) {
        Objects.requireNonNull(settings, "settings");
        for (Map.Entry<String, Boolean> entry : BOOLEAN_DEFAULTS.entrySet()) {
            configurationStore.put(
                entry.getKey(),
                Boolean.toString(settings.getBoolean(entry.getKey(), entry.getValue()))
            );
        }
        for (Map.Entry<String, Integer> entry : INTEGER_DEFAULTS.entrySet()) {
            configurationStore.put(
                entry.getKey(),
                Integer.toString(settings.getInteger(entry.getKey(), entry.getValue()))
            );
        }
        for (Map.Entry<String, String> entry : STRING_DEFAULTS.entrySet()) {
            configurationStore.put(
                entry.getKey(),
                settings.getString(entry.getKey(), entry.getValue())
            );
        }
    }
}
