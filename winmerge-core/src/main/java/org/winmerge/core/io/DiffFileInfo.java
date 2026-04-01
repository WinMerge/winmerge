package org.winmerge.core.io;

import java.util.HashMap;
import java.util.Map;

public final class DiffFileInfo {
    private String filePath;
    private String version;
    private final FileTextEncoding encoding;
    private final FileTextStats textStats;
    private final Map<String, Object> additionalProperties;

    public DiffFileInfo() {
        filePath = "";
        version = "";
        encoding = new FileTextEncoding();
        textStats = new FileTextStats();
        additionalProperties = new HashMap<>();
    }

    public void clearPartial() {
        version = "";
        encoding.clear();
        textStats.clear();
        additionalProperties.clear();
    }

    public boolean isEditableEncoding() {
        return !encoding.hasBom();
    }

    public String getFilePath() {
        return filePath;
    }

    public void setFilePath(String filePath) {
        this.filePath = filePath;
    }

    public String getVersion() {
        return version;
    }

    public void setVersion(String version) {
        this.version = version;
    }

    public FileTextEncoding getEncoding() {
        return encoding;
    }

    public FileTextStats getTextStats() {
        return textStats;
    }

    public Map<String, Object> getAdditionalProperties() {
        return additionalProperties;
    }
}
