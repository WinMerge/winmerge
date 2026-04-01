package org.winmerge.core.filter;

import java.util.ArrayList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public final class SubstitutionList {
    public static final class SubstitutionItem {
        private final String pattern;
        private final String replacement;
        private final int regexpCompileOptions;
        private final Pattern regexp;

        public SubstitutionItem(String pattern, String replacement, int regexpCompileOptions) {
            this.pattern = pattern;
            this.replacement = replaceEscapeSequences(replacement);
            this.regexpCompileOptions = regexpCompileOptions;
            this.regexp = Pattern.compile(pattern, regexpCompileOptions);
        }

        public String pattern() {
            return pattern;
        }

        public String replacement() {
            return replacement;
        }

        public int regexpCompileOptions() {
            return regexpCompileOptions;
        }

        public Pattern regexp() {
            return regexp;
        }
    }

    private final List<SubstitutionItem> list;

    public SubstitutionList() {
        list = new ArrayList<>();
    }

    public void add(String pattern, String replacement, int regexpCompileOptions) {
        list.add(new SubstitutionItem(pattern, replacement, regexpCompileOptions));
    }

    public void add(String pattern, String replacement, boolean caseSensitive, boolean matchWholeWordOnly) {
        int regexpCompileOptions = caseSensitive ? 0 : Pattern.CASE_INSENSITIVE;
        String regexPattern = Pattern.quote(pattern);
        if (matchWholeWordOnly) {
            regexPattern = "\\b" + regexPattern + "\\b";
        }
        list.add(new SubstitutionItem(regexPattern, replacement, regexpCompileOptions));
    }

    public void removeAllFilters() {
        list.clear();
    }

    public boolean hasRegExps() {
        return !list.isEmpty();
    }

    public int getCount() {
        return list.size();
    }

    public SubstitutionItem get(int index) {
        return list.get(index);
    }

    public String subst(String subject) {
        return subst(subject, 65001);
    }

    public String subst(String subject, int codepage) {
        String replaced = subject;
        for (SubstitutionItem item : list) {
            try {
                Matcher matcher = item.regexp().matcher(replaced);
                replaced = matcher.replaceAll(item.replacement());
            } catch (RuntimeException ignored) {
                // Keep substitution robust in face of malformed patterns/replacements.
            }
        }
        return replaced;
    }

    private static String replaceEscapeSequences(String input) {
        StringBuilder result = new StringBuilder(input.length());
        for (int i = 0; i < input.length(); i++) {
            char current = input.charAt(i);
            if (current != '\\' || i + 1 >= input.length()) {
                result.append(current);
                continue;
            }

            char next = input.charAt(i + 1);
            switch (next) {
                case 'a' -> result.append('\u0007');
                case 'b' -> result.append('\b');
                case 'f' -> result.append('\f');
                case 'n' -> result.append('\n');
                case 'r' -> result.append('\r');
                case 't' -> result.append('\t');
                case 'v' -> result.append('\u000B');
                case 'x' -> {
                    if (i + 3 < input.length()) {
                        String hex = input.substring(i + 2, i + 4);
                        try {
                            int value = Integer.parseInt(hex, 16);
                            result.append((char) value);
                            i += 2;
                        } catch (NumberFormatException ex) {
                            result.append("\\x");
                        }
                    } else {
                        result.append("\\x");
                    }
                }
                default -> {
                    if (Character.isDigit(next)) {
                        result.append('$');
                    }
                    result.append(next);
                }
            }
            i++;
        }
        return result.toString();
    }
}
