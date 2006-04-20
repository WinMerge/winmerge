/**
 * @file  XmlDoc.h
 *
 * @brief Declaration file for XmlDoc and XmlElement classes.
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#ifndef XmlDoc_h_included
#define XmlDoc_h_included

class XmlElement;
typedef CTypedPtrList<CPtrList, XmlElement*> XmlElementList;

struct XmlLoadDocData;
struct XmlLoadElementData;

/**
 * @brief Simple XML document class to ease doing XML output
 *
 * Sample use:
 *
 * XmlDoc doc(_T("C:\\somewhere\\goodstuff.xml"), XmlDoc::XML_SAVE, _T("UTF-8"));
 * doc.Begin();
 * {
 *    XmlElement(doc, _T("automobile"));
 *    {
 *        XmlElement(doc, _T("seats"));
 *        {
 *            XmlElement(doc, _T("front")).Append(_T("bucket"));
 *            XmlElement(doc, _T("rear")).Append(_T("bench"));
 *        }
 *    }
 * }
 * doc.End();
 *
 * Sample output:
 *
 *  <?xml version='1.0' encoding='UTF-8'>
 *  <automobile>
 *    <seats>
 *      <front>bucket</bucket>
 *      <rear>bench</rear>
 *    </seats>
 *  </automobile>
 */
class XmlDoc
{
public:
	typedef enum { XML_LOAD, XML_SAVE } XML_LOADSAVE;

	XmlDoc(LPCTSTR path, XML_LOADSAVE loadSave, LPCTSTR encoding);
	void Begin();
	void End();

// Implementation methods
private:
	CString GetXml() const;
	int GetCodepage() const { return m_codepage; }

	void Append(LPCTSTR str);
	void AppendN(int n) { TCHAR buff[33]; m_xml += _itot(n, buff, 10); }

	void Content(XmlElement * xel, CString & str);
	void Content(XmlElement * xel, int & val);

	void StartTag(XmlElement * xel);
	void PopMe(XmlElement * xel);
	void EndTag(XmlElement * xel);
	CString GetLeader() const;

// Implementation data
private:
	CString m_path;
	XML_LOADSAVE m_loadSave;
	int m_live;
	CString m_xml;
	XmlElementList m_openElements;
	XmlLoadDocData * m_load; // hide loading implementation data inside this pointer
	int m_codepage;

friend class XmlElement;
};

/**
 * @brief Simple XML element class to ease doing XML output
 */
class XmlElement
{
public:
	// container
	XmlElement(XmlDoc & doc, LPCTSTR tag);

	// value
	XmlElement(XmlDoc & doc, LPCTSTR tag, CString & val);
	XmlElement(XmlDoc & doc, LPCTSTR tag, int & val);

	~XmlElement();

// Implementation methods
private:
	bool isMultiline() const { return m_multiline; }
	void SetMultiline(bool isMultiline=true) { m_multiline = isMultiline; }

// Implementation data
private:
	XmlDoc & m_doc;
	CString m_tag;
	CString m_normalizedTag;
	bool m_multiline;
	XmlLoadElementData * m_load;
friend XmlDoc;
};


#endif // XmlDoc_h_included

