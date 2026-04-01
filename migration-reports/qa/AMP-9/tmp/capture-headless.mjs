import path from 'node:path';
import { chromium } from 'playwright';

const root = '/Users/pddstudio/arconsis/migration-projects/winmerge-paperclip';
const outDir = path.join(root, 'migration-reports/qa/AMP-9');
const baselineSummary = 'file://' + path.join(root, 'migration-reports/qa/AMP-9/tmp/baseline-summary.html');
const candidateRoot = '/tmp/winmerge-qa-amp9-cand-SfPrBi/winmerge-plugins/build/reports/tests/test';

const flows = [
  {
    name: 'Baseline summary report',
    source: baselineSummary,
    screenshot: 'pre-6300aa210-summary.png'
  },
  {
    name: 'Candidate summary report',
    source: 'file://' + path.join(candidateRoot, 'index.html'),
    screenshot: 'post-beeb2520b-summary.png'
  },
  {
    name: 'Candidate plugin bridge flow',
    source: 'file://' + path.join(candidateRoot, 'classes/org.winmerge.plugins.WinMergePluginBridgeTest.html'),
    screenshot: 'post-beeb2520b-plugin-bridge.png'
  },
  {
    name: 'Candidate archive plugin flow',
    source: 'file://' + path.join(candidateRoot, 'classes/org.winmerge.plugins.builtin.CommonsCompressArchivePluginTest.html'),
    screenshot: 'post-beeb2520b-archive-plugin.png'
  }
];

const browser = await chromium.launch({ headless: true });
const page = await browser.newPage({ viewport: { width: 1600, height: 1000 } });

for (const flow of flows) {
  await page.goto(flow.source, { waitUntil: 'domcontentloaded' });
  await page.screenshot({ path: path.join(outDir, flow.screenshot), fullPage: true });
}

await browser.close();

console.log(JSON.stringify({
  issue: 'AMP-9',
  tool: 'playwright',
  mode: 'headless',
  flows: flows.map(flow => ({ name: flow.name, source: flow.source, screenshot: path.join('migration-reports/qa/AMP-9', flow.screenshot) }))
}, null, 2));
