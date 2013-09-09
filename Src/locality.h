/** 
 * @file  locality.h
 *
 * @brief Declaration of helper functions involving locale
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#ifndef locality_h_included
#define locality_h_included

#include "UnicodeString.h"
#include <boost/cstdint.hpp>

namespace locality {

String NumToLocaleStr(int n);
String NumToLocaleStr(boost::int64_t n);
String GetLocaleStr(const TCHAR *str, int decimalDigits = 0);
String TimeString(const boost::int64_t * tim);

};

#endif // locality_h_included
