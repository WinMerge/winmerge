/** 
 * @file  locality.h
 *
 * @brief Declaration of helper functions involving locale
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#ifndef locality_h_included
#define locality_h_included

namespace locality {

String NumToLocaleStr(int n);
String NumToLocaleStr(__int64 n);
String GetLocaleStr(LPCTSTR str, int decimalDigits = 0);

};

#endif // locality_h_included
