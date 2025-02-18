/**
 * @file IniOptionsMgr.h
 *
 * @brief Implementation of Ini file Options management class.
 *
 */
#pragma once

#include "OptionsMgr.h"

class COptionsMgr;

/**
 * @brief Ini-based implementation of OptionsMgr interface (q.v.).
 */
class CIniOptionsMgr : public COptionsMgr
{
public:
	explicit CIniOptionsMgr(const String& filePath);
	virtual ~CIniOptionsMgr();
	CIniOptionsMgr(const CIniOptionsMgr&) = delete;
	CIniOptionsMgr& operator=(const CIniOptionsMgr&) = delete;

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
	int LoadValueFromBuf(const String& strName, const String& textValue, varprop::VariantValue& value);

private:
	bool m_serializing;
	std::map<String, String> m_iniFileKeyValues;
	class IOHandler;
	std::unique_ptr<IOHandler> m_pIOHandler;
};
