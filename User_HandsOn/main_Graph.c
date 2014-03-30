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
File        : main_2D.c
----------------------------------------------------------------------
*/
#ifndef LCD_USE_WINSIM
#include "bsp.h"
#include "main.h"
#include "timeMeasure.h"
#endif
#include "GUI.h"
#include <stdio.h>

#include "stm32f4xx_adc.h"

// user defines
#define ICON_SIZE 48
#define ICONS_HOR_POS	20
#define ICONS_VER_POS	20
#define STBLUE ((93 << 0) + (168 << 8) + (223 << 16))

  #define MESSAGE1   "ADC conversion w/DMA"
  #define MESSAGE1_1 "continuously transfer" 
  #define MESSAGE2   " ADC Ch13 Conv   "
  #define MESSAGE2_1 "    2.4Msps      "
  #define MESSAGE5   " ADC3 = %d,%1d V "
  #define LINENUM            0x15
  #define FONTSIZE         Font12x12

#define NUMCH         2

__IO uint16_t uhADC3ConvertedValue[NUMCH] = {0};
__IO uint32_t uwADC3ConvertedVoltage[NUMCH] = {0};

__IO uint16_t uhADC3ConvertedValue1 = 0;
__IO uint32_t uwADC3ConvertedVoltage1 = 0;

__IO uint8_t open;
__IO uint8_t openTimer;


extern volatile unsigned int tamperPushed;
static GUI_PID_STATE TouchState;
static int iconTouched;

//PF6 18 //P2-3  //ADC3_IN4 		 ADC3
//PC3 29 //P2-15 //ADC123_IN13/ ???? ADC3 ++++

//PA5 41 //P2-21 //ADC12_IN5/        ADC1

static void ADC3_CH13_DMA_Config(void)
{
  ADC_InitTypeDef       ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  DMA_InitTypeDef       DMA_InitStructure;
  GPIO_InitTypeDef      GPIO_InitStructure;

  /* Enable ADC3, DMA2 and GPIO clocks ****************************************/
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOF, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

  /* DMA2 Stream4 channel0 configuration **************************************/
  DMA_InitStructure.DMA_Channel = DMA_Channel_0;
  DMA_InitStructure.DMA_PeripheralBaseAddr = &ADC1->DR;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&uhADC3ConvertedValue1;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = 1;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA2_Stream4, &DMA_InitStructure);
  DMA_Cmd(DMA2_Stream4, ENABLE);

  /* DMA2 Stream0 channel2 configuration **************************************/
  DMA_InitStructure.DMA_Channel = DMA_Channel_2;
  DMA_InitStructure.DMA_PeripheralBaseAddr = &ADC3->DR;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&uhADC3ConvertedValue[0];
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = NUMCH;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA2_Stream0, &DMA_InitStructure);
  DMA_Cmd(DMA2_Stream0, ENABLE);

  /* Configure ADC3 Channel13 pin as analog input ******************************/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOF, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* ADC Common Init **********************************************************/
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);

  /* ADC3 Init ****************************************************************/
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;	
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 2;
  ADC_Init(ADC3, &ADC_InitStructure);

  /* ADC1 Init ****************************************************************/
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC3 regular channel13 configuration *************************************/
  ADC_RegularChannelConfig(ADC3, ADC_Channel_13, 1, ADC_SampleTime_84Cycles);
  ADC_RegularChannelConfig(ADC3, ADC_Channel_4, 2, ADC_SampleTime_84Cycles);

  ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_84Cycles);

 /* Enable DMA request after last transfer (Single-ADC mode) */
  ADC_DMARequestAfterLastTransferCmd(ADC3, ENABLE);
  ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

  /* Enable ADC3 DMA */
  ADC_DMACmd(ADC3, ENABLE);
  ADC_DMACmd(ADC1, ENABLE);

  /* Enable ADC3 */
  ADC_Cmd(ADC3, ENABLE);
  ADC_Cmd(ADC1, ENABLE);
}

void SysTICK(void)
{
SysTick_Config (SystemCoreClock/1000 );
openTimer=0;
}





static GUI_RECT rect = {0, 200, 240 - 1, 200+32};

void MainTask_Graph(void) {
      SysTICK();
	
	SetemWinRunning(1);
		
	GUI_SelectLayer(1); // select foregroung layer
	GUI_SetBkColor(STBLUE);	// select background color as a solid color
	GUI_Clear();	// fill with the background color


        GRAPH_DATA_Handle hData1, hData2, hData3;
        GRAPH_SCALE_Handle hScale;
        WM_HWIN hGraph;
        hGraph = GRAPH_CreateEx(0, 0, 240, 320, WM_HBKWIN, WM_CF_SHOW, 0, GUI_ID_GRAPH0);
        hData1 = GRAPH_DATA_YT_Create(GUI_GREEN, 240, 0, 0);
        hData2 = GRAPH_DATA_YT_Create(GUI_RED, 240, 0, 0);
        hData3 = GRAPH_DATA_YT_Create(GUI_BLUE, 240, 0, 0);

        GRAPH_AttachData(hGraph, hData1);
        GRAPH_AttachData(hGraph, hData2);
        GRAPH_AttachData(hGraph, hData3);

        hScale = GRAPH_SCALE_Create(19, GUI_TA_RIGHT, GRAPH_SCALE_CF_VERTICAL, 20);

        GRAPH_SCALE_SetFactor(hScale, 0.01);
        GRAPH_SCALE_SetNumDecs(hScale, 1);
        GRAPH_SCALE_SetFont(hScale, &GUI_Font16_1);
        GRAPH_SCALE_SetTextColor(hScale, GUI_BLACK);


        GRAPH_AttachScale(hGraph, hScale);
        GRAPH_SetBorder(hGraph, 20, 0, 0, 0);
        GRAPH_SetGridVis(hGraph, 1);


        GRAPH_SetColor(hGraph, GUI_WHITE, GRAPH_CI_BK);
        GRAPH_SetColor(hGraph, GUI_LIGHTGRAY, GRAPH_CI_BORDER);
        GRAPH_SetColor(hGraph, GUI_BLACK, GRAPH_CI_FRAME);
        GRAPH_SetColor(hGraph, GUI_DARKGRAY, GRAPH_CI_GRID);
        
        GRAPH_SetLineStyleH(hGraph, GUI_LS_DOT);
        GRAPH_SetLineStyleV(hGraph, GUI_LS_DOT);
        
        GRAPH_SetGridDistX(hGraph, 50);
        GRAPH_SetGridDistY(hGraph, 20);

        GUI_Exec();

        STM_EVAL_LEDToggle(LED4);
        STM_EVAL_LEDToggle(LED3);

	// the GUI is now rendered 
	// in never ending loop just check if an incon is touched
  
   ADC3_CH13_DMA_Config();
  
  /* Start ADC3 Software Conversion */ 
  ADC_SoftwareStartConv(ADC3);
  ADC_SoftwareStartConv(ADC1);


//while(1){}

  while(!tamperPushed)
  {             
    if(open){
              
              uwADC3ConvertedVoltage[0] = uhADC3ConvertedValue[0] *3000/0xFFF;
              uwADC3ConvertedVoltage[1] = uhADC3ConvertedValue[1] *3000/0xFFF;
              uwADC3ConvertedVoltage1 = uhADC3ConvertedValue1 *3000/0xFFF;
      
              GRAPH_DATA_YT_AddValue(hData1, uwADC3ConvertedVoltage[0]/10);
              GRAPH_DATA_YT_AddValue(hData2, uwADC3ConvertedVoltage[1]/10);
              GRAPH_DATA_YT_AddValue(hData3, uwADC3ConvertedVoltage1/10);
              GUI_Exec();
      
              
                open=0;
    }

  }     
       // WM_DeleteWindow(hGraph);
	SetemWinRunning(0);
	GUI_CURSOR_Hide(); 
}



/*************************** End of file ****************************/

