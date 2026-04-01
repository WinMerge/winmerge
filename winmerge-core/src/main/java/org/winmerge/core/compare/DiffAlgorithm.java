package org.winmerge.core.compare;

public enum DiffAlgorithm {
    DEFAULT,
    MINIMAL,
    PATIENCE,
    HISTOGRAM,
    NONE;

    static DiffAlgorithm fromLegacyValue(int value) {
        return switch (value) {
            case 0 -> DEFAULT;
            case 1 -> MINIMAL;
            case 2 -> PATIENCE;
            case 3 -> HISTOGRAM;
            case 4 -> NONE;
            default -> throw new IllegalArgumentException("Unknown diff algorithm value: " + value);
        };
    }

    int toLegacyValue() {
        return switch (this) {
            case DEFAULT -> 0;
            case MINIMAL -> 1;
            case PATIENCE -> 2;
            case HISTOGRAM -> 3;
            case NONE -> 4;
        };
    }
}
