/**
 *  @file   UniFile.h
 *  @author Perry Rapp, Creator, 2003-2006
 *  @date   Created: 2003-10
 *  @date   Edited:  2006-02-20 (Perry Rapp)
 *
 *  @brief  Declaration of Unicode file classes.
 */
#pragma once

#include "unicoder.h"
#include <cstdio>

namespace Poco { class SharedMemory; }

/**
 * @brief Interface to file classes in this module
 */
class UniFile
{
public:

	/**
	 * @brief A struct for error message or error code.
	 */
	struct UniError
	{
		String desc; // valid if apiname empty

		UniError();
		bool HasError() const;
		void ClearError();
		String GetError() const;
	};

	virtual ~UniFile() { }
	virtual bool OpenReadOnly(const String& filename) = 0;
	virtual void Close() = 0;
	virtual bool IsOpen() const = 0;

	virtual String GetFullyQualifiedPath() const = 0;
	virtual const UniError & GetLastUniError() const = 0;

	virtual bool IsUnicode() = 0;
	virtual bool ReadBom() = 0;
	virtual bool HasBom() const = 0;
	virtual void SetBom(bool bom) = 0;

	virtual ucr::UNICODESET GetUnicoding() const = 0;
	virtual void SetUnicoding(ucr::UNICODESET unicoding) = 0;
	virtual int GetCodepage() const = 0;
	virtual void SetCodepage(int codepage) = 0;

public:
	virtual bool ReadString(String & line, bool * lossy) = 0;
	virtual bool ReadString(String & line, String & eol, bool * lossy) = 0;
	virtual bool ReadStringAll(String & line) = 0;
	virtual int GetLineNumber() const = 0;
	virtual int64_t GetPosition() const = 0;
	virtual bool WriteString(const String & line) = 0;

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
 * @brief Check if there is error.
 * @return true if there is an error.
 */
inline bool UniFile::UniError::HasError() const
{
	return !desc.empty();
}

/**
 * @brief Clears the existing error.
 */
inline void UniFile::UniError::ClearError()
{
	desc.clear();
}

/**
 * @brief Get the error string.
 * @return Error string.
 */
inline String UniFile::UniError::GetError() const
{
	return desc;
}

/**
 * @brief Local file access code used by both UniMemFile and UniStdioFile
 *
 * This class lacks an actual handle to a file
 */
class UniLocalFile : public UniFile
{
public:
	UniLocalFile();
	void Clear();

	virtual String GetFullyQualifiedPath() const override { return m_filepath; }
	virtual const UniError & GetLastUniError() const override { return m_lastError; }

	virtual ucr::UNICODESET GetUnicoding() const override { return m_unicoding; }
	virtual void SetUnicoding(ucr::UNICODESET unicoding) override { m_unicoding = unicoding; }
	virtual int GetCodepage() const override { return m_codepage; }
	virtual void SetCodepage(int codepage) override { 
		m_codepage = codepage;
		switch (m_codepage)
		{
		case ucr::CP_UCS2LE:
			m_unicoding = ucr::UCS2LE;
			m_charsize = 2;
			break;
		case ucr::CP_UCS2BE:
			m_unicoding = ucr::UCS2BE;
			m_charsize = 2;
			break;
		case ucr::CP_UTF_8:
			m_charsize = 1;
			m_unicoding = ucr::UTF8;
			break;
		default:
			m_charsize = 1;
			m_unicoding = ucr::NONE;
			break;
		}
	}

	virtual int GetLineNumber() const override { return m_lineno; }
	virtual const txtstats & GetTxtStats() const override { return m_txtstats; }
	virtual int64_t GetFileSize() const{ return m_filesize; }

	bool IsUnicode() override;

protected:
	virtual bool DoGetFileStatus();
	virtual void LastErrorCustom(const String& desc);

protected:
	int m_statusFetched; // 0 not fetched, -1 error, +1 success
	int m_lineno; // current 0-based line of m_current
	int64_t m_filesize;
	String m_filepath;
	String m_filename;
	UniError m_lastError;
	int m_charsize; // 2 for UCS-2, else 1
	int m_codepage; // only valid if m_unicoding==ucr::NONE;
	txtstats m_txtstats;
	ucr::UNICODESET m_unicoding;
	bool m_bom; /**< Did the file have a BOM when reading? */
	bool m_bUnicodingChecked; /**< Has unicoding been checked for the file? */
	bool m_bUnicode; /**< Is the file unicode file? */
};

/**
 * @brief Memory-Mapped disk file (read-only access)
 */
class UniMemFile : public UniLocalFile
{
public:
	enum AccessMode
	{
		AM_READ = 0,
		AM_WRITE
	};

	UniMemFile();
	virtual ~UniMemFile() { Close(); }

	virtual bool GetFileStatus();

	virtual bool OpenReadOnly(const String& filename) override;
	virtual bool Open(const String& filename);
	virtual bool Open(const String& filename, AccessMode mode);
	void Close() override;
	virtual bool IsOpen() const override;

	virtual bool ReadBom() override;
	virtual bool HasBom() const override;
	virtual void SetBom(bool bom) override;

public:
	virtual bool ReadString(String & line, bool * lossy) override;
	virtual bool ReadString(String & line, String & eol, bool * lossy) override;
	virtual bool ReadStringAll(String & line) override;
	virtual int64_t GetPosition() const override { return m_current - m_base; }
	virtual bool WriteString(const String & line) override;
	unsigned char* GetBase() const { return m_base; }

// Implementation methods
protected:
	virtual bool DoOpen(const String& filename, AccessMode mode);

// Implementation data
private:
	Poco::SharedMemory *m_hMapping;
	unsigned char *m_base; // points to base of mapping
	unsigned char *m_data; // similar to m_base, but after BOM if any
	unsigned char *m_current; // current location in file
};

/** @brief Is it currently attached to a file ? */
inline bool UniMemFile::IsOpen() const
{
	// We don't test the handle here, because we allow "opening" empty file
	// but memory-mapping doesn't work on that, so that uses a special state
	// of no handle, but linenumber of 0
	return m_lineno >= 0;
}

/**
 * @brief Returns if file has a BOM bytes.
 * @return true if file has BOM bytes, false otherwise.
 */
inline bool UniMemFile::HasBom() const
{
	return m_bom;
}

/**
 * @brief Sets if file has BOM or not.
 * @param [in] true to have a BOM in file, false to not to have.
 */
inline void UniMemFile::SetBom(bool bom)
{
	m_bom = bom;
}

/**
 * @brief Regular buffered file (write-only access)
 * (ReadString methods have never been implemented,
 *  because UniMemFile above is good for reading)
 */
class UniStdioFile : public UniLocalFile
{
public:
	UniStdioFile();
	~UniStdioFile();

	virtual bool GetFileStatus();

	virtual bool OpenReadOnly(const String& filename) override;
	virtual bool OpenCreate(const String& filename);
	virtual bool OpenCreateUtf8(const String& filename);
	virtual bool Open(const String& filename, const String& mode);
	virtual bool SetVBuf(int mode, size_t size);
	void Close() override;

	virtual bool IsOpen() const override;

	virtual bool ReadBom() override;
	virtual bool HasBom() const override;
	virtual void SetBom(bool bom) override;

protected:
	virtual bool ReadString(String & line, bool * lossy) override;
	virtual bool ReadString(String & line, String & eol, bool * lossy) override;
	virtual bool ReadStringAll(String & line) override;

public:
	virtual int64_t GetPosition() const override;

	virtual int WriteBom();
	virtual bool WriteString(const String & line) override;

// Implementation methods
protected:
	virtual bool DoOpen(const String& filename, const String& mode);
	virtual void LastErrorCustom(const String& desc) override;

// Implementation data
private:
	FILE * m_fp;
	int64_t m_data; // offset after any initial BOM
	ucr::buffer m_ucrbuff;
};

/** @brief Is it currently attached to a file ? */
inline bool UniStdioFile::IsOpen() const
{
	return m_fp != 0;
}

/**
 * @brief Returns if file has a BOM bytes.
 * @return true if file has BOM bytes, false otherwise.
 */
inline bool UniStdioFile::HasBom() const
{
	return m_bom;
}

/**
 * @brief Sets if file has BOM or not.
 * @param [in] true to have a BOM in file, false to not to have.
 */
inline void UniStdioFile::SetBom(bool bom)
{
	m_bom = bom;
}


