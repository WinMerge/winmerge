/** 
 *  @file   UniFile.h
 *  @author Perry Rapp, Creator, 2003
 *  @date   Created: 2003-10 (Perry)
 *  @date   Edited:  2003-11-15 (Perry)
 *
 *  @brief  Declaration of Memory-Mapped Unicode enabled file class
 */

#ifndef UniFile_h_included
#define UniFile_h_included

/**
 * @brief Interface to file classes in this module
 */
class UniFile
{
public:
	struct UniError
	{
		CString apiname;
		int syserrnum; // valid if apiname nonempty
		CString desc; // valid if apiname empty
		bool hasError() const { return !apiname.IsEmpty() || !desc.IsEmpty(); }
		void ClearError() { apiname = _T(""); syserrnum = ERROR_SUCCESS; desc = _T(""); }
		UniError() { ClearError(); }
	};

	virtual bool OpenReadOnly() = 0;

	virtual void Close() = 0;

	virtual CString GetFullyQualifiedPath() const = 0;

	virtual const UniError & GetLastUniError() const = 0;

	virtual bool ReadBom() = 0;
	virtual int GetUnicoding() const = 0;
	virtual void SetUnicoding(int unicoding) = 0;

	virtual int GetCodepage() const = 0;
	virtual void SetCodepage(int codepage) = 0;

	virtual BOOL ReadString(CString & line) = 0;
	virtual BOOL ReadString(CString & line, CString & eol) = 0;
	virtual int GetLineNumber() const = 0;
	virtual __int64 GetPosition() const = 0;

	struct txtstats
	{
		int ncrs;
		int nlfs;
		int ncrlfs;
		int nzeros;
		int nlosses;
		txtstats() { clear(); }
		void clear() { ncrs = nlfs = ncrlfs = nzeros = nlosses = 0; }
	};
	virtual const txtstats & GetTxtStats() const = 0;
};

/**
 * @brief Memory-Mapped disk file (read-only access)
 */
class UniMemFile : public UniFile
{
public:
	UniMemFile(LPCTSTR filename);
	virtual ~UniMemFile() { Close(); }

	virtual bool GetFileStatus();

	virtual bool OpenReadOnly();
	virtual bool Open();
	virtual bool Open(DWORD dwOpenAccess, DWORD dwOpenShareMode, DWORD dwOpenCreationDispostion, DWORD dwMappingProtect, DWORD dwMapViewAccess);

	void Close();

	virtual CString GetFullyQualifiedPath() const { return m_filepath; }
	const CFileStatus & GetFileStatus() const { return m_filestatus; }

	virtual const UniError & GetLastUniError() const { return m_lastError; }

	virtual bool ReadBom();
	virtual int GetUnicoding() const { return m_unicoding; }
	virtual void SetUnicoding(int unicoding) { m_unicoding = unicoding; }

	virtual int GetCodepage() const { return m_codepage; }
	virtual void SetCodepage(int codepage) { m_codepage = codepage; }

	virtual BOOL ReadString(CString & line);
	virtual BOOL ReadString(CString & line, CString & eol);
	virtual int GetLineNumber() const { return m_lineno; }
	virtual __int64 GetPosition() const { return m_current - m_base; }


	virtual const txtstats & GetTxtStats() const { return m_txtstats; }

// Implementation methods
protected:
	virtual bool DoOpen(DWORD dwOpenAccess, DWORD dwOpenShareMode, DWORD dwOpenCreationDispostion, DWORD dwMappingProtect, DWORD dwMapViewAccess);
	virtual void LastError(LPCTSTR apiname, int syserrnum);
	virtual void LastErrorCustom(LPCTSTR desc);

// Implementation data
private:
	int m_statusFetched; // 0 not fetched, -1 error, +1 success
	CFileStatus m_filestatus;
	__int64 m_filesize;
	CString m_filepath;
	CString m_filename;
	HANDLE m_handle;
	HANDLE m_hMapping;
	LPBYTE m_base; // points to base of mapping
	LPBYTE m_data; // similar to m_base, but after BOM if any
	LPBYTE m_current; // current location in file
	int m_lineno; // current 0-based line of m_current
	UniError m_lastError;
	bool m_readbom; // whether have tested for BOM
	int m_unicoding; // enum UNICODESET in unicoder.h
	int m_charsize; // 2 for UCS-2, else 1
	int m_codepage; // only valid if m_unicoding==ucr::NONE;
	txtstats m_txtstats;
};

#endif // UniFile_h_included
