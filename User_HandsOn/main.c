/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    22-July-2013
  * @brief   This file provides main program functions
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

// !! TO BE FIELD !!
#define EXAMPLE 0xFF


/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "bsp.h"
#include "GUI.h"
#include "timeMeasure.h"
#include "main_layerScroll.h"
#include "main_contentCreation.h"
#include "main_colorFormat.h"
#include "main_colorKeying.h"
#include "main_2layers.h"


#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define STBLUE ((93 << 0) + (168 << 8) + (223 << 16))
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
Callback_t *VerticalBlank_callback = NULL;

/* variable used to distinguish between emWin demos and LTDC plain demos */
uint8_t GUI_Initialized = 0;


__IO uint32_t tamperPushed = 0;
/* Private function prototypes -----------------------------------------------*/
typedef void (*pFunc)(void);
typedef struct sMainDescriptor
{
	pFunc	pFunction;
	char *pStringDescription;	
} tsMainDescriptor;

// table of functions which draw incons
const tsMainDescriptor mainTaskFunctions[] = 
{
	
#if (EXAMPLE == 10) || (defined EXAMPLE_ALL)
	{MainTask_ColorFormat, "Color Formats comparison: RGB888, RGB565, CLUT"},
#endif
#if (EXAMPLE == 1) || (defined EXAMPLE_ALL)
	{MainTask_LayerScroll, "Layer Scrolling"},
#endif
#if (EXAMPLE == 11) || (defined EXAMPLE_ALL)
	{MainTask_ColorKeying, "Color Keying"},
#endif
#if (EXAMPLE == 2) || (defined EXAMPLE_ALL)
	{MainTask_2Layers, "Two Layers"},
#endif
#if (EXAMPLE == 3) || (defined EXAMPLE_ALL)
	{MainTask_ContentCreation, "Content Creation"},
#endif
#if (EXAMPLE == 4) || (defined EXAMPLE_ALL)
	{MainTask_2D, " 2D library "},
#endif
#if (EXAMPLE == 5) || (defined EXAMPLE_ALL)
	{MainTask_2D_text, " 2D library + text "},
#endif
#if (EXAMPLE == 6) || (defined EXAMPLE_ALL)
	{MainTask_2D_AAtext, " 2D library + AAtext "},
#endif
#if (EXAMPLE == 7) || (defined EXAMPLE_ALL)
	{MainTask_2D_text_bmp, " 2D library + text 	+ bitmap"},
#endif
#if (EXAMPLE == 8) || (defined EXAMPLE_ALL)
	{MainTask_WM_button_skin, " WM + button "},
#endif
#if (EXAMPLE == 9) || (defined EXAMPLE_ALL)
	{MainTask_Graph, " Graph "},
#endif        
#if (EXAMPLE == 19) || (defined EXAMPLE_ALL)
	{MainTask_Gyro, " Gyro "},
#endif
#if (EXAMPLE == 0xFE) || (defined EXAMPLE_ALL)
	{MainTask_WM_button_list, " WM + button + list "},
#endif
#if (EXAMPLE == 0xFF) || (defined EXAMPLE_ALL)
	{MainTask_WM_button_list_icons, " WM + button + list\n + icons "}
#endif
};

/* Private functions ---------------------------------------------------------*/

void SetVerticalBlankCallback(Callback_t *callback_pointer)
{
	VerticalBlank_callback = callback_pointer;
}

void ResetVerticalBlankCallback(void)
{
	VerticalBlank_callback = NULL;
}


/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */ 
	
// text rectange definition for title
static GUI_RECT rect = {0, 200, 240 - 1, 200+32};
static GUI_RECT rectNextDemo = {0, 10, 240 - 1, 10+32};
static GUI_RECT rectNameDemo = {0, 10+32, 240 - 1, 199};
static char pDemoName[50] = "";	

uint8_t demoNo = 0;
	
extern GUI_CONST_STORAGE GUI_FONT GUI_FontTimesNewRoman31;

int main(void)
{

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	/* Initilize the BSP layer */
        LowLevel_Init();

	while(1)
	{

		GUI_Init();

		GUI_SelectLayer(1); // select foregroung layer
		GUI_SetBkColor(GUI_BLACK);//GUI_BLACK	// select background color as a solid color
		GUI_Clear();	// fill with the background color
		GUI_SetFont(&GUI_FontTimesNewRoman31);
		GUI_SetColor(STBLUE);
		GUI_DispStringInRect("Next demo: ", &rectNextDemo, GUI_TA_CENTER);
#ifdef EXAMPLE_ALL
		sprintf(pDemoName, "%d: %s", demoNo + 1, mainTaskFunctions[demoNo].pStringDescription);
#else
		sprintf(pDemoName, "%d: %s", EXAMPLE, mainTaskFunctions[0].pStringDescription);
#endif
		GUI_DispStringInRectWrap(pDemoName, &rectNameDemo, GUI_TA_CENTER | GUI_TA_VCENTER, GUI_WRAPMODE_WORD);
		GUI_DispNextLine();
		GUI_SetFont(&GUI_Font16_1);
		GUI_DispStringInRect("Push a button to continue", &rect, GUI_TA_CENTER);
		while(!tamperPushed);	// wait for another push
		tamperPushed = 0;
		GUI_SetColor(GUI_DEFAULT_COLOR);
		// Start Demo 
#ifdef EXAMPLE_ALL
		mainTaskFunctions[demoNo].pFunction();
		demoNo = (demoNo + 1) % GUI_COUNTOF(mainTaskFunctions);
#else
		mainTaskFunctions[0].pFunction();
#endif
		tamperPushed = 0;

	}
}

/**
* @brief  BSP_Background.
* @param  None
* @retval None
*/ 
void BSP_Background (void)
{
  static uint32_t LED_ticks = 0;
  static uint32_t POINTER_ticks = 0;
  
	timeMeasureMilisecondTick();
	
  if ( LED_ticks++ >= 100 )
  {
    LED_ticks = 0;
    
    /* toggle LED3..4 each 100ms */

    STM_EVAL_LEDToggle(LED4);
  }

  if ( POINTER_ticks++ >= 50 )
  {
    POINTER_ticks = 0;
		
    /* Executed only for emWin demos, not for LTDC plain demos */
    if(GUI_Initialized == 1)
    {
			 /* Capture input event and updade cursor */
       BSP_Pointer_Update();
    }
  }
} 

/* to inform other tasks if emWin is running or not */
uint32_t emWinRunning(void)
{
		return GUI_Initialized; 
}

/* to inform other tasks if emWin is running or not */
void SetemWinRunning(uint32_t flag)
{
	GUI_Initialized = flag; 
}


/* LTDC interrupt handler for simple tasks without emWin */
/* main purpose is to synchronize VBlank periods with content update */
void LTDC_ISR_simpleHandler(void)
{
  LTDC->ICR = (U32)LTDC_IER_LIE;	
	
	if (VerticalBlank_callback != NULL)
	{
		VerticalBlank_callback();
	}
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
