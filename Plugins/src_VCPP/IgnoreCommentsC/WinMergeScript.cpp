// WinMergeScript.cpp : Implementation of CWinMergeScript
#include "stdafx.h"
#include "IgnoreCommentsC.h"
#include "WinMergeScript.h"
#include <fstream.h>

/////////////////////////////////////////////////////////////////////////////
// CWinMergeScript


STDMETHODIMP CWinMergeScript::get_PluginEvent(BSTR *pVal)
{
  *pVal = SysAllocString(L"BUFFER_PREDIFF");
  return S_OK;
}

STDMETHODIMP CWinMergeScript::get_PluginDescription(BSTR *pVal)
{
  *pVal = SysAllocString(L"Transform C/CPP to compare without comments - save not possible");
  return S_OK;
}

STDMETHODIMP CWinMergeScript::get_PluginFileFilters(BSTR *pVal)
{
  *pVal = SysAllocString(L"\\.cpp$;\\.cxx$;\\.h$;\\.hxx$;\\.c$;\\.php$;\\.js$");
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


STDMETHODIMP CWinMergeScript::UnpackFile(BSTR fileSrc, BSTR fileDst, VARIANT_BOOL *pbChanged, INT *pSubcode, VARIANT_BOOL *pbSuccess)
{
  // We don't need it
  return S_OK;
}



STDMETHODIMP CWinMergeScript::PackFile(BSTR fileSrc, BSTR fileDst, VARIANT_BOOL *pbChanged, INT pSubcode, VARIANT_BOOL *pbSuccess)
{
  // always return error so the users knows we can not repack
  *pbChanged = VARIANT_FALSE;
  *pbSuccess = VARIANT_FALSE;
  return S_OK;
}


STDMETHODIMP CWinMergeScript::PrediffBufferW(BSTR *pText, INT *pSize, VARIANT_BOOL *pbChanged, VARIANT_BOOL *pbHandled)
{
 // rule
 // scan lines ,  analyse ", \"
 // find // in lines
 //   compact // ..EOL  to //EOL
 //   compact /*.. */ to /**/   (keep line ends)
#ifdef _DEBUG
  __asm  { int 3 };
#endif
  
  LPWSTR p;
  p=*pText;

  UINT N=*pSize;
  
  UINT j=0;
  UINT bol=0, sbol=0;
  for(UINT i=0; i<N;)
  {
    switch(p[i])
    {
    default:
      p[j++]=p[i++]; 
      break;

    case '\r': 
    case '\n': p[j++]=p[i++]; bol=j; sbol=i; break;
    case '"':
      for(p[j++]=p[i++]; i<N && p[i]!='\"' && p[i]!='\r' && p[i]!='\n';)
      {
	if(p[i]=='\\' && i+1<N) // skip over \'
	  p[j++]=p[i++];
        p[j++]=p[i++];
      }
      if(i<N)
	p[j++]=p[i++]; 
      break;
    case '\'':
      for(p[j++]=p[i++]; i<N && p[i]!='\'' && p[i]!='\r' && p[i]!='\n';)
      {
	if(p[i]=='\\' && i+1<N) // skip over \'
	  p[j++]=p[i++];
        p[j++]=p[i++];
      }
      if(i<N)
	p[j++]=p[i++]; 
      break;

    case '/':
      switch(p[i+1])
      {
      case '*': // start /* processing
	p[j++]=p[i++]; 
	p[j++]=p[i++];   // copy /*
	for(;i+1<N && !(p[i]=='*' && p[i+1]=='/'); i++) // skip till */
	  if(p[i]=='\r' || p[i]=='\n') p[j++]=p[i];  // keep line feeds

	i++;
	i--;
	if(i+1<N && (p[i]=='*' && p[i+1]=='/'))
	{
	  p[j++]=p[i++]; 
	  p[j++]=p[i++];   // copy /*
	}
        break;
      case '/': // start // processing
	p[j++]=p[i++]; 
	p[j++]=p[i++];   // copy //
	for(;i<N && p[i]!='\r' && p[i]!='\n'; i++); // skip till eol
        break;
      default:
	p[j++]=p[i++]; 
      }
    }
  }
  p[j]=0;
#ifdef _DEBUG

  if(OpenClipboard(NULL))
  {
    EmptyClipboard();

  HGLOBAL hG=GlobalAlloc(GMEM_DDESHARE|GMEM_MOVEABLE, (j+1)*2);
  CopyMemory(GlobalLock(hG), p,(j+1)*2);
  GlobalUnlock(hG);
    SetClipboardData(CF_UNICODETEXT, (HANDLE)hG);
CloseClipboard();
  }
#endif



  // set the new size
  *pSize = j;

  if (j == N)
    *pbChanged = VARIANT_FALSE;
  else
    *pbChanged = VARIANT_TRUE;

  *pbHandled = VARIANT_TRUE;
  return S_OK;
}

