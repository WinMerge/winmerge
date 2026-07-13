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
#include "Clipboard.h"
#include "UniFile.h"
#include "MergeDoc.h"

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
static void WriteHeader(const std::vector<IMergeDoc*>& mergeDocuments, const CFileCmpReport::Options& options, UniStdioFile& file)
{
	auto* pTextDoc = GetFirstDocument(mergeDocuments, IMergeDoc::DocumentType::Text);
	auto* pTableDoc = GetFirstDocument(mergeDocuments, IMergeDoc::DocumentType::Table);
	auto* pImageDoc = GetFirstDocument(mergeDocuments, IMergeDoc::DocumentType::Image);

	// Light mode colors
	const wchar_t* bgColor = _T("#ffffff");
	const wchar_t* fgColor = _T("#1a1a1a");
	const wchar_t* borderColor = _T("#a0a0a0");
	const wchar_t* surfaceColor = _T("#e8e8e8");
	const wchar_t* titleGradStart = _T("mediumblue");
	const wchar_t* titleGradEnd = _T("darkblue");
	const wchar_t* titleFgColor = _T("#ffffff");
	const wchar_t* tocBgColor = _T("#f0f4f8");
	const wchar_t* tocBorderColor = _T("#d0dae3");
	const wchar_t* linkColor = _T("#0353a4");
	const wchar_t* fabBgColor = _T("#003366");
	const wchar_t* fabFgColor = _T("#ffffff");
	const wchar_t* sectionBorderColor = _T("#d0d0d0");
	const wchar_t* collapsedCellBgColor = _T("#000000");
	const wchar_t* shadowColor = _T("rgba(0, 0, 0, 0.3)");

	// Dark mode colors
	if (options.darkMode)
	{
		bgColor = _T("#1e1e1e");
		fgColor = _T("#e0e0e0");
		borderColor = _T("#555555");
		surfaceColor = _T("#2a2a2a");
		titleGradStart = _T("#2a4d9b");
		titleGradEnd = _T("#16265c");
		titleFgColor = _T("#ffffff");
		tocBgColor = _T("#2a2f36");
		tocBorderColor = _T("#3a4048");
		linkColor = _T("#7fb2f0");
		fabBgColor = _T("#1f4e79");
		fabFgColor = _T("#ffffff");
		sectionBorderColor = _T("#3a3a3a");
		collapsedCellBgColor = _T("#ffffff");
		shadowColor = _T("rgba(0, 0, 0, 0.6)");
	}

	String htmlHeader = strutils::format(
LR"(<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>WinMerge Compare Report</title>
<style>
<!--
body { font-family: "Segoe UI", Arial, sans-serif; margin: 20px; background-color: %s; color: %s; }
table { table-layout: fixed; margin: 0; border: none; box-shadow: 0px 0px 3px 1px rgba(0, 0, 0, 0.15); font-size: %.2fpt; }
.cmp-table-text td { word-break: break-all; padding: 0 3px; vertical-align: top; }
.cmp-table-table td, .cmp-table-table th { word-break: break-all; padding: 0 3px; border: 1px solid %s; vertical-align: top; }
.cmp-table-image td, .cmp-table-webpage td { border: 1px solid %s; }
.ln { position: sticky; left: 0; background-color: %s; }
.title { font-weight: 600; color: %s; text-align: left; padding: 8px 12px; background: linear-gradient(%s, %s); border-bottom: none; position: sticky; vertical-align: top; text-align: center; top: 0; z-index: 9999; }
.title-right, .title-middle { box-shadow: inset 1px 0 %s; }
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
.cmp-collapsed-cell { background-color: %s; }
.cmp-scroll { overflow-x: auto; }
)"
		, bgColor, fgColor, options.fontSize, borderColor, borderColor, surfaceColor, titleFgColor, 
		  titleGradStart, titleGradEnd, bgColor, collapsedCellBgColor);

	if (mergeDocuments.size() > 1)
	{
		htmlHeader += strutils::format(
			_T("#toc { background-color: %s; border: 1px solid %s; border-radius: 6px; padding: 12px 20px; margin: 0 0 24px 0; }\n")
			_T("#toc ol { margin: 0; padding-left: 20px; }\n")
			_T("#toc a { color: %s; text-decoration: none; }\n")
			_T("#toc a:hover { text-decoration: underline; }\n")
			_T("#toc-sentinel { height: 1px; }\n")
			_T("#toc-fab { display: none; position: fixed; top: 48px; left: 20px; z-index: 10000; width: 40px; height: 40px; border-radius: 50%%; background-color: %s; color: %s; border: none; box-shadow: 0 2px 6px %s; cursor: pointer; font-size: 18px; line-height: 40px; padding: 0; }\n")
			_T("#toc-fab.visible { display: block; }\n")
			_T("#toc-fab-panel { display: none; position: fixed; top: 102px; left: 20px; z-index: 10000; background-color: %s; border: 1px solid %s; border-radius: 6px; padding: 12px 20px; max-height: 60vh; overflow-y: auto; box-shadow: 0 2px 6px %s; }\n")
			_T("#toc-fab-panel.open { display: block; }\n")
			_T("#toc-fab-panel ol { margin: 0; padding-left: 20px; }\n")
			_T("#toc-fab-panel a { color: %s; text-decoration: none; }\n")
			_T("#toc-fab-panel a:hover { text-decoration: underline; }\n")
			_T(".report-section { margin: 0 0 32px 0; padding-bottom: 24px; border-bottom: 1px solid %s; }\n")
			_T(".report-section:last-of-type { margin-bottom: 0; padding-bottom: 0; border-bottom: none; }\n"),
			tocBgColor, tocBorderColor, linkColor, fabBgColor, fabFgColor, shadowColor,
			tocBgColor, tocBorderColor, shadowColor, linkColor, sectionBorderColor);
	}
	if (pTextDoc || pTableDoc)
	{
		auto* pDoc = pTextDoc ? pTextDoc : pTableDoc;
		htmlHeader += dynamic_cast<CMergeDoc*>(pDoc)->GetView(0, 0)->GetHTMLStyles();
	}
	htmlHeader += _T("--></style>\n");
	if (pTableDoc || pImageDoc)
	{
		htmlHeader += LR"(<script>
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
		htmlHeader += LR"(<script>
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
		htmlHeader += LR"(<div id="toc-sentinel"></div>
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
static void WriteFooter(UniStdioFile& file)
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
												 const String& sFileName, const Options& options, String& errStr)
{
	if (mergeDocuments.empty())
		return false;

	UniStdioFile file;
	if (!file.Open(sFileName, _T("wt")))
	{
		String errMsg = GetSysError(GetLastError());
		errStr = strutils::format_string1(
			_("Error creating the report:\n%1"), errMsg);
		return false;
	}

	file.SetCodepage(ucr::CP_UTF_8);

	String outputDirectory = paths::RemoveExtension(sFileName) + _T(".files");

	ReportContext reportContext(file, options.includeAllImagePages, outputDirectory);

	WriteHeader(mergeDocuments, options, file);

	int i = 0;
	// Generate report for each document
	for (auto pMergeDoc : mergeDocuments)
	{
		if (pMergeDoc == nullptr)
			continue;

		reportContext.index = i;

		bool ok = false;
		if (mergeDocuments.size() > 1)
		{
			file.WriteString(strutils::format(_T("<section id=\"doc%d\" class=\"report-section\">\n"), i));
			// Generate HTML content from the document
			ok = pMergeDoc->GenerateReport(reportContext);
			file.WriteString(_T("</section>\n"));
		}
		else
		{
			ok = pMergeDoc->GenerateReport(reportContext);
		}
		if (!ok)
		{
			errStr = strutils::format(_T("Failed to generate report for document %d."), i + 1);
			file.Close();
			return false;
		}

		++i;
	}

	WriteFooter(file);

	file.Close();
	return true;
}

bool CFileCmpReport::CopyToClipboard(const String& sFileName, String& errStr)
{
	UniMemFile file;
	if (!file.OpenReadOnly(sFileName))
	{
		String errMsg = GetSysError(GetLastError());
		errStr = strutils::format_string1(
			_("Error opening the report for clipboard copy:\n%1"), errMsg);
		return false;
	}
	file.SetUnicoding(ucr::UTF8);
	String content;
	file.ReadStringAll(content);
	file.Close();
	if (!ClipboardUtils::PutFileAndTextAndHTML(sFileName, content, HWND(nullptr)))
	{
		errStr = _("Failed to copy to clipboard.");
		return false;
	}
	return true;
}
