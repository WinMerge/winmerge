/*!
  \file    AppVersion.h
  \author  Perry Rapp, Smartronix, 2001
  \date    Created: 2001/09/15 (Perry Rapp)
  \date    Edited:  2001/12/12 PR

  \brief   Declaration of simple version functions for an MS-Win app.

  This was built by combining cfilever.*, JulianVersion.*, and VersionReport.*
  on 2001/09/15 by Perry Rapp.

  It requires a CString an a COleDateTime class.
*/
/* The MIT License
Copyright (c) 2001 Perry Rapp
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/


#ifndef AppVersion_h
#define AppVersion_h

namespace appv {

// -- from cfilever.h
/// Get the version number from the fixed (language independent) portion of the version resource for the current module
CString FixedModuleVersion(HMODULE hmod=NULL);
/// Get the version string from the English portion of the version resource for the current module
CString StringModuleVersion();

// -- from JulianVersion.h
bool ConvertDateToWord(const COleDateTime& date, int & word);
bool ConvertDateStringToWord(const CString& sDate, int & word);
bool ConvertWordToDate(int word, COleDateTime & date);
bool ConvertVersionStringToDate(const CString& sVersion, COleDateTime & date);

// -- from VersionReport.h
CString GetVersionReport();

} // namespace

#endif // AppVersion_h
