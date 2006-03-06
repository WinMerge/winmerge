/** 
 * @file  PluginError.h
 *
 * @brief Declaration of global Plugin Error functions
 *
 * These display a string (either specified or from string resouce)
 * via a MessageBox.
 * @todo These need to be altered to store error in property,
 * when Plugin Errors are implemented.
 *
 */
// RCS ID line follows -- this is updated by CVS
// $Id$

#ifndef PluginError_h_included
#define PluginError_h_included

void PluginError(int id);
void PluginErrorFmt(int idfmt, ...);


#endif // PluginError_h_included
