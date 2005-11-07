/*
This is a part of the LiteStep Shell Source code.

Copyright (C) 1997-2002 The LiteStep Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#ifndef __WHARFDATA_H
#define __WHARFDATA_H

typedef struct
{
	HWND Handle;
	BOOL Visible;
	RECT Position;
	int Desk;
}
windowType;

typedef struct
{
	int borderSize;
	int pos;
	int winListSize;
	windowType *winList;
	int trayIconSize;
	int lm78Unit;
	int lm78MaxCpu;
	int lm78MaxMb;
	int lm78Cpu;
	int taskBar;
	int msTaskBar;
	int taskBarFore;
	int taskBarBack;
	int taskBarText;
	int vwmVelocity;
	int autoHideWharf;
	int autoHideTaskbar;
	int VWMDistance;
	int autoHideDelay;
	int showBeta;
	int usClock;
	int VWMNoGathering;
	int VWMNoAuto;
	int xmouseDelay;
	int xmouseBringToTop;
	char *pixmapDir;
	char *defaultBmp;
	int vwmBackColor;
	int vwmSelBackColor;
	int vwmForeColor;
	int vwmBorderColor;
	char *backsaver;
	int taskBarFore2;
	int stripBar;
	char *lsPath;
}
wharfDataType;

#define magicDWord 0x49474541

#endif // __WHARFDATA_H
