package org.winmerge.migration;

import static org.junit.jupiter.api.Assertions.assertEquals;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.stream.Stream;
import org.junit.jupiter.api.Disabled;
import org.junit.jupiter.api.DynamicTest;
import org.junit.jupiter.api.TestFactory;

/**
 * Parity test harness that validates the Java diff engine produces the same
 * results as the golden baseline captured from GNU diff / WinMerge.
 *
 * <p>Enable once the Java diff engine is wired (AMP-7+). Each corpus case
 * becomes an individual dynamic test for clear pass/fail reporting.
 */
class DiffBaselineParityTest {
  private final ObjectMapper mapper = new ObjectMapper();

  @TestFactory
  @Disabled("Enable in phase 2b after Java diff engine implementation is wired.")
  Stream<DynamicTest> javaDiffEngineMatchesGoldenBaseline() throws Exception {
    JsonNode golden = loadJsonResource("/golden-baseline.sample.json");
    JsonNode cases = golden.path("cases");
    JavaDiffEngine engine = new PlaceholderDiffEngine();

    List<DynamicTest> tests = new ArrayList<>();
    for (Iterator<JsonNode> it = cases.elements(); it.hasNext(); ) {
      JsonNode c = it.next();
      String caseId = c.path("id").asText();
      String expectedComparison = c.path("comparison").asText();
      int expectedExitCode = c.path("exit_code").asInt();

      tests.add(DynamicTest.dynamicTest("parity:" + caseId, () -> {
        DiffResult result = engine.compare(caseId);
        assertEquals(expectedComparison, result.comparison(),
            "Comparison mismatch for corpus case: " + caseId);
        assertEquals(expectedExitCode, result.exitCode(),
            "Exit code mismatch for corpus case: " + caseId);
      }));
    }
    return tests.stream();
  }

  private JsonNode loadJsonResource(String resourcePath) throws IOException {
    try (InputStream stream = getClass().getResourceAsStream(resourcePath)) {
      if (stream == null) {
        throw new IOException("Missing resource: " + resourcePath);
      }
      return mapper.readTree(stream);
    }
  }

  record DiffResult(String comparison, int exitCode) {}

  interface JavaDiffEngine {
    DiffResult compare(String caseId) throws Exception;
  }

  static class PlaceholderDiffEngine implements JavaDiffEngine {
    @Override
    public DiffResult compare(String caseId) {
      throw new UnsupportedOperationException(
          "Wire Java diff engine adapter for case: " + caseId);
    }
  }
}
