////////////////////////////////////////////////////////////////////////////
//  File:       editcmd.h
//  Version:    1.2.0.5
//  Created:    29-Dec-1998
//
//  Author:     Stcherbatchenko Andrei
//  E-mail:     windfall@gmx.de
//
//  Registry keys and values for Crystal Edit - syntax colorig text editor.
//
//  You are free to use or modify this code to the following restrictions:
//  - Acknowledge me somewhere in your about box, simple "Parts of code by.."
//  will be enough. If you can't (or don't want to), contact me personally.
//  - LEAVE THIS HEADER INTACT
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//  16-Aug-99
//      Ferdinand Prantl:
//  +   FEATURE: see cpps ...
//
//  ... it's being edited very rapidly so sorry for non-commented
//        and maybe "ugly" code ...
////////////////////////////////////////////////////////////////////////////

#ifndef __EDITREG_H__INCLUDED__
#define __EDITREG_H__INCLUDED__

//  Registry keys & values
#define REG_EDITPAD         _T ("Software\\EditPad")
#define REG_EDITPAD_MULTI   _T ("Software\\EditPad\\Multi")
#define REG_EDITPAD_SINGLE  _T ("Software\\EditPad\\Single")
#define REG_EDITPAD_IDE     _T ("Software\\EditPad\\IDE")
#define REG_EDITPAD_VERSION 0x0108000d
#define MAX_REMEMBERED 100
#define REMEMBERED_TO_REMOVE 10

#endif // __EDITREG_H__INCLUDED__
