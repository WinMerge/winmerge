package org.winmerge.core.diff;

import org.winmerge.core.io.FileLocation;
import org.winmerge.core.io.FileSystemService;
import org.winmerge.core.io.FileTextStats;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.file.Path;

public final class DiffFileData implements AutoCloseable {
    private static final int OPEN_FILE_COUNT = 2;
    private static final int MAX_FILE_SLOTS = 3;

    private final FileSystemService fileSystem;
    private final ByteBuffer[] buffers;
    private final long[] fileSizes;
    private final FileLocation[] fileLocations;
    private final FileTextStats[] textStats;
    private final String[] displayFilepaths;

    private boolean used;

    public DiffFileData() {
        this(FileSystemService.createDefault());
    }

    public DiffFileData(FileSystemService fileSystem) {
        this.fileSystem = fileSystem;
        this.buffers = new ByteBuffer[OPEN_FILE_COUNT];
        this.fileSizes = new long[OPEN_FILE_COUNT];
        this.fileLocations = new FileLocation[MAX_FILE_SLOTS];
        this.textStats = new FileTextStats[MAX_FILE_SLOTS];
        this.displayFilepaths = new String[OPEN_FILE_COUNT];
        for (int i = 0; i < MAX_FILE_SLOTS; i++) {
            fileLocations[i] = new FileLocation();
            textStats[i] = new FileTextStats();
        }
        displayFilepaths[0] = "";
        displayFilepaths[1] = "";
        used = false;
    }

    public boolean openFiles(String filepath1, String filepath2) {
        fileLocations[0].setPath(filepath1);
        fileLocations[1].setPath(filepath2);
        if (!doOpenFiles()) {
            reset();
            return false;
        }
        return true;
    }

    public void setDisplayFilepaths(String trueFilepath1, String trueFilepath2) {
        displayFilepaths[0] = trueFilepath1 == null ? "" : trueFilepath1;
        displayFilepaths[1] = trueFilepath2 == null ? "" : trueFilepath2;
    }

    public boolean isUsed() {
        return used;
    }

    public FileLocation getFileLocation(int index) {
        validateSlotIndex(index);
        return fileLocations[index];
    }

    public FileTextStats getTextStats(int index) {
        validateSlotIndex(index);
        return textStats[index];
    }

    public String getDisplayFilepath(int index) {
        validateOpenFileIndex(index);
        return displayFilepaths[index];
    }

    public ByteBuffer getMappedBuffer(int index) {
        validateOpenFileIndex(index);
        ByteBuffer buffer = buffers[index];
        if (buffer == null) {
            return null;
        }
        return buffer.asReadOnlyBuffer();
    }

    public long getFileSize(int index) {
        validateOpenFileIndex(index);
        return fileSizes[index];
    }

    public void reset() {
        used = false;
        for (int i = 0; i < OPEN_FILE_COUNT; i++) {
            buffers[i] = null;
            fileSizes[i] = 0L;
        }
        for (int i = 0; i < MAX_FILE_SLOTS; i++) {
            textStats[i].clear();
        }
    }

    @Override
    public void close() {
        reset();
    }

    private boolean doOpenFiles() {
        reset();
        try {
            Path leftPath = fileSystem.normalize(Path.of(fileLocations[0].getFilePath()));
            Path rightPath = fileSystem.normalize(Path.of(fileLocations[1].getFilePath()));

            openSingle(0, leftPath);

            if (leftPath.equals(rightPath)) {
                buffers[1] = buffers[0].asReadOnlyBuffer();
                fileSizes[1] = fileSizes[0];
                copyTextStats(0, 1);
            } else {
                openSingle(1, rightPath);
            }

            used = true;
            return true;
        } catch (RuntimeException | IOException ex) {
            reset();
            return false;
        }
    }

    private void openSingle(int index, Path path) throws IOException {
        if (!fileSystem.exists(path) || !fileSystem.isRegularFile(path)) {
            throw new IOException("File is not readable: " + path);
        }

        ByteBuffer buffer = fileSystem.mapReadOnly(path);
        buffers[index] = buffer.asReadOnlyBuffer();
        fileSizes[index] = fileSystem.size(path);
        collectTextStats(textStats[index], buffers[index]);
    }

    private static void collectTextStats(FileTextStats destination, ByteBuffer buffer) {
        destination.clear();
        int limit = buffer.limit();
        for (int i = 0; i < limit; i++) {
            byte value = buffer.get(i);
            if (value == 0) {
                destination.setZeroCount(destination.getZeroCount() + 1);
                continue;
            }

            if (value == '\r') {
                if (i + 1 < limit && buffer.get(i + 1) == '\n') {
                    destination.setCrlfCount(destination.getCrlfCount() + 1);
                    i++;
                } else {
                    destination.setCrCount(destination.getCrCount() + 1);
                }
                continue;
            }

            if (value == '\n') {
                destination.setLfCount(destination.getLfCount() + 1);
            }
        }
    }

    private void copyTextStats(int sourceIndex, int targetIndex) {
        FileTextStats source = textStats[sourceIndex];
        FileTextStats target = textStats[targetIndex];
        target.setCrCount(source.getCrCount());
        target.setLfCount(source.getLfCount());
        target.setCrlfCount(source.getCrlfCount());
        target.setZeroCount(source.getZeroCount());
    }

    private static void validateOpenFileIndex(int index) {
        if (index < 0 || index >= OPEN_FILE_COUNT) {
            throw new IllegalArgumentException("Open file index must be in [0,1]");
        }
    }

    private static void validateSlotIndex(int index) {
        if (index < 0 || index >= MAX_FILE_SLOTS) {
            throw new IllegalArgumentException("File slot index must be in [0,2]");
        }
    }
}
