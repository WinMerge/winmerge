package org.winmerge.core.io;

import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;
import java.util.Objects;

public final class FileTextEncoding {
    public static final int CP_UTF_8 = 65001;
    public static final int CP_UCS2LE = 1200;
    public static final int CP_UCS2BE = 1201;

    private int codepage;
    private UnicodeEncoding unicoding;
    private boolean bom;

    public FileTextEncoding() {
        clear();
    }

    public void clear() {
        codepage = -1;
        unicoding = UnicodeEncoding.NONE;
        bom = false;
    }

    public int getCodepage() {
        return codepage;
    }

    public void setCodepage(int codepage) {
        this.codepage = codepage;
        switch (codepage) {
            case CP_UTF_8 -> unicoding = UnicodeEncoding.UTF8;
            case CP_UCS2LE -> unicoding = UnicodeEncoding.UCS2LE;
            case CP_UCS2BE -> unicoding = UnicodeEncoding.UCS2BE;
            default -> unicoding = UnicodeEncoding.NONE;
        }
    }

    public UnicodeEncoding getUnicoding() {
        return unicoding;
    }

    public void setUnicoding(UnicodeEncoding unicoding) {
        this.unicoding = unicoding;
        switch (unicoding) {
            case NONE -> {
                if (codepage == CP_UTF_8 || codepage == CP_UCS2LE || codepage == CP_UCS2BE) {
                    codepage = 0;
                }
            }
            case UTF8 -> codepage = CP_UTF_8;
            case UCS2LE -> codepage = CP_UCS2LE;
            case UCS2BE -> codepage = CP_UCS2BE;
        }
    }

    public boolean hasBom() {
        return bom;
    }

    public void setBom(boolean bom) {
        this.bom = bom;
    }

    public String getName() {
        if (unicoding == UnicodeEncoding.UTF8) {
            return bom ? "UTF-8 (B)" : "UTF-8";
        }
        if (unicoding == UnicodeEncoding.UCS2LE) {
            return "UCS-2 LE";
        }
        if (unicoding == UnicodeEncoding.UCS2BE) {
            return "UCS-2 BE";
        }
        if (codepage <= 0) {
            return "";
        }
        if (codepage == CP_UTF_8) {
            return "UTF-8";
        }
        return Integer.toString(codepage);
    }

    public Charset toCharset() {
        return switch (unicoding) {
            case UTF8 -> StandardCharsets.UTF_8;
            case UCS2LE -> StandardCharsets.UTF_16LE;
            case UCS2BE -> StandardCharsets.UTF_16BE;
            case NONE -> mapCodePageOrDefault(codepage);
        };
    }

    public static int collate(FileTextEncoding left, FileTextEncoding right) {
        int c = Integer.compare(left.unicoding.ordinal(), right.unicoding.ordinal());
        if (c != 0) {
            return c;
        }
        c = Integer.compare(left.codepage, right.codepage);
        if (c != 0) {
            return c;
        }
        return Boolean.compare(left.bom, right.bom);
    }

    private static Charset mapCodePageOrDefault(int codepage) {
        if (codepage == CP_UTF_8) {
            return StandardCharsets.UTF_8;
        }
        if (codepage == CP_UCS2LE) {
            return StandardCharsets.UTF_16LE;
        }
        if (codepage == CP_UCS2BE) {
            return StandardCharsets.UTF_16BE;
        }
        if (codepage > 0) {
            try {
                return Charset.forName("windows-" + codepage);
            } catch (Exception ignored) {
                return Charset.defaultCharset();
            }
        }
        return StandardCharsets.UTF_8;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (!(o instanceof FileTextEncoding that)) {
            return false;
        }
        return codepage == that.codepage && bom == that.bom && unicoding == that.unicoding;
    }

    @Override
    public int hashCode() {
        return Objects.hash(codepage, unicoding, bom);
    }
}
