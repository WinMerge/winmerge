/** 
 * @file  winnt_supp.h
 *
 * @brief stuff from PlatformSDK version of WinNT.h which MSVC6 version lacks
 */
// RCS ID line follows -- this is updated by CVS
// $Id$


#ifndef winnt_supp_h_included
#define winnt_supp_h_included


#ifndef VER_NT_WORKSTATION
#define VER_NT_WORKSTATION              0x0000001
#endif

#ifndef VER_NT_DOMAIN_CONTROLLER
#define VER_NT_DOMAIN_CONTROLLER        0x0000002
#endif

#ifndef VER_NT_SERVER
#define VER_NT_SERVER                   0x0000003
#endif

#ifndef VER_SUITE_ENTERPRISE
#define VER_SUITE_ENTERPRISE                0x00000002
#endif

#ifndef VER_SUITE_DATACENTER
#define VER_SUITE_DATACENTER                0x00000080
#endif

#ifndef VER_SUITE_SINGLEUSERTS
#define VER_SUITE_SINGLEUSERTS              0x00000100
#endif

#ifndef VER_SUITE_PERSONAL
#define VER_SUITE_PERSONAL                  0x00000200
#endif

#ifndef VER_SUITE_BLADE
#define VER_SUITE_BLADE                     0x00000400
#endif



#endif winnt_supp_h_included
