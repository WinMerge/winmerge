package org.winmerge.core.diff;

public final class DiffEngine {
    private DiffEngine() {
    }

    public enum CompareType {
        CONTENT,
        QUICK_CONTENT,
        BINARY_CONTENT,
        DATE,
        DATE_SIZE,
        SIZE,
        EXISTENCE,
        IMAGE_CONTENT
    }

    public enum IdentLevel {
        ALL,
        NONE,
        EXCEPT_LEFT,
        EXCEPT_MIDDLE,
        EXCEPT_RIGHT
    }

    public static final class DiffStatus {
        private final boolean[] missingNl;
        private boolean binaries;
        private IdentLevel identical;
        private boolean patchFileFailed;

        public DiffStatus() {
            this.missingNl = new boolean[3];
            this.binaries = false;
            this.identical = IdentLevel.NONE;
            this.patchFileFailed = false;
        }

        public boolean[] missingNl() {
            return missingNl;
        }

        public boolean binaries() {
            return binaries;
        }

        public void setBinaries(boolean binaries) {
            this.binaries = binaries;
        }

        public IdentLevel identical() {
            return identical;
        }

        public void setIdentical(IdentLevel identical) {
            this.identical = identical;
        }

        public boolean patchFileFailed() {
            return patchFileFailed;
        }

        public void setPatchFileFailed(boolean patchFileFailed) {
            this.patchFileFailed = patchFileFailed;
        }

        public void mergeStatus(DiffStatus other) {
            if (identical == IdentLevel.ALL) {
                identical = other.identical;
            } else if ((identical == IdentLevel.EXCEPT_LEFT && other.identical != IdentLevel.EXCEPT_LEFT)
                    || (identical == IdentLevel.EXCEPT_RIGHT && other.identical != IdentLevel.EXCEPT_RIGHT)
                    || (identical == IdentLevel.EXCEPT_MIDDLE && other.identical != IdentLevel.EXCEPT_MIDDLE)) {
                identical = IdentLevel.NONE;
            }

            if (other.patchFileFailed) {
                patchFileFailed = true;
            }
            if (other.binaries) {
                binaries = true;
            }
            System.arraycopy(other.missingNl, 0, missingNl, 0, missingNl.length);
        }
    }

    public static final class PostFilterContext {
        private int parsedLineEndLeft = -1;
        private int parsedLineEndRight = -1;
        private int cookieLeft = 0;
        private int cookieRight = 0;

        public int parsedLineEndLeft() {
            return parsedLineEndLeft;
        }

        public void setParsedLineEndLeft(int parsedLineEndLeft) {
            this.parsedLineEndLeft = parsedLineEndLeft;
        }

        public int parsedLineEndRight() {
            return parsedLineEndRight;
        }

        public void setParsedLineEndRight(int parsedLineEndRight) {
            this.parsedLineEndRight = parsedLineEndRight;
        }

        public int cookieLeft() {
            return cookieLeft;
        }

        public void setCookieLeft(int cookieLeft) {
            this.cookieLeft = cookieLeft;
        }

        public int cookieRight() {
            return cookieRight;
        }

        public void setCookieRight(int cookieRight) {
            this.cookieRight = cookieRight;
        }
    }
}
