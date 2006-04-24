/**
 * @file  XmlDoc.cpp
 *
 * @brief Implementation of the XmlDoc and XmlElement classes.
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#include "stdafx.h"
#include "XmlDoc.h"
#include "markdown.h"

// Private types

/**
 * @brief Stuff you need to load an XML file
 */
struct XmlLoadDocData
{
	CMarkdown::File xmlfile;
	CMarkdown::EntityMap entities;
	UINT codepage;
	XmlLoadDocData(LPCTSTR path) : xmlfile(path), codepage(-1) { }
};

/**
 * @brief element-specific stuff used in loading an XML file
 */
struct XmlLoadElementData
{
	CMarkdown markdown;

	// Have to be able to construct either from markdown of parent
	// or from markdown file (if element with no parent, ie, root)
	XmlLoadElementData(CMarkdown &md) : markdown(md) { }
	XmlLoadElementData(CMarkdown::File & file) : markdown(file) { }
};

// Private functions

/**
 * @brief Convert a string into correct encoding & markdown string object
 */
static LPCSTR
MakeMarkdownString(const CString & str, int codepage)
{
	return CMarkdown::String(CMarkdown::HSTR(str.AllocSysString())->Entities()->Octets(codepage)).A;
}

// Body of module

XmlDoc::XmlDoc(LPCTSTR path, XML_LOADSAVE loadSave, LPCTSTR encoding)
: m_path(path)
, m_loadSave(loadSave)
, m_live(0)
// CString m_xml;
// XmlElementList m_openElements;
, m_load(NULL)
, m_codepage(65001)
{
	if (m_loadSave == XML_SAVE)
	{
		m_xml.Format(_T("<?xml version='1.0' encoding='%s'?>"), encoding);
	}
	// Loading is all done in the Begin method (2 stage construction)

}

/**
 * @brief Destructor, delete dynamically created load-object.
 */
XmlDoc::~XmlDoc()
{
	if (m_load)
		delete m_load;
}

/**
 * @brief 2nd stage construction of XmlDoc object (so can throw exception)
 */
void
XmlDoc::Begin()
{
	// Saving did its (simple) initialization in the constructor
	if (m_loadSave == XML_LOAD)
	{
		ASSERT(!m_load);
		m_load = new XmlLoadDocData(m_path);
		m_load->entities.Load();
		if (m_load->xmlfile.pImage == NULL)
		{
			CFileException::ThrowOsError(GetLastError(), m_path);
		}
		// If encoding is other than UTF-8, assume CP_ACP
		CMarkdown::String encoding = CMarkdown(m_load->xmlfile).Move("?xml").GetAttribute("encoding");
		m_codepage = lstrcmpiA(encoding.A, "UTF-8") == 0 ? CP_UTF8 : CP_ACP;
	}
}

/**
 * @brief Finish xml document (needed to write out all XML, if saving)
 */
void
XmlDoc::End()
{
	if (m_loadSave == XML_SAVE)
	{
		// Open file in exclusive mode to make sure there are no other readers
		// or writers to same file
		CStdioFile file;
		BOOL bSuccess = file.Open(m_path, CFile::modeCreate | CFile::modeWrite |
				CFile::shareExclusive, NULL);
		if (bSuccess)
		{
			CString xml = GetXml();
			file.SeekToEnd();
			file.WriteString(xml);
			file.Close();
		}
		else
		{
			CFileException::ThrowOsError(GetLastError(), m_path);
		}
	}
}

/**
 * @brief Add some content text to XML document
 */
void
XmlDoc::Append(LPCTSTR str)
{
	m_xml += MakeMarkdownString(str, m_codepage); // UTF-8 encoding
}

/**
 * @brief Get all accumulated XML
 */
CString
XmlDoc::GetXml() const
{
	// Return accumulated XML, but ensure it has trailing \n
	CString xml = m_xml;
	if (xml[xml.GetLength()-1] != '\n')
		xml += '\n';
	return xml;
}

void
XmlDoc::StartTag(XmlElement * xel)
{
	LPCSTR tag = MakeMarkdownString(xel->m_tag, GetCodepage()); // UTF-8 encoding
	xel->m_normalizedTag = tag;

	if (m_loadSave == XML_SAVE)
	{
		// Add tabs & open tag to XML string
		m_xml += (CString)_T("\n") + GetLeader() + _T("<") + tag + _T(">");
		// Mark any currently open tags as multiline (for pretty printing)
		// (as they are complex b/c they contain this tag)
		for (POSITION pos=m_openElements.GetHeadPosition(); pos; )
		{
			XmlElement * xmem = m_openElements.GetNext(pos);
			xmem->SetMultiline();
		}
	}
	else // XML_LOAD)
	{
		ASSERT(!xel->m_load);
		if (m_openElements.IsEmpty())
		{
			// Create root CMarkdown from CMarkdown::File
			CMarkdown::File & parentFile = (m_load->xmlfile);
			CMarkdown childmd = CMarkdown(parentFile).Move(tag).Pop();
			xel->m_load = new XmlLoadElementData(childmd);
		}
		else
		{
			// Create child CMarkdown from parent CMarkdown
			XmlElement * parel = m_openElements.GetTail();
			CMarkdown & parentMd = (parel->m_load->markdown);
			CMarkdown childMd = CMarkdown(parentMd).Move(tag);
			xel->m_load = new XmlLoadElementData(childMd);
		}
	}
	// Add this tag to list of open tags
	m_openElements.AddTail(xel);
}

/**
 * @brief Non-content elements call this, for CMarkdown::Pop (whatever it does)
 */
void
XmlDoc::PopMe(XmlElement * xel)
{
	if (m_loadSave == XML_LOAD)
	{
		CMarkdown & myMd = xel->m_load->markdown;
		myMd.Pop();
	}
}

/**
 * @brief When saving, write out end tag (at XmlElement destructor time)
 */
void
XmlDoc::EndTag(XmlElement * xel)
{
	CString tag = xel->m_normalizedTag;
	XmlElement * tail = m_openElements.RemoveTail();
	ASSERT(tail == xel);
	if (xel->isMultiline())
	{
		m_xml += _T("\n");
		m_xml += GetLeader();
	}
	m_xml += (CString)_T("</") + tag + _T(">");
}

/**
 * @brief (When saving) Get correct number of tabs to print before element
 */
CString
XmlDoc::GetLeader() const
{
	CString leader;
	for (int i=0; i<m_openElements.GetCount(); ++i)
		leader += _T("\t");
	return leader;
}

/**
 * @brief Read or write some textual content at current location
 */
void
XmlDoc::Content(XmlElement * xel, CString & str)
{
	if (m_loadSave == XML_SAVE)
	{
		Append(str);
	}
	else
	{
		CMarkdown::EntityMap & entities = m_load->entities;
		CMarkdown & myMd = xel->m_load->markdown;
		str = CMarkdown::String(myMd.GetInnerText()->Unicode(m_codepage)->Resolve(entities)).W;
	}
}

/**
 * @brief Read or write an int value at current location
 */
void
XmlDoc::Content(XmlElement * xel, int & val)
{
	if (m_loadSave == XML_SAVE)
	{
		AppendN(val);
	}
	else
	{
		CMarkdown & myMd = xel->m_load->markdown;
		sscanf(CMarkdown::String(myMd.GetInnerText()).A, "%d", &val);
	}
}

/**
 * @brief Create simple container element
 */
XmlElement::XmlElement(XmlDoc & doc, LPCTSTR tag)
: m_doc(doc)
, m_tag(tag)
, m_multiline(false)
, m_load(NULL)
{
	m_doc.StartTag(this);
	m_doc.PopMe(this);
}

/**
 * @brief Create simple string leaf node element
 */
XmlElement::XmlElement(XmlDoc & doc, LPCTSTR tag, CString & val)
: m_doc(doc)
, m_tag(tag)
, m_multiline(false)
, m_load(NULL)
{
	m_doc.StartTag(this);
	m_doc.Content(this, val);
}

/**
 * @brief Create integer string leaf node element
 */
XmlElement::XmlElement(XmlDoc & doc, LPCTSTR tag, int & val)
: m_doc(doc)
, m_tag(tag)
, m_multiline(false)
, m_load(NULL)
{
	m_doc.StartTag(this);
	m_doc.Content(this, val);
}
/**
 * brief Cleanup or memory
 */
XmlElement::~XmlElement()
{
	m_doc.EndTag(this);
	delete m_load;
}
