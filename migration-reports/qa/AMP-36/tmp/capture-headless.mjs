import { chromium } from 'playwright';
import path from 'node:path';

const root = process.cwd();
const outDir = path.join(root, 'migration-reports/qa/AMP-36');

const flows = [
  {
    name: 'Baseline clean compile gate (24b276ea5)',
    source: 'file://' + path.join(root, 'migration-reports/qa/AMP-36/tmp/pre-24b276ea5-compile-log.html'),
    screenshot: 'pre-24b276ea5-compile-gate.png'
  },
  {
    name: 'Candidate clean compile gate (347f6a2bc)',
    source: 'file://' + path.join(root, 'migration-reports/qa/AMP-36/tmp/post-347f6a2bc-compile-log.html'),
    screenshot: 'post-347f6a2bc-compile-gate.png'
  },
  {
    name: 'Candidate workspace test summary',
    source: 'file://' + path.join(root, 'winmerge-desktop/build/reports/tests/test/index.html'),
    screenshot: 'post-347f6a2bc-summary.png'
  },
  {
    name: 'Codepage dialog regression tests',
    source: 'file://' + path.join(root, 'winmerge-desktop/build/reports/tests/test/classes/org.winmerge.desktop.ui.dialogs.CodepageDialogTest.html'),
    screenshot: 'post-347f6a2bc-codepage-dialog.png'
  },
  {
    name: 'Open-table dialog regression tests',
    source: 'file://' + path.join(root, 'winmerge-desktop/build/reports/tests/test/classes/org.winmerge.desktop.ui.dialogs.OpenTableDialogTest.html'),
    screenshot: 'post-347f6a2bc-open-table-dialog.png'
  },
  {
    name: 'Patch dialog regression tests',
    source: 'file://' + path.join(root, 'winmerge-desktop/build/reports/tests/test/classes/org.winmerge.desktop.ui.dialogs.PatchDialogTest.html'),
    screenshot: 'post-347f6a2bc-patch-dialog.png'
  },
  {
    name: 'Save-closing routing regression tests',
    source: 'file://' + path.join(root, 'winmerge-desktop/build/reports/tests/test/classes/org.winmerge.desktop.ui.SaveClosingHandlerTest.html'),
    screenshot: 'post-347f6a2bc-save-closing.png'
  },
  {
    name: 'Tab-manager routing regression tests',
    source: 'file://' + path.join(root, 'winmerge-desktop/build/reports/tests/test/classes/org.winmerge.desktop.ui.TabManagerRoutingTest.html'),
    screenshot: 'post-347f6a2bc-tab-routing.png'
  }
];

const browser = await chromium.launch({ headless: true });
const context = await browser.newContext({ viewport: { width: 1720, height: 1080 } });
const page = await context.newPage();

for (const flow of flows) {
  await page.goto(flow.source, { waitUntil: 'networkidle' });
  await page.screenshot({ path: path.join(outDir, flow.screenshot), fullPage: true });
}

await browser.close();

const payload = {
  issue: 'AMP-36',
  tier: 'Standard',
  tool: 'playwright',
  mode: 'headless',
  captured_at: new Date().toISOString().slice(0, 10),
  flows: flows.map((flow) => ({
    name: flow.name,
    source: flow.source,
    screenshot: path.join('migration-reports/qa/AMP-36', flow.screenshot)
  }))
};

console.log(JSON.stringify(payload, null, 2));
