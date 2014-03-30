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
#include "LISTVIEW.h"
#include "ICONVIEW.h"
#include "TEXT.h"
#include <stdio.h>
#include <string.h>

// user defines
#define BUTTON_WIDTH	100
#define BUTTON_HEIGHT	40
#define BUTTON_VER_POS	50
#define LISTVIEW_WIDTH 220
#define LISTVIEW_HEIGHT 300

#define ICONS_HOR_POS	10
#define ICONS_VER_POS	10
#define STBLUE ((93 << 0) + (168 << 8) + (223 << 16))

static int iconNumber, iconPadding;

static BUTTON_Handle hButton;
static LISTVIEW_Handle hListView;
static TEXT_Handle hTextSong;
static ICONVIEW_Handle hIconView;

static int time;
static char timeString[30] = "";
extern volatile unsigned int tamperPushed;

static unsigned char pSongString[20] = "Song:\n";

static const unsigned char * myFileTable[][1] = {
	"Miley Cyrus" ,
	"Katy Perry" ,
	"Robin Thicke" ,
	"Lady Gaga" 
};

extern GUI_CONST_STORAGE GUI_BITMAP bmStyleNexticon1;
extern GUI_CONST_STORAGE GUI_BITMAP bmStylePauseicon2;
extern GUI_CONST_STORAGE GUI_BITMAP bmStylePlayicon1;
extern GUI_CONST_STORAGE GUI_BITMAP bmStylePreviousicon1;
extern GUI_CONST_STORAGE GUI_BITMAP bmStyleStopicon1;

typedef struct {
  const GUI_BITMAP * pBitmap;
  const char       * pText;
} BITMAP_ITEM;

static const BITMAP_ITEM myBitmapItem[] = {
  {&bmStylePreviousicon1, "Previous"},
	{&bmStyleStopicon1, "Stop"},
  {&bmStylePauseicon2, "Pause"},
  {&bmStylePlayicon1, "Play"},
  {&bmStyleNexticon1,"Next"},
};

//extern GUI_CONST_STORAGE GUI_BITMAP bmstm32_yellow_320x240_bmp_M565;
//extern GUI_CONST_STORAGE GUI_BITMAP bmbluegirl565;
extern GUI_CONST_STORAGE GUI_BITMAP bmbackground;

static void myCbWin(WM_MESSAGE * pMsg);

#ifndef LCD_USE_WINSIM
	void MainTask_WM_button_list_icons(void) {
#else
	void MainTask(void) {
#endif

	int i;
#ifdef LCD_USE_WINSIM
	GUI_Init();	// prepare GUI environment
#endif 
	
#ifndef LCD_USE_WINSIM
	// start time measurement
	timeMeasureStart();
#endif
	
	SetemWinRunning(1);	
	GUI_SelectLayer(0); // select background layer
	GUI_DrawBitmap(&bmbackground, 0, 0);

  // GUI_SetBkColor(STBLUE);	// select background as transparent color
	// GUI_Clear();	// fill with the background color
		
	GUI_SelectLayer(1); // select foregroung layer
	GUI_SetBkColor(GUI_TRANSPARENT);	// select background as transparent color
	GUI_Clear();	// fill with the background color

	// Create BUTTON widget
	hButton = BUTTON_Create( 320/2 - BUTTON_WIDTH / 2, BUTTON_VER_POS,
	BUTTON_WIDTH, BUTTON_HEIGHT, GUI_ID_BUTTON0, WM_CF_SHOW);
	
  // Set the button text
  BUTTON_SetText(hButton, "Load File");

	// create TEXT widget
	hTextSong = TEXT_CreateEx(ICONS_HOR_POS + bmStyleNexticon1.XSize + 20, 200,	// position
														240 - 1 - ICONS_HOR_POS - bmStyleNexticon1.XSize - 20, 100,	//size
														WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_TEXT0, (const char *)pSongString);
	TEXT_SetFont(hTextSong, &GUI_Font24B_ASCII);
  TEXT_SetTextAlign(hTextSong, GUI_TA_HCENTER | GUI_TA_VCENTER);
  TEXT_SetTextColor(hTextSong, GUI_WHITE);
	TEXT_SetWrapMode(hTextSong, GUI_WRAPMODE_WORD);
	
	// Create iconview widget
	hIconView = ICONVIEW_CreateEx(ICONS_HOR_POS, ICONS_VER_POS,	// start position
													bmStyleNexticon1.XSize + 20, GUI_GetScreenSizeY() - 2*ICONS_VER_POS,	// size
                          WM_HBKWIN, WM_CF_SHOW, 
                          ICONVIEW_CF_AUTOSCROLLBAR_V, GUI_ID_ICONVIEW0, 
													bmStyleNexticon1.XSize, bmStyleNexticon1.YSize + GUI_Font8_1.YSize);
  // get the number of icons
	iconNumber = GUI_COUNTOF(myBitmapItem);
	// Add icons to the widget
	for (i = 0; i < iconNumber; i++) {
    ICONVIEW_AddBitmapItem(hIconView, myBitmapItem[i].pBitmap, myBitmapItem[i].pText);
  }

	// compute padding to fit to whole display
	iconPadding =  ((GUI_GetScreenSizeY() - 2*ICONS_HOR_POS - (iconNumber * (bmStyleNexticon1.YSize + GUI_Font8_1.YSize))) / iconNumber);
	ICONVIEW_SetSpace(hIconView, GUI_COORD_Y, iconPadding);
	
#ifndef LCD_USE_WINSIM
		ICONVIEW_SetBkColor(hIconView, ICONVIEW_CI_BK, GUI_TRANSPARENT);
#else
		ICONVIEW_SetBkColor(hIconView, ICONVIEW_CI_BK, STBLUE);
#endif
	ICONVIEW_SetBkColor(hIconView, ICONVIEW_CI_SEL, GUI_LIGHTRED);
	
	// create LISTVIEW widget
	hListView = LISTVIEW_Create( 240/2 - LISTVIEW_WIDTH / 2, 320/2 - LISTVIEW_HEIGHT / 2,
		LISTVIEW_WIDTH, LISTVIEW_HEIGHT, WM_HBKWIN, GUI_ID_LISTVIEW0, 0, 0);
	// fill in the data
	LISTVIEW_AddColumn(hListView, 100, "File Name", GUI_TA_CENTER);
	for (i = 0; i < GUI_COUNTOF(myFileTable); i++) {
		LISTVIEW_AddRow(hListView, (const GUI_ConstString *)myFileTable[i]);
	}
	
	//customize a bit the appearance
	LISTVIEW_SetBkColor(hListView, LISTVIEW_CI_UNSEL, (0x40 << 24) | GUI_WHITE);
	LISTVIEW_SetBkColor(hListView, LISTVIEW_CI_SEL, (0x40 << 24) | GUI_GRAY);
	LISTVIEW_SetBkColor(hListView, LISTVIEW_CI_SELFOCUS, (0x40 << 24) | GUI_BLUE);

	
	// assign callback function to the background window
	WM_SetCallback(WM_HBKWIN, myCbWin);



	// measure effective drawing of the button
	GUI_Exec();

#ifndef LCD_USE_WINSIM
	// end measurement
	time = timeMeasureEnd();
	GUI_SetFont(&GUI_Font8_1);
	sprintf (timeString, "time to render: %d,%dms", time/10, time%10);
	GUI_DispStringAt(timeString, 10, 240 - 10);
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
	
	WM_DeleteWindow(hTextSong);
	WM_DeleteWindow(hListView);
	WM_DeleteWindow(hButton);
	WM_DeleteWindow(hIconView);
	SetemWinRunning(0);
  GUI_CURSOR_Hide(); 
	GUI_Exec();
}

static void myCbWin(WM_MESSAGE * pMsg) {
  int NCode, Id;
  switch (pMsg->MsgId) {
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);      /* Id of widget */
    NCode = pMsg->Data.v;                 /* Notification code */
    switch (Id) {
    case GUI_ID_BUTTON0:
      switch (NCode) {
      case WM_NOTIFICATION_RELEASED:
        WM_ShowWindow(hListView);
        break;
      }
      break;
		case GUI_ID_LISTVIEW0:
      switch (NCode) {
      case WM_NOTIFICATION_RELEASED:
				{
					// import text from listbox selection
					LISTVIEW_GetItemText(hListView, 0, LISTVIEW_GetSel(hListView), (char *)&pSongString[6], 20-6);
					// strcat(pSongString, pTemp);
					// show it
					TEXT_SetText(hTextSong, (const char *)pSongString);
					// now we can hide the list 
					WM_HideWindow(hListView);		
				}
        break;
      }
      break;
    }
    break;
  case WM_PAINT:
		// re-Draw background
#ifndef LCD_USE_WINSIM
		GUI_SetBkColor(GUI_TRANSPARENT);
#else
		GUI_SetBkColor(STBLUE);
#endif
		GUI_Clear();
    break;
	case WM_TOUCH:
		WM_SetFocus(hButton);
		break;
  }
}

	
/*************************** End of file ****************************/

