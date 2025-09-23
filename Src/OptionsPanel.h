/** 
 * @file  OptionsPanel.h
 *
 * @brief Declaration file for OptionsPanel class.
 *
 */
#pragma once

#include "IOptionsPanel.h"
#include "TrDialogs.h"
#include <vector>
#include <functional>
#include <afxwin.h>

class COptionsMgr;

/**
 * @brief A base class for WinMerge options dialogs.
 */
class OptionsPanel : public CTrPropertyPage, public IOptionsPanel
{
public:
	OptionsPanel(COptionsMgr *optionsMgr, UINT nIDTemplate);

	/** 
	 * @brief Reads options values from storage to UI.
	 */
	void ReadOptions() override
	{
		ReadOptionBindings();
	}

	/** 
	 * @brief Writes options values from UI to storage.
	 */
	void WriteOptions() override
	{
		WriteOptionBindings();
	}

	/**
	 * @brief Sets options to defaults
	 */
	void OnDefaults()
	{
		ResetOptionBindings();
		UpdateData(FALSE);
	}

protected:
	COptionsMgr* GetOptionsMgr() const { return m_pOptionsMgr; }

	// -------------------------------
	// Option binding support
	// -------------------------------

	struct OptionBinding
	{
		UINT nCtrlID; // 0 if no control
		String optionID;
		void* pVar;
		std::function<void(CDataExchange*)> ddfunc; // DDX function
		std::function<void()> readFunc;  // optional custom read/write
		std::function<void()> writeFunc;
		std::function<void()> resetFunc;
	};

	std::vector<OptionBinding> m_bindings;

	/**
	 * @brief Bind option with control + DDX function
	 */
	template<typename T>
	void BindOption(const String& optionID, T& var, UINT nCtrlID, void(*ddx)(CDataExchange*, int, T&))
	{
		OptionBinding b{};
		b.nCtrlID = nCtrlID;
		b.optionID = optionID;
		b.pVar = &var;
		T* pVar = &var;
		String optID = optionID;
		b.ddfunc = [ddx, pVar, nCtrlID](CDataExchange* pDX) { ddx(pDX, nCtrlID, *pVar); };
		b.readFunc = [this, pVar, optID]() {
			if constexpr (std::is_same_v<T, int>)
				*pVar = GetOptionsMgr()->GetInt(optID);
			else if constexpr (std::is_same_v<T, bool>)
				*pVar = GetOptionsMgr()->GetBool(optID);
			else if constexpr (std::is_same_v<T, String>)
				*pVar = GetOptionsMgr()->GetString(optID);
			else
				static_assert(always_false<T>, "Unsupported option type");
		};
		b.writeFunc = [this, pVar, optID]() { GetOptionsMgr()->SaveOption(optID, *pVar); };
		b.resetFunc = [this, pVar, optID]() { *pVar = GetOptionsMgr()->GetDefault<T>(optID); };
		m_bindings.push_back(std::move(b));
	}

	/**
	 * @brief Bind option without control
	 */
	template<typename T>
	void BindOption(const String& optionID, T& var)
	{
		OptionBinding b{};
		b.nCtrlID = 0;
		b.optionID = optionID;
		b.pVar = &var;
		T* pVar = &var;
		String optID = optionID;
		b.readFunc = [this, pVar, optID]() { *pVar = GetOptionsMgr()->Get<T>(optID); };
		b.writeFunc = [this, pVar, optID]() { GetOptionsMgr()->SaveOption(optID, *pVar); };
		m_bindings.push_back(std::move(b));
	}

	/** @brief Call in DoDataExchange */
	void DoDataExchangeBindOptions(CDataExchange* pDX)
	{
		for (auto& b : m_bindings)
		{
			if (b.ddfunc && b.nCtrlID != 0)
				b.ddfunc(pDX);
		}
	}

	/** @brief Read values from OptionsMgr to variables */
	void ReadOptionBindings()
	{
		for (auto& b : m_bindings)
		{
			if (b.readFunc)
				b.readFunc();
		}
	}

	/** @brief Write values from variables to OptionsMgr */
	void WriteOptionBindings()
	{
		for (auto& b : m_bindings)
		{
			if (b.writeFunc)
				b.writeFunc();
		}
	}

	/** @brief Reset options to default values */
	void ResetOptionBindings()
	{
		for (auto& b : m_bindings)
		{
			if (b.resetFunc)
				b.resetFunc();
		}
	}

private:
	COptionsMgr * m_pOptionsMgr; /**< Options-manager for storage */
};
