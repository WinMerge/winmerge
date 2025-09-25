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
	virtual void OnDefaults()
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
		UINT nCtrlID;
		String optionID;
		void* pVar;
		std::function<void(CDataExchange*)> ddfunc; // DDX function
		std::function<void()> readFunc;
		std::function<void()> writeFunc;
		std::function<void()> resetFunc;
	};

	std::vector<OptionBinding> m_bindings;

	/**
	 * @brief Bind option with control + DDX function
	 */
	template<typename T>
	void BindOption(const String& optionID, T& var, UINT nCtrlID, void(__stdcall *ddx)(CDataExchange*, int, T&))
	{
		OptionBinding b{};
		b.nCtrlID = nCtrlID;
		b.optionID = optionID;
		b.pVar = &var;
		b.ddfunc = [ddx, &var, nCtrlID](CDataExchange* pDX) { ddx(pDX, nCtrlID, var); };
		b.readFunc = [this, &var, optionID]() { var = GetOptionsMgr()->GetT<T>(optionID); };
		b.writeFunc = [this, &var, optionID]() { GetOptionsMgr()->SaveOption(optionID, var); };
		b.resetFunc = [this, &var, optionID]() { var = GetOptionsMgr()->GetDefault<T>(optionID); };
		m_bindings.push_back(std::move(b));
	}

	template<typename TVar, typename TOpt = TVar, typename ReadConv, typename WriteConv>
	void BindOptionCustom(const String& optionID, TVar& var, UINT nCtrlID, void(__stdcall *ddx)(CDataExchange*, int, TVar&),
		ReadConv readconv, WriteConv writeconv)
	{
		OptionBinding b{};
		b.nCtrlID = nCtrlID;
		b.optionID = optionID;
		b.pVar = &var;
		b.ddfunc = [ddx, &var, nCtrlID](CDataExchange* pDX) { ddx(pDX, nCtrlID, var); };
		b.readFunc = [this, &var, optionID, readconv]() { var = readconv(GetOptionsMgr()->GetT<TOpt>(optionID)); };
		b.writeFunc = [this, &var, optionID, writeconv]() { GetOptionsMgr()->SaveOption(optionID, writeconv(var)); };
		b.resetFunc = [this, &var, optionID, readconv]() { var = readconv(GetOptionsMgr()->GetDefault<TOpt>(optionID)); };
		m_bindings.push_back(std::move(b));
	}

	/** @brief Call in DoDataExchange */
	void DoDataExchangeBindOptions(CDataExchange* pDX)
	{
		for (auto& b : m_bindings)
			b.ddfunc(pDX);
	}

	/** @brief Read values from OptionsMgr to variables */
	void ReadOptionBindings()
	{
		for (auto& b : m_bindings)
			b.readFunc();
	}

	/** @brief Write values from variables to OptionsMgr */
	void WriteOptionBindings()
	{
		for (auto& b : m_bindings)
			b.writeFunc();
	}

	/** @brief Reset options to default values */
	void ResetOptionBindings()
	{
		for (auto& b : m_bindings)
			b.resetFunc();
	}

	/** @brief Sets update handlers for dialog controls.*/
	void DoDataExchange(CDataExchange* pDX) override
	{
		CPropertyPage::DoDataExchange(pDX);
		DoDataExchangeBindOptions(pDX);
	}

private:
	COptionsMgr * m_pOptionsMgr; /**< Options-manager for storage */
};
