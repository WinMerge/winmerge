package org.winmerge.migration;

import static org.junit.jupiter.api.Assertions.assertEquals;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import java.io.IOException;
import java.io.InputStream;
import java.util.Iterator;
import org.junit.jupiter.api.Disabled;
import org.junit.jupiter.api.Test;

class DiffBaselineParityTest {
  private final ObjectMapper mapper = new ObjectMapper();

  @Test
  @Disabled("Enable in phase 2b after Java diff engine implementation is wired.")
  void javaDiffEngineMatchesGoldenBaseline() throws Exception {
    JsonNode golden = loadJsonResource("/golden-baseline.sample.json");
    JsonNode cases = golden.path("cases");

    JavaDiffEngine engine = new PlaceholderDiffEngine();
    for (Iterator<JsonNode> it = cases.elements(); it.hasNext(); ) {
      JsonNode c = it.next();
      String caseId = c.path("id").asText();
      int expectedExitCode = c.path("exit_code").asInt();

      int actualExitCode = engine.compare(caseId);
      assertEquals(expectedExitCode, actualExitCode, "Mismatch for corpus case: " + caseId);
    }
  }

  private JsonNode loadJsonResource(String resourcePath) throws IOException {
    try (InputStream stream = getClass().getResourceAsStream(resourcePath)) {
      if (stream == null) {
        throw new IOException("Missing resource: " + resourcePath);
      }
      return mapper.readTree(stream);
    }
  }

  interface JavaDiffEngine {
    int compare(String caseId) throws Exception;
  }

  static class PlaceholderDiffEngine implements JavaDiffEngine {
    @Override
    public int compare(String caseId) {
      throw new UnsupportedOperationException(
          "Wire Java diff engine adapter for case: " + caseId);
    }
  }
}
