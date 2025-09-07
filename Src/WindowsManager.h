/**
 * @brief Manages MDI child windows within an MDI application.
 */
#pragma once

class CWindowsManager
{
public:
	void AddChildFrame(CMDIChildWnd* pWnd);
	void RemoveChildFrame(CMDIChildWnd* pWnd);
	void ActivateChildFrame(CMDIChildWnd* pChildWnd);
	void ChildFrameActivated(CMDIChildWnd* pChildWnd);
	INT_PTR GetChildCount() const { return m_arrChild.GetSize(); }
	const CTypedPtrArray<CPtrArray, CMDIChildWnd*>& GetChildArray() const { return m_arrChild; }
	void ShowDialog(CMDIFrameWnd* pFrame);
private:
	CTypedPtrArray<CPtrArray, CMDIChildWnd*> m_arrChild;
};
