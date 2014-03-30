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
File        : BASIC_Hello1.c
Purpose     : Simple demo drawing "Hello world"
----------------------------------------------------------------------
*/
#ifndef LCD_USE_WINSIM
#include "bsp.h"
#include "main.h"
#include "timeMeasure.h"
#endif
#include "GUI.h"
#include "BUTTON.h"
#include <stdio.h>

// user defines
#define BUTTON_WIDTH	100
#define BUTTON_HEIGHT	40
#define STBLUE ((93 << 0) + (168 << 8) + (223 << 16))

static BUTTON_Handle hButton;

static int time;
static char timeString[] = "                    ";
extern volatile unsigned int tamperPushed;

#ifndef LCD_USE_WINSIM
	void MainTask_WM_button(void) {
#else
	void MainTask(void) {
#endif

#ifdef LCD_USE_WINSIM
	GUI_Init();	// prepare GUI environment
#endif 
	
	SetemWinRunning(1);	
	GUI_SelectLayer(0); // select foregroung layer
	GUI_SetBkColor(STBLUE);	// select background as transparent color
	GUI_Clear();	// fill with the background color
		
	GUI_SelectLayer(1); // select foregroung layer
	GUI_SetBkColor(GUI_TRANSPARENT);	// select background as transparent color
	GUI_Clear();	// fill with the background color

	// Create button widget
	hButton = BUTTON_Create( 240/2 - BUTTON_WIDTH / 2, 320/2 - BUTTON_HEIGHT / 2,
		BUTTON_WIDTH, BUTTON_HEIGHT, GUI_ID_OK, WM_CF_SHOW);
	
  /* Set the button text */
  BUTTON_SetText(hButton, "Load File");

#ifndef LCD_USE_WINSIM
	// start time measurement
	timeMeasureStart();
#endif

	// measure effective drawing of the button
	GUI_Exec();

#ifndef LCD_USE_WINSIM
	// end measurement
	time = timeMeasureEnd();
	GUI_SetFont(&GUI_Font8_1);
	sprintf (timeString, "time to render: %d,%dms", time/10, time%10);
	GUI_DispStringAt(timeString, 10, 320 - 10);
#endif
	
	// optionally we can use cursor
  GUI_CURSOR_Show(); 
	
	// the GUI is now rendered 
	// in never ending loop just check if an incon is touched
#ifndef LCD_USE_WINSIM
  while(!tamperPushed)
#else
	while(1)
#endif
  {
		GUI_Exec();
  }
	WM_DeleteWindow(hButton);
	SetemWinRunning(0);
  GUI_CURSOR_Hide(); 
}



	
/*************************** End of file ****************************/

