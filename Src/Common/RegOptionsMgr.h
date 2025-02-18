/**
 * @file RegOptionsMgr.h
 *
 * @brief Declaration for Registry options class.
 *
 */
#pragma once

#include "OptionsMgr.h"

class COptionsMgr;

/**
 * @brief Registry-based implementation of OptionsMgr interface (q.v.).
 */
class CRegOptionsMgr : public COptionsMgr
{
public:
	explicit CRegOptionsMgr(const String& path);
	virtual ~CRegOptionsMgr();
	CRegOptionsMgr(const CRegOptionsMgr&) = delete;
	CRegOptionsMgr& operator=(const CRegOptionsMgr&) = delete;

	void CloseKeys();

	virtual int InitOption(const String& name, const varprop::VariantValue& defaultValue) override;
	virtual int InitOption(const String& name, const String& defaultValue) override;
	virtual int InitOption(const String& name, const tchar_t* defaultValue) override;
	virtual int InitOption(const String& name, int defaultValue, bool serializable = true) override;
	virtual int InitOption(const String& name, bool defaultValue) override;

	virtual int SaveOption(const String& name) override;
	virtual int SaveOption(const String& name, const varprop::VariantValue& value) override;
	virtual int SaveOption(const String& name, const String& value) override;
	virtual int SaveOption(const String& name, const tchar_t* value) override;
	virtual int SaveOption(const String& name, int value) override;
	virtual int SaveOption(const String& name, bool value) override;

	virtual int RemoveOption(const String& name) override;

	virtual int FlushOptions() override;

	virtual int ExportOptions(const String& filename, const bool bHexColor=false) const override;
	virtual int ImportOptions(const String& filename) override;

	virtual void SetSerializing(bool serializing = true) override { m_serializing = serializing; }

protected:
	int LoadValueFromBuf(const String& strName, unsigned type, const unsigned char* data, varprop::VariantValue& value);

private:
	bool m_serializing;
	class IOHandler;
	std::unique_ptr<IOHandler> m_pIOHandler;
};
