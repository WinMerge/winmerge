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
import org.apache.commons.compress.archivers.tar.TarArchiveEntry;
import org.apache.commons.compress.archivers.tar.TarArchiveOutputStream;
import org.apache.commons.compress.archivers.zip.ZipArchiveEntry;
import org.apache.commons.compress.archivers.zip.ZipArchiveOutputStream;
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
        String fileName = archivePath.getFileName().toString().toLowerCase(Locale.ROOT);
        return SUPPORTED_EXTENSIONS.stream().anyMatch(fileName::endsWith);
    }

    @Override
    public void extract(Path archivePath, Path destinationDirectory) throws IOException {
        if (!supportsArchive(archivePath)) {
            throw new IOException("Unsupported archive extension for " + archivePath);
        }
        Files.createDirectories(destinationDirectory);
        Path normalizedDestination = destinationDirectory.toAbsolutePath().normalize();

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
        try (ArchiveOutputStream archiveOutput = openArchiveOutputStream(archivePath);
             Stream<Path> stream = Files.walk(sourceDirectory)) {
            stream
                .filter(path -> !sourceDirectory.equals(path))
                .sorted()
                .forEach(path -> writeArchiveEntry(sourceDirectory, path, archiveOutput));
            archiveOutput.finish();
        } catch (RuntimeException ex) {
            if (ex.getCause() instanceof IOException ioException) {
                throw ioException;
            }
            throw ex;
        }
    }

    private void writeArchiveEntry(Path sourceDirectory, Path path, ArchiveOutputStream archiveOutput) {
        String entryName = sourceDirectory.relativize(path).toString().replace('\\', '/');
        try {
            if (archiveOutput instanceof ZipArchiveOutputStream) {
                ZipArchiveEntry entry = new ZipArchiveEntry(entryName + (Files.isDirectory(path) ? "/" : ""));
                if (Files.isRegularFile(path)) {
                    entry.setSize(Files.size(path));
                }
                archiveOutput.putArchiveEntry(entry);
                if (Files.isRegularFile(path)) {
                    Files.copy(path, archiveOutput);
                }
                archiveOutput.closeArchiveEntry();
                return;
            }

            if (archiveOutput instanceof TarArchiveOutputStream) {
                TarArchiveEntry entry = new TarArchiveEntry(path.toFile(), entryName);
                if (Files.isDirectory(path) && !entryName.endsWith("/")) {
                    entry.setName(entryName + "/");
                }
                archiveOutput.putArchiveEntry(entry);
                if (Files.isRegularFile(path)) {
                    Files.copy(path, archiveOutput);
                }
                archiveOutput.closeArchiveEntry();
                return;
            }

            throw new IOException("Unsupported archive output stream type: " + archiveOutput.getClass().getName());
        } catch (IOException ex) {
            throw new RuntimeException(ex);
        }
    }

    private InputStream openArchivePayloadInputStream(Path archivePath) throws IOException {
        String lower = archivePath.getFileName().toString().toLowerCase(Locale.ROOT);
        InputStream fileInput = new BufferedInputStream(Files.newInputStream(archivePath));
        if (lower.endsWith(".tgz") || lower.endsWith(".tar.gz")) {
            return new GzipCompressorInputStream(fileInput, true);
        }
        if (lower.endsWith(".tbz2") || lower.endsWith(".tar.bz2")) {
            return new BZip2CompressorInputStream(fileInput, true);
        }
        if (lower.endsWith(".txz") || lower.endsWith(".tar.xz")) {
            return new XZCompressorInputStream(fileInput, true);
        }
        return fileInput;
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

    private ArchiveOutputStream openArchiveOutputStream(Path archivePath) throws IOException {
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
