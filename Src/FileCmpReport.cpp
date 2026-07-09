/**
 * @file  FileCmpReport.cpp
 *
 * @brief Implementation of CFileCmpReport class for HTML report generation
 */

#include "StdAfx.h"
#include "FileCmpReport.h"
#include "UnicodeString.h"
#include "paths.h"
#include "MergeEditView.h"
#include "Merge.h"
#include "MergeApp.h"
#include "OptionsMgr.h"
#include "OptionsDef.h"

static IMergeDoc* GetFirstDocument(const std::vector<IMergeDoc*>& mergeDocuments, IMergeDoc::DocumentType docType)
{
	for (auto& doc : mergeDocuments)
	{
		if (doc && doc->GetDocumentType() == docType)
			return doc;
	}
	return nullptr;
}

/**
 * @brief Write HTML header to file
 * @param [in,out] file UniStdioFile to write header to
 */
void CFileCmpReport::WriteHeader(const std::vector<IMergeDoc*>& mergeDocuments, UniStdioFile& file)
{
	auto* pTextDoc = GetFirstDocument(mergeDocuments, IMergeDoc::DocumentType::Text);
	auto* pTableDoc = GetFirstDocument(mergeDocuments, IMergeDoc::DocumentType::Table);
	auto* pImageDoc = GetFirstDocument(mergeDocuments, IMergeDoc::DocumentType::Image);

	// calculate HTML font size
	CDC dc;
	dc.CreateDC(_T("DISPLAY"), nullptr, nullptr, nullptr);
	int nFontSize = -MulDiv(theApp.m_lfDiff.lfHeight, 72, dc.GetDeviceCaps(LOGPIXELSY));
	bool bDarkMode = (GetOptionsMgr()->GetInt(OPT_COLOR_MODE_EFFECTIVE) == 1);

	String htmlHeader = strutils::format(
LR"(
<!DOCTYPE html>
<html%s>
<head>
<meta charset=\"UTF-8\">
<title>WinMerge Compare Report</title>
<style>
<!--
:root {
	--bg: #ffffff;
	--fg: #1a1a1a;
	--border: #a0a0a0;
	--surface: #e8e8e8;
	--title-grad-start: mediumblue;
	--title-grad-end: darkblue;
	--title-fg: #ffffff;
	--toc-bg: #f0f4f8;
	--toc-border: #d0dae3;
	--link: #0353a4;
	--fab-bg: #003366;
	--fab-fg: #ffffff;
	--section-border: #d0d0d0;
	--collapsed-cell-bg: #000000;
	--shadow: rgba(0, 0, 0, 0.3);
}
html[data-theme="dark"] {
	--bg: #1e1e1e;
	--fg: #e0e0e0;
	--border: #555555;
	--surface: #2a2a2a;
	--title-grad-start: #2a4d9b;
	--title-grad-end: #16265c;
	--title-fg: #ffffff;
	--toc-bg: #2a2f36;
	--toc-border: #3a4048;
	--link: #7fb2f0;
	--fab-bg: #1f4e79;
	--fab-fg: #ffffff;
	--section-border: #3a3a3a;
	--collapsed-cell-bg: #ffffff;
	--shadow: rgba(0, 0, 0, 0.6);
}
body { font-family: "Segoe UI", Arial, sans-serif; margin: 20px; background-color: var(--bg); color: var(--fg); }
table { table-layout: fixed; margin: 0; border: none; box-shadow: 0px 0px 3px 1px rgba(0, 0, 0, 0.15); font-size: %dpt; }
.cmp-table-text td { word-break: break-all; padding: 0 3px; vertical-align: top; }
.cmp-table-table td, .cmp-table-table th { word-break: break-all; padding: 0 3px; border: 1px solid var(--border); vertical-align: top; }
.cmp-table-image td, .cmp-table-webpage td { border: 1px solid var(--border); }
.ln { position: sticky; left: 0; background-color: var(--surface); }
.title { font-weight: 600; color: var(--title-fg); text-align: left; padding: 8px 12px; background: linear-gradient(var(--title-grad-start), var(--title-grad-end)); border-bottom: none; position: sticky; vertical-align: top; text-align: center; top: 0; z-index: 9999; }
.title-right, .title-middle { box-shadow: inset 1px 0 var(--bg); }
.cmp-div-image { overflow: scroll; text-align: center; }
.cmp-pdf-webpage { width: 100%%; height: calc(100vh - 56px) }
.cmp-table-full { width: 100%%; border-collapse: collapse; overflow: hidden; }
.cmp-table-auto { width: max-content; border-collapse: collapse; overflow: hidden; }
.cmp-table-full .cmp-table-fill-height { height: 100%%; }
.cmp-table-header { position: sticky; top: 0; z-index: 99; }
.cmp-grid { display: grid; grid-template-rows: max-content; height: calc(100vh - 16px); }
.cmp-grid-2 { grid-template-columns: 50%% 50%%; }
.cmp-grid-3 { grid-template-columns: 33.33%% 33.33%% 33.33%%; }
.cmp-collapsed-row { height: 1px; }
.cmp-collapsed-cell { background-color: var(--collapsed-cell-bg); }
.cmp-scroll { overflow-x: auto; }
)"
		, bDarkMode ? _T(" data-theme=\"dark\"") : _T(""), nFontSize);

	if (mergeDocuments.size() > 1)
	{
		htmlHeader += LR"(
#toc { background-color: var(--toc-bg); border: 1px solid var(--toc-border); border-radius: 6px; padding: 12px 20px; margin: 0 0 24px 0; }
#toc ol { margin: 0; padding-left: 20px; }
#toc a { color: var(--link); text-decoration: none; }
#toc a:hover { text-decoration: underline; }
#toc-sentinel { height: 1px; }
#toc-fab { display: none; position: fixed; top: calc(var(--title-height, 40px) + 8px); left: 20px; z-index: 10000; width: 40px; height: 40px; border-radius: 50%; background-color: var(--fab-bg); color: var(--fab-fg); border: none; box-shadow: 0 2px 6px var(--shadow); cursor: pointer; font-size: 18px; line-height: 40px; padding: 0; }
#toc-fab.visible { display: block; }
#toc-fab-panel { display: none; position: fixed; top: calc(var(--title-height, 40px) + 54px); left: 20px; z-index: 10000; background-color: var(--toc-bg); border: 1px solid var(--toc-border); border-radius: 6px; padding: 12px 20px; max-height: 60vh; overflow-y: auto; box-shadow: 0 2px 6px var(--shadow); }
#toc-fab-panel.open { display: block; }
#toc-fab-panel ol { margin: 0; padding-left: 20px; }
#toc-fab-panel a { color: var(--link); text-decoration: none; }
#toc-fab-panel a:hover { text-decoration: underline; }
.report-section { margin: 0 0 32px 0; padding-bottom: 24px; border-bottom: 1px solid var(--section-border); }
.report-section:last-of-type { margin-bottom: 0; padding-bottom: 0; border-bottom: none; }
)";
	}
	if (pTextDoc || pTableDoc)
	{
		auto* pDoc = pTextDoc ? pTextDoc : pTableDoc;
		htmlHeader += dynamic_cast<CMergeDoc*>(pDoc)->GetView(0, 0)->GetHTMLStyles();
	}
	htmlHeader += _T("--></style>\n");
	if (pTableDoc || pImageDoc)
	{
		htmlHeader += LR"(
<script>
<!--
document.addEventListener("DOMContentLoaded", () => {
	const syncing = new WeakSet();
	document.querySelectorAll("[data-group]").forEach(elem => {
		elem.addEventListener("scroll", () => {
			if (syncing.has(elem))
				return;
			const group = elem.dataset.group;
			document.querySelectorAll(`[data-group="${group}"]`).forEach(other => {
				if (other === elem)
					return;
				syncing.add(other);
				other.scrollTop = elem.scrollTop;
				other.scrollLeft = elem.scrollLeft;
				requestAnimationFrame(() => syncing.delete(other));
			});
		});
	});
});
-->
</script>
)";
	}
	if (mergeDocuments.size() > 1)
	{
		htmlHeader += LR"(
<script>
<!--
document.addEventListener("DOMContentLoaded", () => {
	const list = document.getElementById("toc-list");
	document.querySelectorAll("section.report-section").forEach((section, i) => {
		const titles = [...section.querySelectorAll(".title")]
			.map(elem => elem.textContent.trim())
			.filter(text => text.length > 0);
		const label = titles.length ? titles.join(" - ") : `Comparison ${i + 1}`;
		const li = document.createElement("li");
		const a = document.createElement("a");
		a.href = `#${section.id}`;
		a.textContent = label;
		li.appendChild(a);
		list.appendChild(li);
	});

	const sentinel = document.getElementById("toc-sentinel");
	const fab = document.getElementById("toc-fab");
	const fabPanel = document.getElementById("toc-fab-panel");
	fabPanel.appendChild(list.cloneNode(true));

	const titleElem = document.querySelector(".title");
	if (titleElem)
	{
		const setTitleHeight = () => document.documentElement.style.setProperty(
			"--title-height", `${titleElem.getBoundingClientRect().height}px`);
		setTitleHeight();
		window.addEventListener("resize", setTitleHeight);
	}

	new IntersectionObserver(([entry]) => {
		fab.classList.toggle("visible", !entry.isIntersecting);
		if (entry.isIntersecting)
			fabPanel.classList.remove("open");
	}, { threshold: 0 }).observe(sentinel);

	fab.addEventListener("click", () => fabPanel.classList.toggle("open"));
	fabPanel.addEventListener("click", (e) => {
		if (e.target.tagName === "A")
			fabPanel.classList.remove("open");
	});
});

-->
</script>
)";
	}
	htmlHeader += _T("</head><body>\n");
	if (mergeDocuments.size() > 1)
	{
		htmlHeader += LR"(
<div id="toc-sentinel"></div>
<nav id="toc">
<ol id="toc-list"></ol>
</nav>
<button id="toc-fab" type="button" aria-label="Show table of contents">&#9776;</button>
<div id="toc-fab-panel"></div>
)";
	}
	file.WriteString(htmlHeader);
}

/**
 * @brief Write HTML footer to file
 * @param [in,out] file UniStdioFile to write footer to
 */
void CFileCmpReport::WriteFooter(UniStdioFile& file)
{
	// Write HTML footer
	file.WriteString(_T("</body>\n</html>\n"));
}

/**
 * @brief Generate a unified HTML report from multiple documents
 * @param [in] mergeDocuments Vector of merge documents to include
 * @param [in] sFileName Output HTML file path
 * @return true if report was generated successfully
 */
bool CFileCmpReport::GenerateDocumentReport(const std::vector<IMergeDoc*>& mergeDocuments,
												 const String& sFileName)
{
	if (mergeDocuments.empty())
		return false;

	UniStdioFile file;
	if (!file.Open(sFileName, _T("wt")))
	{
		String errMsg = GetSysError(GetLastError());
		String msg = strutils::format_string1(
			_("Error creating the report:\n%1"), errMsg);
		AfxMessageBox(msg.c_str(), MB_OK | MB_ICONSTOP);
		return false;
	}

	file.SetCodepage(ucr::CP_UTF_8);

	String outputDirectory = paths::RemoveExtension(sFileName) + _T(".files");

	ReportContext reportContext(file, true, outputDirectory);

	WriteHeader(mergeDocuments, file);

	int i = 0;
	// Generate report for each document
	for (auto pMergeDoc : mergeDocuments)
	{
		if (pMergeDoc == nullptr)
			continue;

		reportContext.index = i;

		if (mergeDocuments.size() > 1)
		{
			file.WriteString(strutils::format(_T("<section id=\"doc%d\" class=\"report-section\">\n"), i));
			// Generate HTML content from the document
			pMergeDoc->GenerateReport(reportContext);
			file.WriteString(_T("</section>\n"));
		}
		else
		{
			pMergeDoc->GenerateReport(reportContext);
		}

		++i;
	}

	WriteFooter(file);

	file.Close();
	return true;
}
