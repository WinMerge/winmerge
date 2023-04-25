/////////////////////////////////////////////////////////////////////////////
//
//    DisplayXMLFiles:  a plugin for WinMerge to display XML files
//    Copyright (C) 2005  Christian List
//    Author: Christian List
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <stdio.h>
#include "DisplayXMLFiles.h"
#include "WinMergeScript.h"
#include "expat.h"
#include "expat_maps.h"


/////////////////////////////////////////////////////////////////////////////
// CWinMergeScript


STDMETHODIMP CWinMergeScript::get_PluginEvent(BSTR *pVal)
{
	*pVal = SysAllocString(L"FILE_PACK_UNPACK");
	return S_OK;
}

STDMETHODIMP CWinMergeScript::get_PluginDescription(BSTR *pVal)
{
	*pVal = SysAllocString(L"Transform an xml file so WinMerge can display it - save will not transform back");
	return S_OK;
}

STDMETHODIMP CWinMergeScript::get_PluginFileFilters(BSTR *pVal)
{
	// xml - XML files
	// xsd - XML schema files
	// xsl - XML style sheet, xslt-fo, whatever
	// xslt - XML style sheet
	// svg - Scalable Vector graphics
	// wsf - Windows Scripting Host files
	*pVal = SysAllocString(L"\\.xml;\\.xsd;\\.xsl;\\.xslt;\\.svg;\\.wsf$");
	return S_OK;
}

STDMETHODIMP CWinMergeScript::get_PluginIsAutomatic(VARIANT_BOOL *pVal)
{
	*pVal = VARIANT_TRUE;
	return S_OK;
}


STDMETHODIMP CWinMergeScript::UnpackBufferA(SAFEARRAY **pBuffer, INT *pSize, VARIANT_BOOL *pbChanged, INT *pSubcode, VARIANT_BOOL *pbSuccess)
{
	// We don't need it
	return S_OK;
}

STDMETHODIMP CWinMergeScript::PackBufferA(SAFEARRAY **pBuffer, INT *pSize, VARIANT_BOOL *pbChanged, INT subcode, VARIANT_BOOL *pbSuccess)
{
	// We don't need it
	return S_OK;
}

/**
 * @brief A class for the stuff we need to get carried around to all the handlers
 */
class CXMLData
{
public:
	FILE* pOutput;
	int iDepth;
	bool bInElement;
	bool bNeedsEnding;
};

static void XMLCALL StartElementHandler(void *userData, const char *name, const char **atts)
{
	int i;
	CXMLData *pData = (CXMLData*)userData;

	// End the previous element, if needed
	if (pData->bNeedsEnding)
	{
		fprintf(pData->pOutput,">");
	}
	
	// Indent
	if (pData->iDepth > 0)
	{
		fprintf(pData->pOutput,"\n");
		for (i = 0; i < pData->iDepth; i++)
		{
			fprintf(pData->pOutput,"\t");
		}
	}

	// Start new element
	fprintf(pData->pOutput,"<%s",name);
	
	// Process all attributes
	size_t iLen = 0;
	int iCount = 0;
	for (i = 0; atts[i]; i += 2)
	{
		if (iLen > 80 || iCount > 4)
		{
			iLen = 0;
			iCount = 0;
			// Indent
			if (pData->iDepth > 0)
			{
				fprintf(pData->pOutput,"\n");
				for (int j = 0; j < pData->iDepth + 1; j++)
				{
					fprintf(pData->pOutput,"\t");
				}
			}
		}

		fprintf(pData->pOutput," %s=\"%s\"", atts[i], atts[i + 1]);

		iCount++;
		iLen += 4 + strlen(atts[i]) + strlen(atts[i + 1]);
	}
	
	// New elements are open-ended, and needs to be closed depending on what follows
	pData->bNeedsEnding = true;
	// We are now inside an element
	pData->bInElement = true;
	// Indent one more
	pData->iDepth += 1;
}

static void XMLCALL EndElementHandler(void *userData, const char *name)
{
	int i;
	CXMLData *pData = (CXMLData*)userData;
	// Indent one less
	pData->iDepth -= 1;

	// End this element, depending on what was before this
	if (pData->bNeedsEnding)
	{
		fprintf(pData->pOutput,"/>");
	}
	else
	{
		if (!pData->bInElement)
		{
			// Indent
			fprintf(pData->pOutput,"\n");
			for (i = 0; i < pData->iDepth; i++)
			{
				fprintf(pData->pOutput,"\t");
			}
		}
		fprintf(pData->pOutput,"</%s>",name);
	}

	// Element is ended
	pData->bNeedsEnding = false;
	// We are no longer inside an element
	pData->bInElement = false;
}

static void XMLCALL DefaultHandler(void *userData, const char *s, int len)
{
	CXMLData *pData = (CXMLData*)userData;

	// Test for all whitespace
	int i;
	bool bIsAllWhiteSpace = true;
	for (i = 0; i < len; i++)
	{
		if (!isspace((unsigned char)s[i]))
			bIsAllWhiteSpace = false;
	}

	// Only output something if it's not all whitespace
	if (!bIsAllWhiteSpace)
	{
		// End the previous element if needed
		if (pData->bNeedsEnding)
		{
			fprintf(pData->pOutput,">");
			pData->bNeedsEnding = false;

		}

		// Just output verbatim, this is most likely the text content of a an element
		fwrite( s, sizeof( char ), len, pData->pOutput );
	}

}

static void XMLCALL ProcessingInstructionHandler(void *userData, const char *target, const char *data)
{
	CXMLData *pData = (CXMLData*)userData;

	// End the previous element if needed
	if (pData->bNeedsEnding)
	{
		fprintf(pData->pOutput,">\n");
		pData->bNeedsEnding = false;
	}

	// Indent
	for (int i = 0; i < pData->iDepth; i++)
	{
		fprintf(pData->pOutput,"\t");
	}

	// Output processing instruction
	fprintf(pData->pOutput, "<?%s %s?>\n", target, data);
}

static void XMLCALL CommentHandler(void *userData, const char *data)
{
	CXMLData *pData = (CXMLData*)userData;

	// End the previous element if needed
	if (pData->bNeedsEnding)
	{
		fprintf(pData->pOutput,">");
		pData->bNeedsEnding = false;
	}

	// Indent
	if (pData->iDepth > 0)
	{
		fprintf(pData->pOutput,"\n");
		for (int i = 0; i < pData->iDepth; i++)
		{
			fprintf(pData->pOutput,"\t");
		}
	}

	// Output comment
	fprintf(pData->pOutput,"<!--%s-->\n",data);
}

static void XMLCALL XmlDeclHandler(void *userData, const char *version, const char *encoding, int standalone)
{
	CXMLData *pData = (CXMLData*)userData;
	// Output the declaration line (the topmost line)
	fprintf(pData->pOutput,"<?xml");
	if (version)
		fprintf(pData->pOutput," version=\"%s\"",version);
	if (encoding)
		fprintf(pData->pOutput," encoding=\"%s\"",encoding);
	if (standalone == 0)
		fprintf(pData->pOutput," standalone=\"no\"");
	else if (standalone == 1)
		fprintf(pData->pOutput," standalone=\"yes\"");

	fprintf(pData->pOutput," ?>\n");
}

static int
WinMerge_Plug_UnknownEncodingHandler(void *encodingHandlerData,
	const XML_Char *name,
	XML_Encoding *info)
{
	return expat_maps_getMap(name, info);
}


STDMETHODIMP CWinMergeScript::UnpackFile(BSTR fileSrc, BSTR fileDst, VARIANT_BOOL *pbChanged, INT *pSubcode, VARIANT_BOOL *pbSuccess)
{
	USES_CONVERSION;

	// Open input file for read binary
	FILE *pInput = _wfopen(fileSrc, L"rb");
	// Init data structure used in handlers
	CXMLData oData;
	oData.iDepth = 0;
	oData.bInElement = false;
	oData.bNeedsEnding = false;
	// Get file size
	fseek(pInput, 0L, SEEK_END);
	size_t size = ftell(pInput);
	fseek(pInput, 0L, SEEK_SET);
	// Open output file for write binary
	oData.pOutput = _wfopen(fileDst, L"wb");

	// Set all handlers
	char *buf = new char[size];
	XML_Parser parser = XML_ParserCreate(NULL);
	XML_SetUserData(parser, &oData);
	XML_SetElementHandler(parser, StartElementHandler, EndElementHandler);
	XML_SetDefaultHandler(parser, DefaultHandler);
	XML_SetProcessingInstructionHandler(parser, ProcessingInstructionHandler);
	XML_SetCommentHandler(parser, CommentHandler);
	XML_SetXmlDeclHandler(parser, XmlDeclHandler);
	XML_SetUnknownEncodingHandler(parser, WinMerge_Plug_UnknownEncodingHandler, this);
	size_t len = fread(buf, 1, size, pInput);
	// Parse
	if (XML_Parse(parser, buf, static_cast<int>(len), true) == XML_STATUS_ERROR)
	{
		// There was an error
		// Give a warning and return without converting anything
		char sError[1024];
		sprintf(sError, "%s at line %d\n",
			XML_ErrorString(XML_GetErrorCode(parser)),
			XML_GetCurrentLineNumber(parser));
		
		::MessageBoxA(NULL, sError, "The xml has an error", MB_OK | MB_ICONWARNING | MB_SETFOREGROUND | MB_TOPMOST);
		XML_ParserFree(parser);
		delete [] buf;
		fclose(pInput);
		fclose(oData.pOutput);
		*pbChanged = VARIANT_FALSE;
		*pbSuccess = VARIANT_FALSE;
		return S_FALSE;
	}
	XML_ParserFree(parser);
	delete [] buf;

	// Close all files
	fclose(pInput);
	fclose(oData.pOutput);

	*pbChanged = VARIANT_TRUE;
	*pbSuccess = VARIANT_TRUE;
	return S_OK;
}

STDMETHODIMP CWinMergeScript::PackFile(BSTR fileSrc, BSTR fileDst, VARIANT_BOOL *pbChanged, INT pSubcode, VARIANT_BOOL *pbSuccess)
{
	// always return error so the users knows we can not repack
	*pbChanged = VARIANT_FALSE;
	*pbSuccess = VARIANT_FALSE;
	return S_OK;
}

STDMETHODIMP CWinMergeScript::ShowSettingsDialog(VARIANT_BOOL *pbHandled)
{
	*pbHandled = VARIANT_FALSE;
	return E_NOTIMPL;
}
