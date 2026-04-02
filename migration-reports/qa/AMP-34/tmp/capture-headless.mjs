import { chromium } from 'playwright';
import fs from 'node:fs';
import path from 'node:path';

const root = process.cwd();
const outDir = path.join(root, 'migration-reports/qa/AMP-34');
const tmpDir = path.join(outDir, 'tmp');

const baselineCommit = '2d8222b20';
const candidateLabel = 'workspace-amp34';

const logFlows = [
  {
    name: `Baseline clean compile gate (${baselineCommit})`,
    source: 'file://' + path.join(tmpDir, 'pre-2d8222b20-compile-log.html'),
    screenshot: `pre-${baselineCommit}-compile-gate.png`
  },
  {
    name: `Candidate compile gate (${candidateLabel})`,
    source: 'file://' + path.join(tmpDir, 'post-workspace-amp34-compile-log.html'),
    screenshot: `post-${candidateLabel}-compile-gate.png`
  },
  {
    name: `Candidate targeted migrated-flow tests (${candidateLabel})`,
    source: 'file://' + path.join(tmpDir, 'post-workspace-amp34-targeted-tests-log.html'),
    screenshot: `post-${candidateLabel}-targeted-tests-log.png`
  }
];

const reportIndex = 'file://' + path.join(root, 'winmerge-desktop/build/reports/tests/test/index.html');

const classFlows = [
  {
    name: 'FilterCondition dialog logic flow',
    classFile: 'org.winmerge.desktop.ui.dialogs.FilterConditionDialogControllerTest.html',
    screenshot: `post-${candidateLabel}-filter-condition-dialog.png`
  },
  {
    name: 'FileFilters model flow',
    classFile: 'org.winmerge.desktop.ui.dialogs.FileFiltersDialogModelTest.html',
    screenshot: `post-${candidateLabel}-file-filters-model.png`
  },
  {
    name: 'SharedFilter selection flow',
    classFile: 'org.winmerge.desktop.ui.dialogs.SharedFilterDialogTest.html',
    screenshot: `post-${candidateLabel}-shared-filter.png`
  },
  {
    name: 'LineFilters model flow',
    classFile: 'org.winmerge.desktop.ui.dialogs.LineFiltersDialogModelTest.html',
    screenshot: `post-${candidateLabel}-line-filters.png`
  },
  {
    name: 'SubstitutionFilters model flow',
    classFile: 'org.winmerge.desktop.ui.dialogs.SubstitutionFiltersDialogModelTest.html',
    screenshot: `post-${candidateLabel}-substitution-filters.png`
  },
  {
    name: 'FilterSettings integration flow',
    classFile: 'org.winmerge.desktop.ui.FilterSettingsDialogFxIntegrationTest.html',
    screenshot: `post-${candidateLabel}-filter-settings-integration.png`
  },
  {
    name: 'DialogService routing continuity',
    classFile: 'org.winmerge.desktop.ui.TabManagerRoutingTest.html',
    screenshot: `post-${candidateLabel}-tab-routing.png`
  },
  {
    name: 'Save/close session continuity',
    classFile: 'org.winmerge.desktop.ui.SaveClosingHandlerTest.html',
    screenshot: `post-${candidateLabel}-save-closing.png`
  }
];

const browser = await chromium.launch({ headless: true });
const context = await browser.newContext({ viewport: { width: 1720, height: 1080 } });
const page = await context.newPage();

const captured = [];

for (const flow of logFlows) {
  await page.goto(flow.source, { waitUntil: 'networkidle' });
  await page.screenshot({ path: path.join(outDir, flow.screenshot), fullPage: true });
  captured.push({ ...flow, screenshot: path.join('migration-reports/qa/AMP-34', flow.screenshot) });
}

await page.goto(reportIndex, { waitUntil: 'networkidle' });
const summaryShot = `post-${candidateLabel}-summary.png`;
await page.screenshot({ path: path.join(outDir, summaryShot), fullPage: true });
captured.push({
  name: 'Candidate desktop test summary',
  source: reportIndex,
  screenshot: path.join('migration-reports/qa/AMP-34', summaryShot)
});

for (const flow of classFlows) {
  const href = `classes/${flow.classFile}`;
  const selector = `a[href="${href}"]`;
  if (await page.locator(selector).count()) {
    await Promise.all([
      page.waitForLoadState('networkidle'),
      page.click(selector)
    ]);
  } else {
    await page.goto('file://' + path.join(root, 'winmerge-desktop/build/reports/tests/test/classes', flow.classFile), {
      waitUntil: 'networkidle'
    });
  }
  await page.screenshot({ path: path.join(outDir, flow.screenshot), fullPage: true });
  captured.push({
    name: flow.name,
    source: page.url(),
    screenshot: path.join('migration-reports/qa/AMP-34', flow.screenshot)
  });
  await page.goto(reportIndex, { waitUntil: 'networkidle' });
}

await browser.close();

const testTotals = JSON.parse(fs.readFileSync(path.join(tmpDir, 'test-totals.json'), 'utf8'));
const baseStatusText = fs.readFileSync(path.join(tmpDir, 'baseline-command-status.env'), 'utf8');
const candStatusText = fs.readFileSync(path.join(tmpDir, 'candidate-command-status.env'), 'utf8');

const parseEnv = (text) => Object.fromEntries(text.trim().split('\n').map((line) => line.split('=')));

const payload = {
  issue: 'AMP-34',
  tier: 'Exhaustive',
  tool: 'playwright',
  mode: 'headless',
  captured_at: new Date().toISOString().slice(0, 10),
  flows: captured,
  results: {
    candidate_status: parseEnv(candStatusText),
    baseline_status: parseEnv(baseStatusText),
    migrated_flow_tests: testTotals.migrated_flow_tests,
    candidate_desktop_totals: testTotals.candidate_desktop_totals,
    candidate_all_modules_totals: testTotals.candidate_all_modules_totals
  }
};

console.log(JSON.stringify(payload, null, 2));
