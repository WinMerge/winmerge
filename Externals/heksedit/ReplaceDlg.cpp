#include "precomp.h"
#include "resource.h"
#include "hexwnd.h"
#include "hexwdlg.h"
#include "BinTrans.h"

// String containing data to replace.
SimpleString ReplaceDlg::strToReplaceData;
// String containing data to replace with.
SimpleString ReplaceDlg::strReplaceWithData;

//-------------------------------------------------------------------
// Translate the text in the string to binary data and store in the array.
int ReplaceDlg::transl_text_to_binary(SimpleArray<char> &out)
{
	char *pcOut;
	int destlen = create_bc_translation(&pcOut,
		(char *)strReplaceWithData, strReplaceWithData.StrLen());
	if (destlen)
		out.Adopt(pcOut, destlen - 1, destlen);
	return destlen;
}

//-------------------------------------------------------------------
// Create a text representation of an array of bytes and save it in
// a SimpleString object.
int	ReplaceDlg::transl_binary_to_text(char *src, int len)
{
	// How long will the text representation of array of bytes be?
	int destlen = Text2BinTranslator::iBytes2BytecodeDestLen(src, len);
	strToReplaceData.SetSize(destlen);
	strToReplaceData.ExpandToSize();
	if (char *pd = strToReplaceData)
	{
		Text2BinTranslator::iTranslateBytesToBC(pd, (unsigned char*) src, len);
		return TRUE;
	}
	return FALSE;
}

//-------------------------------------------------------------------
// Following code by R. Kibria.
int ReplaceDlg::find_and_select_data(int finddir, char (*cmp)(char))
{
	char *tofind;
	// Create a translation from bytecode to char array of finddata.
	int destlen = create_bc_translation(&tofind, strToReplaceData, strToReplaceData.StrLen());
	int i;
	if (finddir >= 0)
	{
		// Find forward.
		i = find_bytes((char *)&DataArray[iCurByte + finddir],
			DataArray.GetLength() - iCurByte - 1,
			tofind,	destlen, 1, cmp );
		if (i != -1)
			iCurByte += i + finddir;
	}
	else
	{
		// Find backward.
		i = find_bytes((char *)&DataArray[0],
			min(iCurByte + (destlen - 1), DataArray.GetLength()),
			tofind, destlen, -1, cmp );
		if (i != -1)
			iCurByte = i;
	}
	int done = 0;
	if (i != -1)
	{
		// NEW: Select found interval.
		bSelected = TRUE;
		iStartOfSelection = iCurByte;
		iEndOfSelection = iCurByte + destlen - 1;
		done = 1;
	}
	delete [] tofind;
	return done;
}

//-------------------------------------------------------------------
// SimpleString replacedata contains data to replace with.
int ReplaceDlg::replace_selected_data(HWND hDlg)
{
	if (!bSelected)
	{
		MessageBox(hDlg, "Data to replace must be selected.", "Replace", MB_ICONERROR);
		return FALSE;
	}
	int i = iGetStartOfSelection();
	int n = iGetEndOfSelection() - i + 1;
	if (strReplaceWithData.IsEmpty())
	{
		// Selected data is to be deleted, since replace-with data is empty string.
		if (!DataArray.Replace(i, n, 0, 0))
		{
			MessageBox(hDlg, "Could not delete selected data.", "Replace", MB_ICONERROR);
			return FALSE;
		}
		bSelected = FALSE;
		iCurByte = iStartOfSelection;
	}
	else if (iPasteAsText)
	{
		// Replace with non-zero-length data.
		if (!DataArray.Replace(i, n, (unsigned char *)(char *)strReplaceWithData, strReplaceWithData.StrLen()))
		{
			MessageBox(hDlg, "Replacing failed.", "Replace", MB_ICONERROR);
			return FALSE;
		}
		iEndOfSelection = iStartOfSelection + strReplaceWithData.StrLen() - 1;
	}
	else
	{
		// Input string contains special-syntax-coded binary data.
		SimpleArray<char> out;
		if (!transl_text_to_binary(out))
		{
			MessageBox(hDlg, "Could not translate text to binary.", "Replace", MB_ICONERROR);
			return FALSE;
		}
		if (!DataArray.Replace(i, n, (unsigned char*)(char*)out, out.GetLength()))
		{
			MessageBox(hDlg, "Replacing failed.", "Replace", MB_ICONERROR);
			return FALSE;
		}
		iEndOfSelection = iStartOfSelection + out.GetLength() - 1;
	}
	bFilestatusChanged = TRUE;
	iFileChanged = TRUE;
	return TRUE;
}

//-------------------------------------------------------------------
INT_PTR ReplaceDlg::DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg)
	{
	case WM_INITDIALOG:
		// If there is selected data then make it the data to find.
		if (bSelected)
		{
			int sel_start, select_len;
			if( iEndOfSelection < iStartOfSelection )
			{
				sel_start = iEndOfSelection;
				select_len = iStartOfSelection - iEndOfSelection + 1;
			}
			else
			{
				sel_start = iStartOfSelection;
				select_len = iEndOfSelection - iStartOfSelection + 1;
			}

			if (transl_binary_to_text((char *)&DataArray[sel_start], select_len))
			{
			}
			else
			{
				MessageBox(hDlg, "Could not use selection as replace target.", "Replace", MB_OK);
				EndDialog(hDlg, IDCANCEL);
				return TRUE;
			}
		}
		CheckDlgButton(hDlg, IDC_USETRANSLATION_CHECK, !iPasteAsText);
		if (char *pstr = strToReplaceData)
			SetDlgItemText(hDlg, IDC_TO_REPLACE_EDIT, pstr);
		if (char *pstr = strReplaceWithData)
			SetDlgItemText(hDlg, IDC_REPLACEWITH_EDIT, pstr);
		return TRUE;

	case WM_COMMAND:
		switch (wParam)
		{
		case IDCANCEL:
			iPasteAsText = !IsDlgButtonChecked(hDlg, IDC_USETRANSLATION_CHECK);
			EndDialog(hDlg, wParam);
			return TRUE;

		case IDC_FINDPREVIOUS_BUTTON:
			{
				char (*cmp)(char) = IsDlgButtonChecked(hDlg, IDC_MATCHCASE_CHECK) ? equal : lower_case;
				GetDlgItemText(hDlg, IDC_TO_REPLACE_EDIT, strToReplaceData);
				if (find_and_select_data(-1, cmp))
				{
					adjust_view_for_selection();
					repaint();
					synch_sibling();
				}
				else
				{
					MessageBox(hDlg, "Could not find data.", "Replace/Find backward", MB_ICONWARNING);
				}
			}
			break;

		case IDC_FINDNEXT_BUTTON:
			{
				char (*cmp)(char) = IsDlgButtonChecked(hDlg, IDC_MATCHCASE_CHECK) ? equal : lower_case;
				GetDlgItemText(hDlg, IDC_TO_REPLACE_EDIT, strToReplaceData);
				if (find_and_select_data(1, cmp))
				{
					adjust_view_for_selection();
					repaint();
					synch_sibling();
				}
				else
				{
					MessageBox(hDlg, "Could not find data.", "Replace/Find forward", MB_ICONWARNING);
				}
			}
			break;

		// Replace all following occurances of the findstring.
		case IDC_FOLLOCC_BUTTON:
			{
				char (*cmp)(char) = IsDlgButtonChecked(hDlg, IDC_MATCHCASE_CHECK) ? equal : lower_case;
				GetDlgItemText(hDlg, IDC_TO_REPLACE_EDIT, strToReplaceData);
				GetDlgItemText(hDlg, IDC_REPLACEWITH_EDIT, strReplaceWithData);
				int occ_num = 0;
				iPasteAsText = !IsDlgButtonChecked(hDlg, IDC_USETRANSLATION_CHECK);
				//------------------
				// Don't do anything if to-replace and replace-with data are same.
				Text2BinTranslator tr_find(strToReplaceData), tr_replace(strReplaceWithData);
				if (tr_find.bCompareBin(tr_replace, iCharacterSet, iBinaryMode))
				{
					MessageBox(hDlg, "To-replace and replace-with data are same.", "Replace all following occurances", MB_ICONERROR);
					break;
				}

				SetCursor( LoadCursor( NULL, IDC_WAIT ) );
				while (find_and_select_data(0, cmp))
				{
					occ_num++;
					replace_selected_data(hDlg);
				};
				SetCursor( LoadCursor( NULL, IDC_ARROW ) );

				set_wnd_title();
				adjust_view_for_selection();
				resize_window();
				synch_sibling();

				char tbuf[80];
				sprintf(tbuf, "%d occurences replaced.", occ_num);
				MessageBox(hDlg, tbuf, "Replace/Replace all following", MB_ICONINFORMATION);
			}
			break;

		case IDC_PREVOCC_BUTTON:
			{
				// Replace all previous occurances of the findstring.
				char (*cmp)(char) = IsDlgButtonChecked(hDlg, IDC_MATCHCASE_CHECK) ? equal : lower_case;
				GetDlgItemText(hDlg, IDC_TO_REPLACE_EDIT, strToReplaceData);
				GetDlgItemText(hDlg, IDC_REPLACEWITH_EDIT, strReplaceWithData);
				int occ_num = 0;
				iPasteAsText = !IsDlgButtonChecked(hDlg, IDC_USETRANSLATION_CHECK);
				// Don't do anything if to-replace and replace-with data are same.
				Text2BinTranslator tr_find(strToReplaceData), tr_replace(strReplaceWithData);
				if (tr_find.bCompareBin(tr_replace, iCharacterSet, iBinaryMode))
				{
					MessageBox(hDlg, "To-replace and replace-with data are same.", "Replace all following occurances", MB_ICONERROR);
					break;
				}

				SetCursor( LoadCursor( NULL, IDC_WAIT ) );
				while (find_and_select_data(-1, cmp))
				{
					occ_num++;
					replace_selected_data(hDlg);
				};
				SetCursor( LoadCursor( NULL, IDC_ARROW ) );

				set_wnd_title();
				adjust_view_for_selection();
				resize_window();
				synch_sibling();

				char tbuf[32];
				sprintf(tbuf, "%d occurances replaced.", occ_num);
				MessageBox(hDlg, tbuf, "Replace/Replace all following", MB_ICONINFORMATION);
			}
			break;

		case IDC_REPLACE_BUTTON:
			iPasteAsText = !IsDlgButtonChecked(hDlg, IDC_USETRANSLATION_CHECK);
			GetDlgItemText(hDlg, IDC_TO_REPLACE_EDIT, strToReplaceData);
			GetDlgItemText(hDlg, IDC_REPLACEWITH_EDIT, strReplaceWithData);
			replace_selected_data(hDlg);
			set_wnd_title();
			adjust_view_for_selection();
			resize_window();
			synch_sibling();
			break;
		}
		break;
	}
	return FALSE;
}
