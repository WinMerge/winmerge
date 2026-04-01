package org.winmerge.desktop.ui.merge;

import java.nio.file.Path;
import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.Optional;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.fxmisc.richtext.model.StyleSpans;
import org.fxmisc.richtext.model.StyleSpansBuilder;

/**
 * Adapter facade for TextMate grammar-based highlighting.
 *
 * <p>The TM4E migration path is preserved via `.tmLanguage.json` grammar resources and a parser
 * abstraction. This baseline implementation compiles scope spans from language profiles to keep
 * AMP-15 behavior deterministic while richer TM4E token streams are integrated incrementally.
 */
public final class TextMateGrammarParser {
    private static final Map<String, GrammarProfile> PROFILES_BY_EXTENSION = buildProfiles();

    public StyleSpans<Collection<String>> computeStyleSpans(Path filePath, String text) {
        Optional<GrammarProfile> profile = profileFor(filePath);
        if (profile.isEmpty()) {
            StyleSpansBuilder<Collection<String>> noStyles = new StyleSpansBuilder<>();
            noStyles.add(Collections.emptyList(), text.length());
            return noStyles.create();
        }

        Matcher matcher = profile.get().tokenPattern.matcher(text);
        int lastMatchEnd = 0;
        StyleSpansBuilder<Collection<String>> builder = new StyleSpansBuilder<>();
        while (matcher.find()) {
            builder.add(Collections.emptyList(), matcher.start() - lastMatchEnd);
            builder.add(Collections.singleton(tokenStyleFor(matcher)), matcher.end() - matcher.start());
            lastMatchEnd = matcher.end();
        }
        builder.add(Collections.emptyList(), text.length() - lastMatchEnd);
        return builder.create();
    }

    public Optional<String> grammarResourceFor(Path filePath) {
        return profileFor(filePath).map(profile -> profile.grammarResourcePath);
    }

    private Optional<GrammarProfile> profileFor(Path filePath) {
        String fileName = filePath.getFileName().toString();
        int extensionIndex = fileName.lastIndexOf('.');
        if (extensionIndex < 0 || extensionIndex == fileName.length() - 1) {
            return Optional.empty();
        }
        String extension = fileName.substring(extensionIndex + 1).toLowerCase(Locale.ROOT);
        return Optional.ofNullable(PROFILES_BY_EXTENSION.get(extension));
    }

    private static String tokenStyleFor(Matcher matcher) {
        if (matcher.group("KEYWORD") != null) {
            return "syntax-keyword";
        }
        if (matcher.group("COMMENT") != null) {
            return "syntax-comment";
        }
        if (matcher.group("STRING") != null) {
            return "syntax-string";
        }
        if (matcher.group("NUMBER") != null) {
            return "syntax-number";
        }
        return "syntax-text";
    }

    private static Map<String, GrammarProfile> buildProfiles() {
        Map<String, GrammarProfile> profiles = new HashMap<>();

        GrammarProfile javaProfile = new GrammarProfile(
            "/grammars/source.java.tmLanguage.json",
            buildPattern(
                List.of(
                    "abstract", "assert", "boolean", "break", "byte", "case", "catch", "char",
                    "class", "const", "continue", "default", "do", "double", "else", "enum",
                    "extends", "final", "finally", "float", "for", "if", "implements", "import",
                    "instanceof", "int", "interface", "long", "native", "new", "package", "private",
                    "protected", "public", "return", "short", "static", "strictfp", "super",
                    "switch", "synchronized", "this", "throw", "throws", "transient", "try",
                    "void", "volatile", "while"
                ),
                "//[^\\n]*",
                "/\\*(.|\\R)*?\\*/",
                "\"([^\"\\\\]|\\\\.)*\"|'([^'\\\\]|\\\\.)*'"
            )
        );
        profiles.put("java", javaProfile);

        GrammarProfile cppProfile = new GrammarProfile(
            "/grammars/source.cpp.tmLanguage.json",
            buildPattern(
                List.of(
                    "alignas", "alignof", "auto", "bool", "break", "case", "catch", "char",
                    "class", "const", "constexpr", "continue", "default", "delete", "do",
                    "double", "else", "enum", "explicit", "extern", "false", "final", "float",
                    "for", "friend", "if", "inline", "int", "long", "mutable", "namespace",
                    "new", "noexcept", "nullptr", "operator", "override", "private", "protected",
                    "public", "return", "short", "signed", "sizeof", "static", "struct", "switch",
                    "template", "this", "throw", "true", "try", "typedef", "typename", "union",
                    "unsigned", "using", "virtual", "void", "volatile", "while"
                ),
                "//[^\\n]*",
                "/\\*(.|\\R)*?\\*/",
                "\"([^\"\\\\]|\\\\.)*\"|'([^'\\\\]|\\\\.)*'"
            )
        );
        profiles.put("cpp", cppProfile);
        profiles.put("cc", cppProfile);
        profiles.put("cxx", cppProfile);
        profiles.put("c", cppProfile);
        profiles.put("hpp", cppProfile);
        profiles.put("h", cppProfile);

        GrammarProfile pythonProfile = new GrammarProfile(
            "/grammars/source.python.tmLanguage.json",
            buildPattern(
                List.of(
                    "and", "as", "assert", "async", "await", "break", "class", "continue",
                    "def", "del", "elif", "else", "except", "False", "finally", "for", "from",
                    "global", "if", "import", "in", "is", "lambda", "None", "nonlocal", "not",
                    "or", "pass", "raise", "return", "True", "try", "while", "with", "yield"
                ),
                "#[^\\n]*",
                "\"\"\"(.|\\R)*?\"\"\"|'''(.|\\R)*?'''",
                "\"([^\"\\\\]|\\\\.)*\"|'([^'\\\\]|\\\\.)*'"
            )
        );
        profiles.put("py", pythonProfile);

        return profiles;
    }

    private static Pattern buildPattern(
        List<String> keywords,
        String singleLineCommentPattern,
        String multiLineCommentPattern,
        String stringPattern
    ) {
        String keywordPattern = "\\b(" + String.join("|", keywords) + ")\\b";
        String numberPattern = "\\b\\d+(\\.\\d+)?\\b";
        String tokenPattern =
            "(?<KEYWORD>" + keywordPattern + ")"
                + "|(?<COMMENT>" + singleLineCommentPattern + "|" + multiLineCommentPattern + ")"
                + "|(?<STRING>" + stringPattern + ")"
                + "|(?<NUMBER>" + numberPattern + ")";
        return Pattern.compile(tokenPattern);
    }

    private record GrammarProfile(String grammarResourcePath, Pattern tokenPattern) {
    }
}
