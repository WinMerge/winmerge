/*!
  \file    CSubclass.h
  \author  Perry Rapp, Smartronix
  \date    Created: 1998
  \date    Edited: 2001/12/12 PR

  \brief   Subclass library - functions to subclass any window proc


 Multiple subclasses of a given window proc are allowed, as long as each is a different
 subclass window proc. A second subclass attempt with same window proc will fail --
 Subclass(..) will return false.

 Client may optionally specify unsubclass messages, if client wants to be notified
 after the subclass has succeeded or failed.
*/
/* The MIT License
Copyright (c) 2001 Perry Rapp
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/


#ifndef CSubclass_h
#define CSubclass_h

namespace prdlg {

//! post-subclass message (CallOldProc does not forward these)
// note that if another subclass removes ours violently, these will not get sent
struct Msg
{
	UINT msg; // must be non-zero to be used
	WPARAM wparam;
	LPARAM lparam;
};

	//! Client calls Subclass(..) to install new window proc
bool Subclass(WNDPROC wndproc, HWND hwnd, void * data
	, Msg * unsubclassSucceeded = NULL, Msg * unsubclassFailed = NULL);

	//! Client calls UnSubclass(..) to remove existing window proc subclass
	// this does NOT trigger post-subclass messages
bool UnSubclass(WNDPROC id, HWND hwnd);

	//! Client must call CallOldProc(..) at end of subclassing proc
LRESULT CallOldProc(WNDPROC id, HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	//! Client calls GetData(..) to retrieve data pointer passed in its Subclass(..) call
void * GetData(WNDPROC id, HWND hwnd);


} // namespace

#endif CSubclass_h

