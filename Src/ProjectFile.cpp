/////////////////////////////////////////////////////////////////////////////
//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or (at
//    your option) any later version.
//    
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  ProjectFile.cpp
 *
 * @brief Implementation file for ProjectFile class.
 */
// ID line follows -- this is updated by CVS
// $Id: ProjectFile.cpp 7081 2010-01-01 20:33:30Z kimmov $

#include "ProjectFile.h"

#include <Poco/SAX/InputSource.h>
#include <Poco/XML/XMLWriter.h>
#include <Poco/DOM/AutoPtr.h>
#include <Poco/DOM/Document.h>
#include <Poco/DOM/DOMParser.h>
#include <Poco/DOM/DOMWriter.h>
#include <Poco/DOM/NodeIterator.h>
#include <Poco/DOM/NodeFilter.h>
#include <Poco/DOM/Text.h>
#include <Poco/Exception.h>
#include "UnicodeString.h"
#include "unicoder.h"

using Poco::AutoPtr;
using Poco::XML::InputSource;
using Poco::XML::DOMParser;
using Poco::XML::DOMWriter;
using Poco::XML::Document;
using Poco::XML::Element;
using Poco::XML::NodeFilter;
using Poco::XML::NodeIterator;
using Poco::XML::Node;
using Poco::XML::Text;
using Poco::XML::XMLWriter;
using ucr::UTF82T;
using ucr::T2UTF8;

// Constants for xml element names
const char Root_element_name[] = "project";
const char Paths_element_name[] = "paths";
const char Left_element_name[] = "left";
const char Middle_element_name[] = "middle";
const char Right_element_name[] = "right";
const char Filter_element_name[] = "filter";
const char Subfolders_element_name[] = "subfolders";
const char Left_ro_element_name[] = "left-readonly";
const char Middle_ro_element_name[] = "middle-readonly";
const char Right_ro_element_name[] = "right-readonly";


/** @brief File extension for path files */
const String ProjectFile::PROJECTFILE_EXT = UTF82T("WinMerge");

/** 
 * @brief Standard constructor.
 */
 ProjectFile::ProjectFile()
: m_bHasLeft(false)
, m_bHasMiddle(false)
, m_bHasRight(false)
, m_bHasFilter(false)
, m_bHasSubfolders(false)
, m_subfolders(-1)
, m_bLeftReadOnly(false)
, m_bMiddleReadOnly(false)
, m_bRightReadOnly(false)
{
}

/** 
 * @brief Open given path-file and read data from it to member variables.
 * @param [in] path Path to project file.
 * @param [out] sError Error string if error happened.
 * @return true if reading succeeded, false if error happened.
 */
bool ProjectFile::Read(const String& path)
{
	DOMParser parser;
	parser.setFeature(DOMParser::FEATURE_FILTER_WHITESPACE, true);
	AutoPtr<Document> tree = parser.parse(T2UTF8(path));
	if (!tree)
		return false;
	Element* root = GetRootElement(tree.get());
	if (!root)
		return false;
	// Currently our content is paths, so expect
	// having paths in valid project file!
	if (!GetPathsData(root))
		return false;

	return true;
}

/** 
 * @brief Return project file XML's root element.
 * @param [in] tree XML tree we got from the parser.
 * @return Root element pointer.
 */
Element* ProjectFile::GetRootElement(const Document * tree)
{
	if (!tree)
		return NULL;
	// Make sure we have correct root element
	Element *root = dynamic_cast<Element *>(tree->firstChild());
	if (root->nodeName() != Root_element_name)
		return NULL;
	return root;
}

/** 
 * @brief Reads the paths data from the XML data.
 * This function reads the paths data inside given element in XML data.
 * @param [in] parent Parent element for the paths data.
 * @return true if pathdata was found from the file.
 */
bool ProjectFile::GetPathsData(const Element * parent)
{
	if (!parent)
		return false;
	
	Node* paths = parent->firstChild();
	if (!paths || paths->nodeName() != Paths_element_name)
		return false;

	NodeIterator it(paths, NodeFilter::SHOW_ELEMENT);
	Node* pNode = it.nextNode();
	while (pNode)
	{
		std::string nodename = pNode->nodeName();
		if (nodename == Left_element_name)
		{
			m_paths.SetLeft(UTF82T(pNode->innerText()).c_str());
			m_bHasLeft = true;
		}
		else if (nodename == Middle_element_name)
		{
			m_paths.SetMiddle(UTF82T(pNode->innerText()).c_str());
			m_bHasMiddle = true;
		}
		else if (nodename == Right_element_name)
		{
			m_paths.SetRight(UTF82T(pNode->innerText()).c_str());
			m_bHasRight = true;
		}
		else if (nodename == Filter_element_name)
		{
			m_filter = UTF82T(pNode->innerText());
			m_bHasFilter = true;
		}
		else if (nodename == Subfolders_element_name)
		{
			m_subfolders = atoi(pNode->innerText().c_str());
			m_bHasSubfolders = true;
		}
		else if (nodename == Left_ro_element_name)
		{
			m_bLeftReadOnly = atoi(pNode->innerText().c_str()) != 0;
		}
		else if (nodename == Middle_ro_element_name)
		{
			m_bMiddleReadOnly = atoi(pNode->innerText().c_str()) != 0;
		}
		else if (nodename == Right_ro_element_name)
		{
			m_bRightReadOnly = atoi(pNode->innerText().c_str()) != 0;
		}
		pNode = it.nextNode();
	}
	return true;
}

/** 
 * @brief Save data from member variables to path-file.
 * @param [in] path Path to project file.
 * @param [out] sError Error string if error happened.
 * @return true if saving succeeded, false if error happened.
 */
bool ProjectFile::Save(const String& path) const
{
	AutoPtr<Document> doc = new Document();
	AutoPtr<Element> root = doc->createElement(Root_element_name);
	if (!root)
		return false;
	doc->appendChild(root);
	AutoPtr<Element> paths = AddPathsElement(root);
	if (!paths)
		return false;
	AddPathsContent(paths);

	DOMWriter writer;
	writer.setOptions(XMLWriter::WRITE_XML_DECLARATION | XMLWriter::PRETTY_PRINT);
	writer.writeNode(T2UTF8(path), doc);

	return true;
}

/**
 * @brief Add paths element into XML tree.
 * @param [in] parent Parent element for the paths element.
 * @return pointer to added paths element.
 */
Element* ProjectFile::AddPathsElement(Element * parent) const
{
	Element *element = parent->ownerDocument()->createElement(Paths_element_name);
	parent->appendChild(element);
	return element;
}

static Element *createElement(const Document *doc, const std::string& tagname, const std::string& content)
{
	Element *element = doc->createElement(tagname);
	AutoPtr<Text> text = doc->createTextNode(content);
	element->appendChild(text);
	return element;
}

/**
 * @brief Add paths data to the XML tree.
 * This function adds our paths data to the XML tree.
 * @param [in] parent Parent element for paths data.
 * @return true if we succeeded, false otherwise.
 */
bool ProjectFile::AddPathsContent(Element * parent) const
{
	Document *doc = parent->ownerDocument();
	AutoPtr<Element> element;

	if (!m_paths.GetLeft().empty())
	{
		element = createElement(doc, Left_element_name, T2UTF8(m_paths.GetLeft()));
		parent->appendChild(element);
	}

	if (!m_paths.GetMiddle().empty())
	{
		element = createElement(doc, Middle_element_name, T2UTF8(m_paths.GetMiddle()));
		parent->appendChild(element);
	}

	if (!m_paths.GetRight().empty())
	{
		element = createElement(doc, Right_element_name, T2UTF8(m_paths.GetRight()));
		parent->appendChild(element);
	}

	if (!m_filter.empty())
	{
		element = createElement(doc, Filter_element_name, T2UTF8(m_filter));
		parent->appendChild(element);
	}

	element = createElement(doc, Subfolders_element_name, m_subfolders != 0 ? "1" : "0");
	parent->appendChild(element);

	element = createElement(doc, Left_ro_element_name, m_bLeftReadOnly ? "1" : "0");
	parent->appendChild(element);

	if (!m_paths.GetMiddle().empty())
	{
		element = createElement(doc, Middle_ro_element_name, m_bMiddleReadOnly ? "1" : "0");
		parent->appendChild(element);
	}

	element = createElement(doc, Right_ro_element_name, m_bRightReadOnly ? "1" : "0");
	parent->appendChild(element);

	return true;
}

/** 
 * @brief Returns if left path is defined in project file.
 * @return true if project file has left path.
 */
bool ProjectFile::HasLeft() const
{
	return m_bHasLeft;
}

/** 
 * @brief Returns if middle path is defined.
 */
bool ProjectFile::HasMiddle() const
{
	return m_bHasMiddle;
}

/** 
 * @brief Returns if right path is defined in project file.
 * @return true if project file has right path.
 */
bool ProjectFile::HasRight() const
{
	return m_bHasRight;
}

/** 
 * @brief Returns if filter is defined in project file.
 * @return true if project file has filter.
 */
bool ProjectFile::HasFilter() const
{
	return m_bHasFilter;
}

/** 
 * @brief Returns if subfolder is defined in projectfile.
 * @return true if project file has subfolder definition.
 */
bool ProjectFile::HasSubfolders() const
{
	return m_bHasSubfolders;
}

/** 
 * @brief Returns left path.
 * @param [out] pReadOnly true if readonly was specified for path.
 * @return Left path.
 */
String ProjectFile::GetLeft(bool * pReadOnly /*=NULL*/) const
{
	if (pReadOnly)
		*pReadOnly = m_bLeftReadOnly;
	return m_paths.GetLeft();
}

/** 
 * @brief Returns if left path is specified read-only.
 * @return true if left path is read-only, false otherwise.
 */
bool ProjectFile::GetLeftReadOnly() const
{
	return m_bLeftReadOnly;
}

/** 
 * @brief Set left path, returns old left path.
 * @param [in] sLeft Left path.
 * @param [in] bReadOnly Will path be recorded read-only?
 */
void ProjectFile::SetLeft(const String& sLeft, const bool * pReadOnly /*=NULL*/)
{
	m_paths.SetLeft(sLeft.c_str(), false);
	if (pReadOnly)
		m_bLeftReadOnly = *pReadOnly;
}

/** 
 * @brief Returns middle path.
 * @param [out] pReadOnly true if readonly was specified for path.
 */
String ProjectFile::GetMiddle(bool * pReadOnly /*=NULL*/) const
{
	if (pReadOnly)
		*pReadOnly = m_bMiddleReadOnly;
	return m_paths.GetMiddle();
}

/** 
 * @brief Returns if middle path is specified read-only.
 */
bool ProjectFile::GetMiddleReadOnly() const
{
	return m_bMiddleReadOnly;
}

/** 
 * @brief Set middle path, returns old middle path.
 * @param [in] sMiddle Middle path.
 * @param [in] bReadOnly Will path be recorded read-only?
 */
void ProjectFile::SetMiddle(const String& sMiddle, const bool * pReadOnly /*=NULL*/)
{
	m_paths.SetMiddle(sMiddle.c_str(), false);
	if (pReadOnly)
		m_bMiddleReadOnly = *pReadOnly;

	return;
}

/** 
 * @brief Returns right path.
 * @param [out] pReadOnly true if readonly was specified for path.
 * @return Right path.
 */
String ProjectFile::GetRight(bool * pReadOnly /*=NULL*/) const
{
	if (pReadOnly)
		*pReadOnly = m_bRightReadOnly;
	return m_paths.GetRight();
}

/** 
 * @brief Returns if right path is specified read-only.
 * @return true if right path is read-only, false otherwise.
 */
bool ProjectFile::GetRightReadOnly() const
{
	return m_bRightReadOnly;
}

/** 
 * @brief Set right path, returns old right path.
 * @param [in] sRight Right path.
 * @param [in] bReadOnly Will path be recorded read-only?
 */
void ProjectFile::SetRight(const String& sRight, const bool * pReadOnly /*=NULL*/)
{
	m_paths.SetRight(sRight.c_str(), false);
	if (pReadOnly)
		m_bRightReadOnly = *pReadOnly;
}

/** 
 * @brief Returns filter.
 * @return Filter string.
 */
String ProjectFile::GetFilter() const
{
	return m_filter;
}

/** 
 * @brief Set filter.
 * @param [in] sFilter New filter string to set.
 */
void ProjectFile::SetFilter(const String& sFilter)
{
	m_filter = sFilter;
}

/** 
 * @brief Returns subfolder included -setting.
 * @return != 0 if subfolders are included.
 */
int ProjectFile::GetSubfolders() const
{
	return m_subfolders;
}

/** 
 * @brief set subfolder.
 * @param [in] iSubfolder New value for subfolder inclusion.
 */
void ProjectFile::SetSubfolders(int iSubfolder)
{
	m_subfolders = iSubfolder ? 1 : 0;
}

/** 
 * @brief 
 *
 * @param [in] files Files in project
 * @param [in] bSubFolders If true subfolders included (recursive compare)
 */
void ProjectFile::SetPaths(const PathContext& files, bool bSubfolders)
{
	m_paths = files;
	m_subfolders = bSubfolders;
}

/** 
 * @brief Returns left and right paths and recursive from project file
 * 
 * @param [out] files Files in project
 * @param [out] bSubFolders If true subfolders included (recursive compare)
 */
void ProjectFile::GetPaths(PathContext& files, bool & bSubfolders) const
{
	files = m_paths;
	if (HasSubfolders())
		bSubfolders = (GetSubfolders() == 1);
}
