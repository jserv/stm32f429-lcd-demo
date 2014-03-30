/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2012  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.18 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The software  has been  licensed to  STMicroelectronics  International
N.V. whose  registered office  is situated at Plan-les-Ouates, Geneva,
39 Chemin du Champ des Filles,  Switzerland solely for the purposes of
creating libraries for  STMicroelectronics  ARM Cortex™-M-based 32-bit
microcontroller    products,    sublicensed    and    distributed   by
STMicroelectronics  under  the  terms  and  conditions of the End User
License Agreement supplied with  the software. The use of the software
on a stand-alone basis  or for any purpose other  than to operate with
the specified  microcontroller is prohibited and subject to a separate
license agreement.

Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : main_2D_AAtext.c
----------------------------------------------------------------------
*/
#ifndef LCD_USE_WINSIM
#include "bsp.h"
#include "main.h"
#include "timeMeasure.h"
#endif
#include "GUI.h"
#include <stdio.h>

// user defines
#define ICON_SIZE 48
#define ICONS_HOR_POS	20
#define ICONS_VER_POS	20
#define STBLUE ((93 << 0) + (168 << 8) + (223 << 16))


static void DrawStop(int x, int y, int size);
static void DrawPlay(int x, int y, int size);
static void DrawNext(int x, int y, int size);
static void DrawPrevious(int x, int y, int size);
static void DrawPause(int x, int y, int size);
static void ReDrawIcon(GUI_COLOR color, int iconTouched);

static GUI_PID_STATE TouchState;
static int iconTouched;
static int horPos, verPos;
static int iconNumber, iconPadding;

static int time;
static char timeString[] = "                    ";
extern volatile unsigned int tamperPushed;

// text rectange definition for title
static GUI_RECT rect = {ICON_SIZE + ICONS_HOR_POS, 10, 240 - 1, 320-1};

typedef void (*pFuncIconDraw)(int x, int y, int size);

// table of functions which draw incons
static const pFuncIconDraw iconDrawFunctions[] = 
{
	DrawPrevious,
	DrawStop,
	DrawPause,
	DrawPlay,
	DrawNext
};

// !! TO BE MODIFIED !!
 extern GUI_CONST_STORAGE GUI_FONT GUI_FontVerdana32;

void MainTask_2D_AAtext(void) {


	int i;

	horPos = ICONS_HOR_POS;
	verPos = ICONS_VER_POS;
	iconTouched = 0xFF;

	SetemWinRunning(1);	
	GUI_SelectLayer(1); // select foregroung layer
	GUI_SetBkColor(STBLUE);	// select background color as a solid color
	GUI_Clear();	// fill with the background color

	// get the number of icons
	iconNumber = GUI_COUNTOF(iconDrawFunctions);
	// compute padding to fit to whole display
	iconPadding =  ((GUI_GetScreenSizeY() - (iconNumber * ICON_SIZE) - 2*ICONS_VER_POS) / iconNumber);

	// start time measurement
	timeMeasureStart();

	// now draw our icons one by one
	for (i = 0; i < iconNumber; i++)
	{
		iconDrawFunctions[i](horPos, verPos, ICON_SIZE);
		verPos += ICON_SIZE + iconPadding;
	}
	
	// add a title text
	// !! TO BE MODIFIED !!
	 GUI_SetFont(&GUI_FontVerdana32);
	 GUI_DispStringInRectWrap("tompul", &rect, GUI_TA_CENTER, GUI_WRAPMODE_WORD);

	// end measurement
	time = timeMeasureEnd();
	GUI_SetFont(&GUI_Font8_1);
	sprintf (timeString, "time to render: %d,%dms", time/10, time%10);
	GUI_DispStringAt(timeString, 10, 320 - 10);
	
	// optionally we can use cursor
  GUI_CURSOR_Show(); 

	// the GUI is now rendered 
	// in never ending loop just check if an incon is touched

  while(!tamperPushed)
  {
		GUI_TOUCH_GetState(&TouchState);  // Get the touch position in pixel
		if (TouchState.Pressed)
		{
			if (iconTouched == 0xFF)	// no icon was touched previously
			{
				GUI_CURSOR_SetPosition(TouchState.x, TouchState.y);	// move the cursor to current touch position
				// check if the touch is in icons area
				if (TouchState.x > ICONS_HOR_POS && TouchState.x < ICONS_HOR_POS + ICON_SIZE)
				{
					if(TouchState.y > ICONS_VER_POS && TouchState.y < ICONS_VER_POS + iconNumber*(ICON_SIZE + iconPadding))
					{
						// get he number of touched icon
						iconTouched = (TouchState.y - ICONS_VER_POS) / (ICON_SIZE + iconPadding);
						ReDrawIcon(GUI_RED, iconTouched); // draw again with hihghlight color
					}
				}
			}
		}
		else
		{
			ReDrawIcon(GUI_WHITE, iconTouched);
			iconTouched = 0xFF;
		}  
  }
	SetemWinRunning(0);
  GUI_CURSOR_Hide(); 
}


// functions to draw icons

static void DrawStop(int x, int y, int size)
{
	GUI_FillRect(x, y, x + size,  y + size);
}

static void DrawPlay(int x, int y, int size)
{
	GUI_POINT pPoint[3]; 
	pPoint[0].x = 0;
	pPoint[0].y = 0;
	pPoint[1].x = size;
	pPoint[1].y = size / 2;
	pPoint[2].x = 0;
	pPoint[2].y = size;
	
	GUI_FillPolygon(pPoint, 3, x, y);
}

static void DrawNext(int x, int y, int size)
{
	GUI_POINT pPoint[3]; 
	pPoint[0].x = 0;
	pPoint[0].y = 0;
	pPoint[1].x = (size/2);
	pPoint[1].y = size / 2;
	pPoint[2].x = 0;
	pPoint[2].y = size;
	
	GUI_FillPolygon(pPoint, 3, x, y);
	GUI_FillPolygon(pPoint, 3, x + size/2, y);
}

static void DrawPrevious(int x, int y, int size)
{
	GUI_POINT pPoint[3]; 
	pPoint[0].x = (size/2);
	pPoint[0].y = 0;
	pPoint[1].x = (size/2);
	pPoint[1].y = size;
	pPoint[2].x = 0;
	pPoint[2].y = size / 2;
	
	GUI_FillPolygon(pPoint, 3, x, y);
	GUI_FillPolygon(pPoint, 3, x + size/2, y);
}

static void DrawPause(int x, int y, int size)
{
	GUI_FillRect(x, y, x + size/2 - size/10,  y + size);
	GUI_FillRect(x + size/2 + size/10, y, x + size,  y + size);	
}

// redraws icon with specified color 
static void ReDrawIcon(GUI_COLOR color, int iconTouched)
{
	GUI_SetColor(color);
	if (iconTouched < iconNumber)	// valid icon number?
	{
		iconDrawFunctions[iconTouched](horPos,ICONS_VER_POS + iconTouched * (ICON_SIZE + iconPadding), ICON_SIZE);
	}
}


/*************************** End of file ****************************/

