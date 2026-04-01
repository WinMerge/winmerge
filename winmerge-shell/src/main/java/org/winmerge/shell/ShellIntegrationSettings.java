package org.winmerge.shell;

public record ShellIntegrationSettings(
    boolean contextMenuEnabled,
    boolean advancedContextMenu,
    boolean compareAsSubmenu,
    String jumpListTasks
) {
    private static final String DEFAULT_TASKS = "newText,newTable,newBinary,newImage,newWebPage,clipboard,options";

    public ShellIntegrationSettings {
        jumpListTasks = (jumpListTasks == null || jumpListTasks.isBlank()) ? DEFAULT_TASKS : jumpListTasks.trim();
    }

    public static ShellIntegrationSettings defaults() {
        return new ShellIntegrationSettings(false, false, false, DEFAULT_TASKS);
    }
}
