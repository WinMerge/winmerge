import java.util.Collection;
import java.util.Collections;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.Objects;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javafx.application.Application;
import javafx.geometry.Insets;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.ComboBox;
import javafx.scene.control.Label;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.HBox;
import javafx.stage.Stage;

import org.fxmisc.richtext.CodeArea;
import org.fxmisc.richtext.LineNumberFactory;
import org.fxmisc.richtext.model.StyleSpans;
import org.fxmisc.richtext.model.StyleSpansBuilder;

public class RichTextFxSyntaxSpikeApp extends Application {

  private static final String JAVA_SAMPLE =
      "public class Greeter {\n"
          + "  public static void main(String[] args) {\n"
          + "    int count = 3;\n"
          + "    for (int i = 0; i < count; i++) {\n"
          + "      System.out.println(\"Hello, Java \" + i);\n"
          + "    }\n"
          + "  }\n"
          + "}\n";

  private static final String CPP_SAMPLE =
      "#include <iostream>\n"
          + "\n"
          + "int main() {\n"
          + "  int count = 3;\n"
          + "  for (int i = 0; i < count; ++i) {\n"
          + "    std::cout << \"Hello, C++ \" << i << std::endl;\n"
          + "  }\n"
          + "  return 0;\n"
          + "}\n";

  private static final String PYTHON_SAMPLE =
      "def greet(count: int) -> None:\n"
          + "    for i in range(count):\n"
          + "        print(f\"Hello, Python {i}\")\n"
          + "\n"
          + "if __name__ == \"__main__\":\n"
          + "    greet(3)\n";

  private final Map<String, LanguageConfig> languages = new LinkedHashMap<>();

  @Override
  public void start(Stage stage) {
    registerLanguages();

    CodeArea codeArea = new CodeArea();
    codeArea.getStyleClass().add("code-area");
    codeArea.setParagraphGraphicFactory(LineNumberFactory.get(codeArea));
    codeArea.replaceText(languages.get("Java").sample());
    applyHighlighting(codeArea, languages.get("Java"));

    ComboBox<String> languageChooser = new ComboBox<>();
    languageChooser.getItems().addAll(languages.keySet());
    languageChooser.setValue("Java");
    languageChooser.setOnAction(
        event -> {
          String selected = languageChooser.getValue();
          LanguageConfig config = languages.get(selected);
          if (config == null) {
            return;
          }
          codeArea.replaceText(config.sample());
          applyHighlighting(codeArea, config);
        });

    codeArea.textProperty()
        .addListener(
            (obs, oldValue, newValue) -> {
              LanguageConfig config = languages.get(languageChooser.getValue());
              if (config != null) {
                applyHighlighting(codeArea, config);
              }
            });

    Button undoButton = new Button("Undo");
    undoButton.setOnAction(event -> codeArea.undo());
    Button redoButton = new Button("Redo");
    redoButton.setOnAction(event -> codeArea.redo());

    HBox toolbar = new HBox(8, new Label("Language:"), languageChooser, undoButton, redoButton);
    toolbar.getStyleClass().add("toolbar");
    toolbar.setPadding(new Insets(8));

    BorderPane root = new BorderPane();
    root.setTop(toolbar);
    root.setCenter(codeArea);

    Scene scene = new Scene(root, 980, 640);
    scene.getStylesheets()
        .add(Objects.requireNonNull(getClass().getResource("syntax-spike.css")).toExternalForm());

    stage.setTitle("RichTextFX Syntax Spike (C++, Java, Python)");
    stage.setScene(scene);
    stage.show();
  }

  private void registerLanguages() {
    languages.put(
        "Java",
        new LanguageConfig(
            buildPattern(
                new String[] {
                  "abstract", "assert", "boolean", "break", "byte", "case", "catch", "char",
                  "class", "const", "continue", "default", "do", "double", "else", "enum",
                  "extends", "final", "finally", "float", "for", "if", "implements", "import",
                  "instanceof", "int", "interface", "long", "native", "new", "package", "private",
                  "protected", "public", "return", "short", "static", "strictfp", "super",
                  "switch", "synchronized", "this", "throw", "throws", "transient", "try", "void",
                  "volatile", "while"
                },
                "//[^\\n]*",
                "/\\*(.|\\R)*?\\*/",
                "\"([^\"\\\\]|\\\\.)*\"|'([^'\\\\]|\\\\.)*'"),
            JAVA_SAMPLE));

    languages.put(
        "C++",
        new LanguageConfig(
            buildPattern(
                new String[] {
                  "alignas", "alignof", "auto", "bool", "break", "case", "catch", "char",
                  "class", "const", "constexpr", "continue", "default", "delete", "do", "double",
                  "else", "enum", "explicit", "extern", "false", "final", "float", "for", "friend",
                  "if", "inline", "int", "long", "mutable", "namespace", "new", "noexcept",
                  "nullptr", "operator", "override", "private", "protected", "public", "return",
                  "short", "signed", "sizeof", "static", "struct", "switch", "template", "this",
                  "throw", "true", "try", "typedef", "typename", "union", "unsigned", "using",
                  "virtual", "void", "volatile", "while"
                },
                "//[^\\n]*",
                "/\\*(.|\\R)*?\\*/",
                "\"([^\"\\\\]|\\\\.)*\"|'([^'\\\\]|\\\\.)*'"),
            CPP_SAMPLE));

    languages.put(
        "Python",
        new LanguageConfig(
            buildPattern(
                new String[] {
                  "and", "as", "assert", "async", "await", "break", "class", "continue", "def",
                  "del", "elif", "else", "except", "False", "finally", "for", "from", "global",
                  "if", "import", "in", "is", "lambda", "None", "nonlocal", "not", "or", "pass",
                  "raise", "return", "True", "try", "while", "with", "yield"
                },
                "#[^\\n]*",
                "\"\"\"(.|\\R)*?\"\"\"|'''(.|\\R)*?'''",
                "\"([^\"\\\\]|\\\\.)*\"|'([^'\\\\]|\\\\.)*'"),
            PYTHON_SAMPLE));
  }

  private static Pattern buildPattern(
      String[] keywords, String singleLineComment, String multiLineComment, String stringPattern) {
    String keywordPattern = "\\b(" + String.join("|", keywords) + ")\\b";
    String numberPattern = "\\b\\d+(\\.\\d+)?\\b";
    String combinedPattern =
        "(?<KEYWORD>"
            + keywordPattern
            + ")"
            + "|(?<COMMENT>"
            + singleLineComment
            + "|"
            + multiLineComment
            + ")"
            + "|(?<STRING>"
            + stringPattern
            + ")"
            + "|(?<NUMBER>"
            + numberPattern
            + ")";
    return Pattern.compile(combinedPattern);
  }

  private static void applyHighlighting(CodeArea codeArea, LanguageConfig config) {
    int caret = codeArea.getCaretPosition();
    codeArea.setStyleSpans(0, computeHighlighting(codeArea.getText(), config.pattern()));
    codeArea.moveTo(Math.min(caret, codeArea.getLength()));
  }

  private static StyleSpans<Collection<String>> computeHighlighting(String text, Pattern pattern) {
    Matcher matcher = pattern.matcher(text);
    int last = 0;
    StyleSpansBuilder<Collection<String>> builder = new StyleSpansBuilder<>();

    while (matcher.find()) {
      String styleClass = styleFor(matcher);
      builder.add(Collections.emptyList(), matcher.start() - last);
      builder.add(Collections.singleton(styleClass), matcher.end() - matcher.start());
      last = matcher.end();
    }

    builder.add(Collections.emptyList(), text.length() - last);
    return builder.create();
  }

  private static String styleFor(Matcher matcher) {
    if (matcher.group("KEYWORD") != null) {
      return "keyword";
    }
    if (matcher.group("COMMENT") != null) {
      return "comment";
    }
    if (matcher.group("STRING") != null) {
      return "string";
    }
    if (matcher.group("NUMBER") != null) {
      return "number";
    }
    return "";
  }

  public static void main(String[] args) {
    launch(args);
  }

  private record LanguageConfig(Pattern pattern, String sample) {}
}
