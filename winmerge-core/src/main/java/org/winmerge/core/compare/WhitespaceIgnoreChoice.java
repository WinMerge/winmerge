package org.winmerge.core.compare;

public enum WhitespaceIgnoreChoice {
    COMPARE_ALL,
    IGNORE_CHANGE,
    IGNORE_ALL;

    static WhitespaceIgnoreChoice fromLegacyValue(int value) {
        return switch (value) {
            case 0 -> COMPARE_ALL;
            case 1 -> IGNORE_CHANGE;
            case 2 -> IGNORE_ALL;
            default -> throw new IllegalArgumentException("Unknown whitespace ignore value: " + value);
        };
    }

    int toLegacyValue() {
        return switch (this) {
            case COMPARE_ALL -> 0;
            case IGNORE_CHANGE -> 1;
            case IGNORE_ALL -> 2;
        };
    }
}
