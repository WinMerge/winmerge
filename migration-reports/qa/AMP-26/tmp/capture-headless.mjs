import path from "node:path";
import { chromium } from "playwright";

const root = process.cwd();
const baseDir = process.env.AMP26_BASE_DIR;
const outDir = path.join(root, "migration-reports/qa/AMP-26");

if (!baseDir) {
  throw new Error("AMP26_BASE_DIR environment variable is required.");
}

const flows = [
  {
    name: "Baseline summary report",
    source: `file://${path.join(baseDir, "winmerge-core/build/reports/tests/test/index.html")}`,
    screenshot: "pre-a3dfd4d40-summary.png",
  },
  {
    name: "Baseline diff-engine suite details",
    source: `file://${path.join(baseDir, "winmerge-core/build/reports/tests/test/classes/org.winmerge.core.diff.DiffEngineTest.html")}`,
    screenshot: "pre-a3dfd4d40-diff-engine.png",
  },
  {
    name: "Candidate summary report",
    source: `file://${path.join(root, "winmerge-core/build/reports/tests/test/index.html")}`,
    screenshot: "post-5162a4d73-summary.png",
  },
  {
    name: "Candidate diff-engine suite details",
    source: `file://${path.join(root, "winmerge-core/build/reports/tests/test/classes/org.winmerge.core.diff.DiffEngineTest.html")}`,
    screenshot: "post-5162a4d73-diff-engine.png",
  },
  {
    name: "Candidate corpus parity suite details",
    source: `file://${path.join(root, "winmerge-core/build/reports/tests/test/classes/org.winmerge.core.diff.DiffBaselineParityTest.html")}`,
    screenshot: "post-5162a4d73-diff-baseline-parity.png",
  },
];

const browser = await chromium.launch({ headless: true });
const page = await browser.newPage({ viewport: { width: 1600, height: 1000 } });

for (const flow of flows) {
  await page.goto(flow.source, { waitUntil: "domcontentloaded" });
  await page.screenshot({ path: path.join(outDir, flow.screenshot), fullPage: true });
}

await browser.close();

console.log(JSON.stringify({
  issue: "AMP-26",
  tier: "Standard",
  tool: "playwright",
  mode: "headless",
  captured_at: new Date().toISOString().slice(0, 10),
  flows: flows.map((flow) => ({
    name: flow.name,
    source: flow.source,
    screenshot: path.join("migration-reports/qa/AMP-26", flow.screenshot),
  })),
}, null, 2));
