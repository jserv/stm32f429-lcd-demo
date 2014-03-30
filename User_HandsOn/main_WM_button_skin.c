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
File        : main_WM_button_skin.c
----------------------------------------------------------------------
*/
#ifndef LCD_USE_WINSIM
#include "bsp.h"
#include "timeMeasure.h"
#endif
#include "GUI.h"
#include "BUTTON.h"
#include "main.h"
#include <stdio.h>

// user defines
#define BUTTON_WIDTH	100
#define BUTTON_HEIGHT	40
#define STBLUE ((93 << 0) + (168 << 8) + (223 << 16))
#define STPINK ((212 << 0) + (0 << 8) + (122 << 16))

static GUI_PID_STATE TouchState;
static BUTTON_Handle hButton;

static int time;
static char timeString[] = "                    ";
extern volatile unsigned int tamperPushed;

static int DrawSkinST_BUTTON(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo);

void MainTask_WM_button_skin(void) {

	//GUI_Init();
	SetemWinRunning(1);

	sprintf (timeString, "GUI1");
	GUI_DispStringAt(timeString, 10, 10);


	GUI_SelectLayer(0); // select background  layer
	GUI_SetBkColor(STBLUE);	// select background as transparent color
	GUI_Clear();	// fill with the background color

	GUI_SelectLayer(1); // select foregroung layer
	GUI_SetBkColor(GUI_TRANSPARENT);	// select background as transparent color
	GUI_Clear();	// fill with the background color

	// Create button widget
	hButton = BUTTON_Create( 240/2 - BUTTON_WIDTH / 2, 320/2 - BUTTON_HEIGHT / 2,
		BUTTON_WIDTH, BUTTON_HEIGHT, GUI_ID_OK, WM_CF_SHOW);
	
	// !! TO BE MODIFIED !!
	// DOJSC Z TYM DO LADU
	 BUTTON_SetSkin(hButton, DrawSkinST_BUTTON);


  /* Set the button text */
	BUTTON_SetText(hButton, "ST Button");

	// start time measurement
	timeMeasureStart();
	STM_EVAL_LEDToggle(LED3);
	// measure effective drawing of the button
	GUI_Exec();

	// end measurement
	time = timeMeasureEnd();
	GUI_SetFont(&GUI_Font8_1);
	sprintf (timeString, "time to render: %d,%dms", time/10, time%10);
	GUI_DispStringAt(timeString, 10, 320 - 10);
	
	// optionally we can use cursor
  GUI_CURSOR_Show(); 

	// the GUI is now rendered 

  while(!tamperPushed)
  {
		GUI_Exec();
  }
	WM_DeleteWindow(hButton);
	SetemWinRunning(0);
  GUI_CURSOR_Hide(); 
}

static int DrawSkinST_BUTTON(const WIDGET_ITEM_DRAW_INFO * pDrawItemInfo) {
// define radius
#define RAD 5
	STM_EVAL_LEDToggle(LED4);
	char acBuffer[20];
	GUI_RECT Rect;
	switch (pDrawItemInfo->Cmd) {
		case WIDGET_ITEM_DRAW_TEXT:
			BUTTON_GetText(pDrawItemInfo->hWin, acBuffer, sizeof(acBuffer));
			GUI_SetColor(STPINK);
			Rect.x0 = pDrawItemInfo->x0; // Default position of text
			Rect.y0 = pDrawItemInfo->y0;
			Rect.x1 = pDrawItemInfo->x1;
			Rect.y1 = pDrawItemInfo->y1;
			GUI_SetTextMode(GUI_TM_TRANS);
			GUI_DispStringInRect(acBuffer, &Rect, GUI_TA_VCENTER | GUI_TA_HCENTER);
		break;
		case WIDGET_ITEM_DRAW_BACKGROUND:
			GUI_SetPenSize(3);
			// draw background
			Rect.x0 = pDrawItemInfo->x0; 
			Rect.y0 = pDrawItemInfo->y0;
			Rect.x1 = pDrawItemInfo->x1;
			Rect.y1 = pDrawItemInfo->y1;
			if (pDrawItemInfo->ItemIndex == BUTTON_SKINFLEX_PI_PRESSED)
				GUI_SetColor(GUI_GRAY);
			else
				GUI_SetColor(GUI_WHITE);
			GUI_FillRoundedRect(Rect.x0, Rect.y0, Rect.x1, Rect.y1, RAD);
			// draw outline
			GUI_SetColor(STPINK);
			GUI_DrawArc(Rect.x0 + RAD, Rect.y0 + RAD, RAD, RAD, 90, 180);
			GUI_DrawArc(Rect.x1 - RAD, Rect.y0 + RAD, RAD, RAD, 0, 90);
			GUI_DrawArc(Rect.x1 - RAD, Rect.y1 - RAD, RAD, RAD, 270, 359);
			GUI_DrawArc(Rect.x0 + RAD, Rect.y1 - RAD, RAD, RAD, 180, 270);
			GUI_DrawLine(Rect.x0 + RAD, Rect.y0, Rect.x1 - RAD, Rect.y0);
			GUI_DrawLine(Rect.x1, Rect.y0 + RAD, Rect.x1, Rect.y1 - RAD);
			GUI_DrawLine(Rect.x0 + RAD, Rect.y1, Rect.x1 - RAD, Rect.y1);
			GUI_DrawLine(Rect.x0, Rect.y0 + RAD, Rect.x0, Rect.y1 - RAD);
			break;
		default:
			//
			// Use the default skinning routine for processing all other commands
			//
			return BUTTON_DrawSkinFlex(pDrawItemInfo);
	}
	return 0;
}





	
/*************************** End of file ****************************/

