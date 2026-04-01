package org.winmerge.desktop.i18n;

import java.text.MessageFormat;
import java.util.Locale;
import java.util.MissingResourceException;
import java.util.ResourceBundle;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public final class I18n {
    private static final String BASE_NAME = "i18n.WinMerge";
    private static final Pattern POSITIONAL_PLACEHOLDER = Pattern.compile("%(\\d+)");

    private static volatile ResourceBundle bundle = ResourceBundle.getBundle(BASE_NAME, Locale.getDefault());

    private I18n() {
    }

    public static void setLocale(Locale locale) {
        bundle = ResourceBundle.getBundle(BASE_NAME, locale);
    }

    public static String tr(String key) {
        try {
            return bundle.getString(key);
        } catch (MissingResourceException ex) {
            return key;
        }
    }

    public static String tr(String key, Object... args) {
        String template = tr(key);
        if (args == null || args.length == 0) {
            return template;
        }

        String pattern = normalizeForMessageFormat(template);
        return MessageFormat.format(pattern, args);
    }

    static void resetForTests() {
        bundle = ResourceBundle.getBundle(BASE_NAME, Locale.getDefault());
    }

    private static String normalizeForMessageFormat(String value) {
        Matcher matcher = POSITIONAL_PLACEHOLDER.matcher(value);
        StringBuilder converted = new StringBuilder(value.length() + 16);
        int cursor = 0;
        while (matcher.find()) {
            appendEscapedLiteral(converted, value, cursor, matcher.start());
            int oneBased = Integer.parseInt(matcher.group(1));
            converted.append('{').append(Math.max(0, oneBased - 1)).append('}');
            cursor = matcher.end();
        }
        appendEscapedLiteral(converted, value, cursor, value.length());
        return converted.toString();
    }

    private static void appendEscapedLiteral(StringBuilder output, String value, int start, int end) {
        for (int i = start; i < end; i++) {
            char ch = value.charAt(i);
            if (ch == '\'') {
                output.append("''");
            } else if (ch == '{' || ch == '}') {
                output.append('\'').append(ch).append('\'');
            } else {
                output.append(ch);
            }
        }
    }
}
