/*
Frhed - Free hex editor
Copyright (C) 2000 Raihan Kibria

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License, version 2, as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.

Last change: 2013-04-09 by Jochen Neubeck
*/
/** 
 * @file  OSTools.cpp
 *
 * @brief Implementation OS tool functions.
 *
 */
#include "precomp.h"
#include "OSTools.h"
#include "DllProxies.h"

/**
 * @brief Check if current user has Administrator privileges.
 * @note This check applies only for NT-based Windows. For W9X Windows this
 *   function always returns TRUE.
 * @return TRUE if user has Administrator privileges, FALSE otherwise.
 */
BOOL ostools_HaveAdminAccess()
{
	BOOL isadmin = TRUE;
	if (struct ADVAPI32 *ADVAPI32 = ::ADVAPI32)
	{
		SID_IDENTIFIER_AUTHORITY ntauth = SECURITY_NT_AUTHORITY;
		PSID admgroup = NULL;
		if (ADVAPI32->AllocateAndInitializeSid(&ntauth, 2,
			SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS,
			0, 0, 0, 0, 0, 0, &admgroup))
		{
			if (!ADVAPI32->CheckTokenMembership(NULL, admgroup, &isadmin))
			{
				isadmin = FALSE;
			}
			ADVAPI32->FreeSid(admgroup);
		}
	}
	return isadmin;
}
