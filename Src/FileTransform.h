#ifndef FileTransform_h
#define FileTransform_h

#include "resource.h"

class CRegExp;
typedef CTypedPtrList<CPtrList, CRegExp*>RegList;



/**
 * @brief Modes for unpacking
 */
enum 
{
	UNPACK_MANUAL,
	UNPACK_AUTO,
};

extern BOOL bUnpackerMode;



/**
 * @brief List of transformation categories (events)
 *
 * @note If you add some event, you have to complete this array in FileTransform.cpp
 */
extern LPCWSTR TransformationCategories[];



typedef int (*packingInternalFnc)				 (char *& pszBuf, UINT & nBufSize, int & nChanged, int handlerSubcode);
typedef int (*unpackingInternalFnc)			 (char *& pszBuf, UINT & nBufSize, int & nChanged, int & handlerSubcode);
typedef BOOL (*checkHeaderInternalFnc)	 (const char * pszBuf, DWORD dwSize);
/** 
 * @brief Do nothing packer
 */
int PackPass (char *& pszBuf, UINT & nBufSize, int & nChanged, int handlerSubcode);
/** 
 * @brief Do nothing unpacker
 */
int UnpackPass (char *& pszBuf, UINT & nBufSize, int & nChanged, int & handlerSubcode);


/**
 * @brief Unpacking info kept during merge session to use the correct packer
 *
 * @note Can be be passed between threads, except for Unicode packing/unpacking
 */
class PackingInfo
{
public:
	void Initialize(BOOL bLocalUnpackerMode)
	{
		// init functions as a valid "do nothing" unpacker
		fncPack = PackPass;
		fncUnpack = UnpackPass;
		// and init bAutomatic flag and name according to global variable
		if (bLocalUnpackerMode == UNPACK_MANUAL)
		{			
			bToBeScanned = FALSE;
		}
		else
		{
			VERIFY(pluginName.LoadString(IDS_USERCHOICE_AUTOMATIC));
			bToBeScanned = TRUE;
		}
	};
	PackingInfo() 
	{
		Initialize(bUnpackerMode);
	};
	PackingInfo(BOOL bLocalUnpackerMode) 
	{
		Initialize(bLocalUnpackerMode);
	};

  operator=(PackingInfo * newInfo) 
	{
		bToBeScanned = newInfo->bToBeScanned;
		fncPack = newInfo->fncPack;
		fncUnpack = newInfo->fncUnpack;
		pluginName = newInfo->pluginName;
		subcode = newInfo->subcode;
		bWithFile = newInfo->bWithFile;
	}

public:
	BOOL										bToBeScanned;
	packingInternalFnc			fncPack;
	unpackingInternalFnc		fncUnpack;
	CString									pluginName;
	int											subcode;
	BOOL										bWithFile;
};


// Events handler
// WinMerge uses one of these entry points to call a plugin

// bMayOverwrite : tells if we can overwrite the source file
// if we don't, don't forget do delete the temp file after use

/**
 * @brief Prepare one file for loading, scan all available plugins (events+filename filtering) 
 *
 * @param filepath : [in, out] Most plugins change this filename
 * @param handler : unpacking handler, to keep to pack again
 * @param attrs : return some info about the Unicode type
 * @param bDoUnicodeJob : flag because loading for DirScan or for MergeDoc differ
 * FALSE : loading into the editor, MergeDoc will do the Unicode conversion
 * just look for handler
 * TRUE : if Unicode BOM (utf-8, ucs-2)  convert it to standard unicode ucs-2 LE
 * no Unicode BOM, look for handler ; if no handler, check that the file is ANSI
 *
 * @return Tells if WinMerge handles this file
 *
 * @note Event FILE_UNPACK
 * Apply only the first correct handler
 */
BOOL FileTransform_Unpacking(CString & filepath, CString filteredText, PackingInfo * handler, int * handlerSubcode);
/**
 * @brief Prepare one file for loading, known handler
 *
 * @param filepath : [in, out] Most plugins change this filename
 */
BOOL FileTransform_Unpacking(CString & filepath, PackingInfo handler, int * handlerSubcode);
/**
 * @brief Prepare one file for saving, known handler
 *
 * @return Tells if we can save the file (really hope we can)
 *
 * @param filepath : [in, out] Most plugins change this filename
 *
 * @note Event FILE_PACK
 * Never do Unicode conversion, it was done in SaveFromFile
 */
BOOL FileTransform_Packing(CString & filepath, PackingInfo handler);

/**
 * @brief Transform formats UCS-2BE and UTF-8 to UCS-2LE (standard OLECHAR in Windows)
 *
 * @param filepath : path of file to be prepared. Must be ANSI or UCS-2LE
 * @param bMayOverwrite : [in] True only if the filepath points out a temp file
 * @param filepath : [in, out] Plugins change this filename if bMayOverwrite is FALSE
 *
 * @return Tells if we can go on with diffutils
 *
 * The output format depends on the input format (we trust the BOM)
 * UCS-2LE, UCS-2BE, UTF-8      -->    UCS-2LE
 * ANSI                         -->    ANSI
 */
BOOL FileTransform_NormalizeUnicode(CString & filepath, CString filteredText, int bMayOverwrite);

/**
 * @brief Prepare one file for diffutils, scan all available plugins (events+filename filtering) 
 *
 * @param filepath : path of file to be prepared. 
 * Input file must be ANSI or UCS-2LE, output file is UTF-8 without BOM.
 * @param bMayOverwrite : [in] True only if the filepath points out a temp file
 * @param filepath : [in, out] Plugins change this filename if bMayOverwrite is FALSE
 *
 * @return Tells if we can go on with diffutils
 *
 * @note Event PREDIFFING
 * Optimization try : call the A or W plugin method according to the status of the file
 * Unicode file                            -> call W (mandatory), fast
 * ANSI file and A plugins available       -> call A, fast
 * ANSI file and A plugins not available   -> convert and call W, slow
 */
BOOL FileTransform_Preprocess(CString & filepath, CString filteredText, int bMayOverwrite);




/**
 * @brief Transform a text (interactive)
 *
 * @param callbackUserChooseFunction  needs a callback : context menu, dialog box...
 *
 * @return Tells if the text has been changed 
 *
 * @note Event CONTEXT_MENU, ?
 */
BOOL TextTransform_Interactive(CString & text, LPCWSTR TransformationEvent,
															 int (*callbackUserChooseFunction) (CStringArray*, void*), void * dataForCallback);


#endif // FileTransform_h
