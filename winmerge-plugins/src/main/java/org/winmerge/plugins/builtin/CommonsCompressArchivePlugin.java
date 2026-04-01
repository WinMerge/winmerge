package org.winmerge.plugins.builtin;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.EnumSet;
import java.util.Locale;
import java.util.Set;
import java.util.stream.Stream;

import org.apache.commons.compress.archivers.ArchiveEntry;
import org.apache.commons.compress.archivers.ArchiveException;
import org.apache.commons.compress.archivers.ArchiveInputStream;
import org.apache.commons.compress.archivers.ArchiveOutputStream;
import org.apache.commons.compress.archivers.ArchiveStreamFactory;
import org.apache.commons.compress.archivers.sevenz.SevenZArchiveEntry;
import org.apache.commons.compress.archivers.sevenz.SevenZFile;
import org.apache.commons.compress.archivers.sevenz.SevenZOutputFile;
import org.apache.commons.compress.archivers.tar.TarArchiveEntry;
import org.apache.commons.compress.archivers.tar.TarArchiveOutputStream;
import org.apache.commons.compress.archivers.zip.ZipArchiveEntry;
import org.apache.commons.compress.archivers.zip.ZipArchiveOutputStream;
import org.apache.commons.compress.compressors.CompressorException;
import org.apache.commons.compress.compressors.CompressorStreamFactory;
import org.apache.commons.compress.compressors.bzip2.BZip2CompressorInputStream;
import org.apache.commons.compress.compressors.bzip2.BZip2CompressorOutputStream;
import org.apache.commons.compress.compressors.gzip.GzipCompressorInputStream;
import org.apache.commons.compress.compressors.gzip.GzipCompressorOutputStream;
import org.apache.commons.compress.compressors.xz.XZCompressorInputStream;
import org.apache.commons.compress.compressors.xz.XZCompressorOutputStream;
import org.winmerge.plugins.spi.ArchivePlugin;
import org.winmerge.plugins.spi.PluginCapability;
import org.winmerge.plugins.spi.PluginDescriptor;

public final class CommonsCompressArchivePlugin implements ArchivePlugin {
    private static final Set<String> SUPPORTED_EXTENSIONS = Set.of(
        ".zip",
        ".jar",
        ".7z",
        ".tar",
        ".tgz",
        ".tar.gz",
        ".tbz2",
        ".tar.bz2",
        ".txz",
        ".tar.xz"
    );

    private static final PluginDescriptor DESCRIPTOR = new PluginDescriptor(
        "archive.commons-compress",
        "Commons Compress Archive Plugin",
        "1.0.0",
        EnumSet.of(PluginCapability.ARCHIVE)
    );

    @Override
    public PluginDescriptor descriptor() {
        return DESCRIPTOR;
    }

    @Override
    public boolean supportsArchive(Path archivePath) {
        if (supportsByExtension(archivePath)) {
            return true;
        }
        if (!Files.isRegularFile(archivePath)) {
            return false;
        }
        return isSevenZipArchiveBySignature(archivePath)
            || supportsByKnownArchiveSignature(archivePath)
            || supportsCompressedTarSignature(archivePath);
    }

    @Override
    public void extract(Path archivePath, Path destinationDirectory) throws IOException {
        if (!supportsArchive(archivePath)) {
            throw new IOException("Unsupported archive extension for " + archivePath);
        }
        Files.createDirectories(destinationDirectory);
        Path normalizedDestination = destinationDirectory.toAbsolutePath().normalize();

        if (isSevenZipArchive(archivePath)) {
            extractSevenZipArchive(archivePath, normalizedDestination);
            return;
        }

        try (InputStream payload = openArchivePayloadInputStream(archivePath);
             ArchiveInputStream archiveInput = openArchiveInputStream(archivePath, payload)) {
            ArchiveEntry entry;
            while ((entry = archiveInput.getNextEntry()) != null) {
                Path outputPath = normalizedDestination.resolve(entry.getName()).normalize();
                if (!outputPath.startsWith(normalizedDestination)) {
                    throw new IOException("Blocked zip-slip path: " + entry.getName());
                }
                if (entry.isDirectory()) {
                    Files.createDirectories(outputPath);
                    continue;
                }
                Path parent = outputPath.getParent();
                if (parent != null) {
                    Files.createDirectories(parent);
                }
                try (OutputStream output = Files.newOutputStream(outputPath)) {
                    archiveInput.transferTo(output);
                }
            }
        }
    }

    @Override
    public void create(Path sourceDirectory, Path archivePath) throws IOException {
        if (!Files.isDirectory(sourceDirectory)) {
            throw new IOException("Source path must be a directory: " + sourceDirectory);
        }
        Path parent = archivePath.toAbsolutePath().normalize().getParent();
        if (parent != null) {
            Files.createDirectories(parent);
        }

        if (isSevenZipArchiveByExtension(archivePath)) {
            createSevenZipArchive(sourceDirectory, archivePath);
            return;
        }

        try (ArchiveOutputStream<?> archiveOutput = openArchiveOutputStream(archivePath);
             Stream<Path> stream = Files.walk(sourceDirectory)) {
            if (archiveOutput instanceof ZipArchiveOutputStream zipOutput) {
                stream
                    .filter(path -> !sourceDirectory.equals(path))
                    .sorted()
                    .forEach(path -> writeZipArchiveEntry(sourceDirectory, path, zipOutput));
            } else if (archiveOutput instanceof TarArchiveOutputStream tarOutput) {
                stream
                    .filter(path -> !sourceDirectory.equals(path))
                    .sorted()
                    .forEach(path -> writeTarArchiveEntry(sourceDirectory, path, tarOutput));
            } else {
                throw new IOException("Unsupported archive output stream type: " + archiveOutput.getClass().getName());
            }
            archiveOutput.finish();
        } catch (RuntimeException ex) {
            if (ex.getCause() instanceof IOException ioException) {
                throw ioException;
            }
            throw ex;
        }
    }

    private void extractSevenZipArchive(Path archivePath, Path destinationDirectory) throws IOException {
        try (SevenZFile sevenZFile = new SevenZFile(archivePath.toFile())) {
            SevenZArchiveEntry entry;
            byte[] buffer = new byte[8192];
            while ((entry = sevenZFile.getNextEntry()) != null) {
                Path outputPath = destinationDirectory.resolve(entry.getName()).normalize();
                if (!outputPath.startsWith(destinationDirectory)) {
                    throw new IOException("Blocked zip-slip path: " + entry.getName());
                }
                if (entry.isDirectory()) {
                    Files.createDirectories(outputPath);
                    continue;
                }
                Path parent = outputPath.getParent();
                if (parent != null) {
                    Files.createDirectories(parent);
                }
                try (OutputStream output = new BufferedOutputStream(Files.newOutputStream(outputPath))) {
                    int read;
                    while ((read = sevenZFile.read(buffer)) != -1) {
                        output.write(buffer, 0, read);
                    }
                }
            }
        }
    }

    private void createSevenZipArchive(Path sourceDirectory, Path archivePath) throws IOException {
        try (SevenZOutputFile sevenZOutput = new SevenZOutputFile(archivePath.toFile());
             Stream<Path> stream = Files.walk(sourceDirectory)) {
            stream
                .filter(path -> !sourceDirectory.equals(path))
                .sorted()
                .forEach(path -> writeSevenZipEntry(sourceDirectory, path, sevenZOutput));
        } catch (RuntimeException ex) {
            if (ex.getCause() instanceof IOException ioException) {
                throw ioException;
            }
            throw ex;
        }
    }

    private void writeZipArchiveEntry(Path sourceDirectory, Path path, ZipArchiveOutputStream archiveOutput) {
        String entryName = sourceDirectory.relativize(path).toString().replace('\\', '/');
        try {
            ZipArchiveEntry entry = new ZipArchiveEntry(entryName + (Files.isDirectory(path) ? "/" : ""));
            if (Files.isRegularFile(path)) {
                entry.setSize(Files.size(path));
            }
            archiveOutput.putArchiveEntry(entry);
            if (Files.isRegularFile(path)) {
                Files.copy(path, archiveOutput);
            }
            archiveOutput.closeArchiveEntry();
        } catch (IOException ex) {
            throw new RuntimeException(ex);
        }
    }

    private void writeTarArchiveEntry(Path sourceDirectory, Path path, TarArchiveOutputStream archiveOutput) {
        String entryName = sourceDirectory.relativize(path).toString().replace('\\', '/');
        try {
            TarArchiveEntry entry = new TarArchiveEntry(path.toFile(), entryName);
            if (Files.isDirectory(path) && !entryName.endsWith("/")) {
                entry.setName(entryName + "/");
            }
            archiveOutput.putArchiveEntry(entry);
            if (Files.isRegularFile(path)) {
                Files.copy(path, archiveOutput);
            }
            archiveOutput.closeArchiveEntry();
        } catch (IOException ex) {
            throw new RuntimeException(ex);
        }
    }

    private void writeSevenZipEntry(Path sourceDirectory, Path path, SevenZOutputFile sevenZOutput) {
        String entryName = sourceDirectory.relativize(path).toString().replace('\\', '/');
        if (Files.isDirectory(path) && !entryName.endsWith("/")) {
            entryName = entryName + "/";
        }
        try {
            SevenZArchiveEntry entry = sevenZOutput.createArchiveEntry(path.toFile(), entryName);
            sevenZOutput.putArchiveEntry(entry);
            if (Files.isRegularFile(path)) {
                try (InputStream input = new BufferedInputStream(Files.newInputStream(path))) {
                    byte[] buffer = new byte[8192];
                    int read;
                    while ((read = input.read(buffer)) != -1) {
                        sevenZOutput.write(buffer, 0, read);
                    }
                }
            }
            sevenZOutput.closeArchiveEntry();
        } catch (IOException ex) {
            throw new RuntimeException(ex);
        }
    }

    private boolean supportsByExtension(Path archivePath) {
        String fileName = archivePath.getFileName().toString().toLowerCase(Locale.ROOT);
        return SUPPORTED_EXTENSIONS.stream().anyMatch(fileName::endsWith);
    }

    private boolean isSevenZipArchive(Path archivePath) {
        return isSevenZipArchiveByExtension(archivePath) || isSevenZipArchiveBySignature(archivePath);
    }

    private boolean isSevenZipArchiveByExtension(Path archivePath) {
        String lower = archivePath.getFileName().toString().toLowerCase(Locale.ROOT);
        return lower.endsWith(".7z");
    }

    private boolean isSevenZipArchiveBySignature(Path archivePath) {
        try (InputStream input = new BufferedInputStream(Files.newInputStream(archivePath))) {
            byte[] signature = input.readNBytes(12);
            return SevenZFile.matches(signature, signature.length);
        } catch (IOException ex) {
            return false;
        }
    }

    private boolean supportsByKnownArchiveSignature(Path archivePath) {
        try (InputStream input = new BufferedInputStream(Files.newInputStream(archivePath))) {
            String detected = ArchiveStreamFactory.detect(input);
            return ArchiveStreamFactory.ZIP.equals(detected) || ArchiveStreamFactory.TAR.equals(detected);
        } catch (ArchiveException | IOException ex) {
            return false;
        }
    }

    private boolean supportsCompressedTarSignature(Path archivePath) {
        String compressor;
        try {
            compressor = detectCompressor(archivePath);
        } catch (IOException ex) {
            return false;
        }
        if (compressor == null) {
            return false;
        }

        try (InputStream payload = openCompressedPayloadInputStream(archivePath, compressor);
             InputStream bufferedPayload = new BufferedInputStream(payload)) {
            String detected = ArchiveStreamFactory.detect(bufferedPayload);
            return ArchiveStreamFactory.TAR.equals(detected);
        } catch (ArchiveException | IOException ex) {
            return false;
        }
    }

    private InputStream openArchivePayloadInputStream(Path archivePath) throws IOException {
        String compressor = compressorFromExtension(archivePath);
        if (compressor == null) {
            compressor = detectCompressor(archivePath);
        }
        if (compressor != null) {
            return openCompressedPayloadInputStream(archivePath, compressor);
        }
        return new BufferedInputStream(Files.newInputStream(archivePath));
    }

    private ArchiveInputStream openArchiveInputStream(Path archivePath, InputStream payload) throws IOException {
        String lower = archivePath.getFileName().toString().toLowerCase(Locale.ROOT);
        try {
            if (lower.endsWith(".zip") || lower.endsWith(".jar")) {
                return new ArchiveStreamFactory().createArchiveInputStream(ArchiveStreamFactory.ZIP, payload);
            }
            if (lower.endsWith(".tar")
                || lower.endsWith(".tgz")
                || lower.endsWith(".tar.gz")
                || lower.endsWith(".tbz2")
                || lower.endsWith(".tar.bz2")
                || lower.endsWith(".txz")
                || lower.endsWith(".tar.xz")) {
                return new ArchiveStreamFactory().createArchiveInputStream(ArchiveStreamFactory.TAR, payload);
            }
            return new ArchiveStreamFactory().createArchiveInputStream(payload);
        } catch (ArchiveException ex) {
            throw new IOException("Failed to read archive stream for " + archivePath, ex);
        }
    }

    private String compressorFromExtension(Path archivePath) {
        String lower = archivePath.getFileName().toString().toLowerCase(Locale.ROOT);
        if (lower.endsWith(".tgz") || lower.endsWith(".tar.gz")) {
            return CompressorStreamFactory.GZIP;
        }
        if (lower.endsWith(".tbz2") || lower.endsWith(".tar.bz2")) {
            return CompressorStreamFactory.BZIP2;
        }
        if (lower.endsWith(".txz") || lower.endsWith(".tar.xz")) {
            return CompressorStreamFactory.XZ;
        }
        return null;
    }

    private String detectCompressor(Path archivePath) throws IOException {
        try (InputStream input = new BufferedInputStream(Files.newInputStream(archivePath))) {
            return CompressorStreamFactory.detect(input);
        } catch (CompressorException ex) {
            return null;
        }
    }

    private InputStream openCompressedPayloadInputStream(Path archivePath, String compressor) throws IOException {
        InputStream fileInput = new BufferedInputStream(Files.newInputStream(archivePath));
        try {
            if (CompressorStreamFactory.GZIP.equals(compressor)) {
                return new GzipCompressorInputStream(fileInput, true);
            }
            if (CompressorStreamFactory.BZIP2.equals(compressor)) {
                return new BZip2CompressorInputStream(fileInput, true);
            }
            if (CompressorStreamFactory.XZ.equals(compressor)) {
                return new XZCompressorInputStream(fileInput, true);
            }
            return fileInput;
        } catch (IOException ex) {
            try {
                fileInput.close();
            } catch (IOException ignored) {
            }
            throw ex;
        }
    }

    private ArchiveOutputStream<?> openArchiveOutputStream(Path archivePath) throws IOException {
        String lower = archivePath.getFileName().toString().toLowerCase(Locale.ROOT);

        OutputStream fileOutput = new BufferedOutputStream(Files.newOutputStream(archivePath));
        try {
            if (lower.endsWith(".zip") || lower.endsWith(".jar")) {
                return new ZipArchiveOutputStream(fileOutput);
            }
            if (lower.endsWith(".tar")) {
                TarArchiveOutputStream output = new TarArchiveOutputStream(fileOutput);
                output.setLongFileMode(TarArchiveOutputStream.LONGFILE_POSIX);
                return output;
            }
            if (lower.endsWith(".tgz") || lower.endsWith(".tar.gz")) {
                TarArchiveOutputStream output = new TarArchiveOutputStream(new GzipCompressorOutputStream(fileOutput));
                output.setLongFileMode(TarArchiveOutputStream.LONGFILE_POSIX);
                return output;
            }
            if (lower.endsWith(".tbz2") || lower.endsWith(".tar.bz2")) {
                TarArchiveOutputStream output = new TarArchiveOutputStream(new BZip2CompressorOutputStream(fileOutput));
                output.setLongFileMode(TarArchiveOutputStream.LONGFILE_POSIX);
                return output;
            }
            if (lower.endsWith(".txz") || lower.endsWith(".tar.xz")) {
                TarArchiveOutputStream output = new TarArchiveOutputStream(new XZCompressorOutputStream(fileOutput));
                output.setLongFileMode(TarArchiveOutputStream.LONGFILE_POSIX);
                return output;
            }
            throw new IOException("Unsupported archive extension for " + archivePath);
        } catch (IOException ex) {
            try {
                fileOutput.close();
            } catch (IOException ignored) {
            }
            throw ex;
        }
    }
}
