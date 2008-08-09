#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"

BOOL MoveCopyDlg::OnInitDialog(HWND hw)
{
	char buf[30];
	int iMove1stEnd = iGetStartOfSelection();
	int iMove2ndEndorLen = iGetEndOfSelection();
	sprintf(buf, "x%x", iMove1stEnd);
	SetDlgItemText (hw, IDC_1STOFFSET, buf);
	sprintf(buf, "x%x", iMove2ndEndorLen);
	SetDlgItemText (hw, IDC_2NDDELIM, buf);
	CheckDlgButton (hw, IDC_OTHEREND, BST_CHECKED);
	sprintf(buf, "x%x", iMovePos);
	SetDlgItemText(hw, IDC_MOVEMENT, buf);
	CheckDlgButton(hw, IDC_FPOS, BST_CHECKED);
	if (iMoveOpTyp == OPTYP_MOVE)
		CheckDlgButton(hw, IDC_MOVE, BST_CHECKED);
	else if (iMoveOpTyp == OPTYP_COPY)
		CheckDlgButton(hw, IDC_COPY, BST_CHECKED);
	return TRUE;
}

BOOL MoveCopyDlg::Apply(HWND hw)
{
	char buf[30];
	const int dlgitems[3] = { IDC_1STOFFSET, IDC_2NDDELIM, IDC_MOVEMENT };
	const int check[3] = { 0, IDC_LEN, IDC_FORWARD };
	int vals[3];
	for (int n = 0 ; n < 3; n++)
	{
		HWND cntrl = GetDlgItem(hw, dlgitems[n]);
		GetWindowText(cntrl, buf, sizeof buf);
		int i = 0;
		if (n && buf[i] == '-')
		{
			if (!IsDlgButtonChecked(hw, check[n]))
			{
				MessageBox(hw, "You have chosen an offset but it is negative, which is invalid.", "Move/Copy", MB_ICONERROR);
				return FALSE;
			}
			// Relative jump. Read offset from next character on.
			i++;
		}
		if (sscanf(&buf[i], "x%x", &vals[n]) == 0 &&
			sscanf(&buf[i], "%d", &vals[n]) == 0)
		{
			// No fields assigned: badly formed number.
			char msg[80];
			sprintf(msg, "The value in box number %d cannot be recognized.", n + 1);
			MessageBox(hw, msg, "Move/Copy", MB_ICONERROR);
			return FALSE;
		}
		if (buf[0] == '-')
			vals[n] = -vals[n]; //Negate
	}
	int clen = DataArray.GetLength();
	int iMove1stEnd = vals[0];
	int iMove2ndEndorLen = vals[1];
	if (!IsDlgButtonChecked(hw, IDC_OTHEREND))
	{
		if (iMove2ndEndorLen == 0)
		{
			MessageBox(hw, "Cannot move/copy a block of zero length", "Move/Copy", MB_OK | MB_ICONERROR);
			return 0;
		}
		if (iMove2ndEndorLen > 0)
			iMove2ndEndorLen--;
		else
			iMove2ndEndorLen++;
		iMove2ndEndorLen += iMove1stEnd;
	}

	if (iMove1stEnd < 0 ||
		iMove1stEnd >= clen ||
		iMove2ndEndorLen < 0 ||
		iMove2ndEndorLen >= clen)
	{
		MessageBox(hw, "The chosen block extends into non-existant data.", "Move/Copy", MB_ICONERROR);
		return FALSE;
	}

	if (iMove1stEnd > iMove2ndEndorLen)
		swap(iMove1stEnd, iMove2ndEndorLen);

	if (IsDlgButtonChecked(hw, IDC_FPOS))
		iMovePos = vals[2];
	else if(IsDlgButtonChecked(hw,IDC_LPOS))
		iMovePos = iMove1stEnd + vals[2] - iMove2ndEndorLen;
	else
		iMovePos = iMove1stEnd + vals[2];

	iMoveOpTyp = IsDlgButtonChecked(hw, IDC_MOVE) ? OPTYP_MOVE : OPTYP_COPY;

	if (iMovePos == iMove1stEnd && iMoveOpTyp == OPTYP_MOVE)
	{
		MessageBox(hw, "The block was not moved!", "Move/Copy", MB_ICONEXCLAMATION);
		EndDialog(hw,0);
		return FALSE;
	}

	if (iMovePos < 0 || (iMoveOpTyp == OPTYP_MOVE ?
		iMovePos + iMove2ndEndorLen - iMove1stEnd >= clen :
		iMovePos > clen))
	{
		MessageBox(hw, "Cannot move/copy the block outside the data", "Move/Copy", MB_OK | MB_ICONERROR);
		return FALSE;
	}
	CMD_move_copy(iMove1stEnd, iMove2ndEndorLen, true);
	return TRUE;
}

BOOL MoveCopyDlg::OnCommand(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case IDOK:
		if (Apply(hDlg))
		{
		case IDCANCEL:
			EndDialog(hDlg, wParam);
		}
		return TRUE;
	}
	return FALSE;
}

INT_PTR MoveCopyDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		return OnInitDialog(hDlg);
	case WM_COMMAND:
		return OnCommand(hDlg, wParam, lParam);
	}
	return FALSE;
}
