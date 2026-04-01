package org.winmerge.core.io;

public final class FileLocation {
    private String filePath;
    private final FileTextEncoding encoding;

    public FileLocation() {
        this.filePath = "";
        this.encoding = new FileTextEncoding();
    }

    public FileLocation(String filePath) {
        this.filePath = filePath;
        this.encoding = new FileTextEncoding();
    }

    public String getFilePath() {
        return filePath;
    }

    public void setPath(String filePath) {
        this.filePath = filePath;
    }

    public FileTextEncoding getEncoding() {
        return encoding;
    }
}
