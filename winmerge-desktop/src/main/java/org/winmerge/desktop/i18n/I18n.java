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
        String escapedQuotes = value.replace("'", "''");
        Matcher matcher = POSITIONAL_PLACEHOLDER.matcher(escapedQuotes);
        StringBuffer converted = new StringBuffer();
        while (matcher.find()) {
            int oneBased = Integer.parseInt(matcher.group(1));
            matcher.appendReplacement(converted, "{" + Math.max(0, oneBased - 1) + "}");
        }
        matcher.appendTail(converted);
        return converted.toString();
    }
}
