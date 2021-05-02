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


#include "stm32f429i_discovery_l3gd20.h"


/* Private define ------------------------------------------------------------*/
#define L3G_Sensitivity_250dps     (float)   114.285f         /*!< gyroscope sensitivity with 250 dps full scale [LSB/dps]  */
#define L3G_Sensitivity_500dps     (float)    57.1429f        /*!< gyroscope sensitivity with 500 dps full scale [LSB/dps]  */
#define L3G_Sensitivity_2000dps    (float)    14.285f         /*!< gyroscope sensitivity with 2000 dps full scale [LSB/dps] */
/* Private macro -------------------------------------------------------------*/
#define ABS(x)  ((x) > 0 ? (x) : -(x))



// user defines
#define ICON_SIZE 48
#define ICONS_HOR_POS	20
#define ICONS_VER_POS	20
#define STBLUE ((93 << 0) + (168 << 8) + (223 << 16))


#define GYRO_BYTES         6
 uint8_t GyroOUT[GYRO_BYTES] = {0};
 uint16_t GyroVALUE[GYRO_BYTES/2] = {0};




extern volatile __IO uint8_t open;
extern volatile __IO uint8_t openTimer;


extern volatile unsigned int tamperPushed;



static void SysTICK(void)
{
SysTick_Config (SystemCoreClock/1000 );
openTimer=0;
}



void Demo_GyroConfig(void)
{
  L3GD20_InitTypeDef L3GD20_InitStructure;
  L3GD20_FilterConfigTypeDef L3GD20_FilterStructure;

  /* Configure Mems L3GD20 */
  L3GD20_InitStructure.Power_Mode = L3GD20_MODE_ACTIVE;
  L3GD20_InitStructure.Output_DataRate = L3GD20_OUTPUT_DATARATE_1;
  L3GD20_InitStructure.Axes_Enable = L3GD20_AXES_ENABLE;
  L3GD20_InitStructure.Band_Width = L3GD20_BANDWIDTH_1;
  L3GD20_InitStructure.BlockData_Update = L3GD20_BlockDataUpdate_Single;
  L3GD20_InitStructure.Endianness = L3GD20_BLE_LSB;
  L3GD20_InitStructure.Full_Scale = L3GD20_FULLSCALE_2000;
  L3GD20_Init(&L3GD20_InitStructure);

  L3GD20_FilterStructure.HighPassFilter_Mode_Selection =L3GD20_HPM_NORMAL_MODE_RES;
  L3GD20_FilterStructure.HighPassFilter_CutOff_Frequency = L3GD20_HPFCF_0;
  L3GD20_FilterConfig(&L3GD20_FilterStructure) ;

  L3GD20_FilterCmd(L3GD20_HIGHPASSFILTER_ENABLE);
}



void Demo_GyroReadAngRate (float* pfData)
{
  uint8_t tmpbuffer[6] ={0};
  int16_t RawData[3] = {0};
  uint8_t tmpreg = 0;
  float sensitivity = 0;
  int i =0;

  L3GD20_Read(&tmpreg,L3GD20_CTRL_REG4_ADDR,1);

  L3GD20_Read(tmpbuffer,L3GD20_OUT_X_L_ADDR,6);

  /* check in the control register 4 the data alignment (Big Endian or Little Endian)*/
  if(!(tmpreg & 0x40))
  {
    for(i=0; i<3; i++)
    {
      RawData[i]=(int16_t)(((uint16_t)tmpbuffer[2*i+1] << 8) + tmpbuffer[2*i]);
    }
  }
  else
  {
    for(i=0; i<3; i++)
    {
      RawData[i]=(int16_t)(((uint16_t)tmpbuffer[2*i] << 8) + tmpbuffer[2*i+1]);
    }
  }

  /* Switch the sensitivity value set in the CRTL4 */
  switch(tmpreg & 0x30)
  {
  case 0x00:
    sensitivity=L3G_Sensitivity_250dps;
    break;

  case 0x10:
    sensitivity=L3G_Sensitivity_500dps;
    break;

  case 0x20:
    sensitivity=L3G_Sensitivity_2000dps;
    break;
  default:
    sensitivity=L3G_Sensitivity_250dps;

  }
  /* divide by sensitivity */
  for(i=0; i<3; i++)
  {
    pfData[i]=(float)RawData[i]/sensitivity;
  }
}



static GUI_RECT rect = {0, 200, 240 - 1, 200+32};

void MainTask_Gyro(void) {
      SysTICK();
      float Buffer[6] = {0};
      uint8_t Xval, Yval = 0x00;


      Demo_GyroConfig();



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
        //hData3 = GRAPH_DATA_YT_Create(GUI_BLUE, 240, 0, 0);

        GRAPH_AttachData(hGraph, hData1);
        GRAPH_AttachData(hGraph, hData2);
        //GRAPH_AttachData(hGraph, hData3);

        hScale = GRAPH_SCALE_Create(19, GUI_TA_RIGHT, GRAPH_SCALE_CF_VERTICAL, 20);

        GRAPH_SCALE_SetFactor(hScale, 0.5);
        GRAPH_SCALE_SetNumDecs(hScale, 0.01);
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
  


  while(!tamperPushed)
  {             
    if(open){

        /* Read Gyro Angular data */
        Demo_GyroReadAngRate(Buffer);
        /* Update autoreload and capture compare registers value*/
        Xval = ABS((int8_t)(Buffer[0]));
        Yval = ABS((int8_t)(Buffer[1]));

      
              GRAPH_DATA_YT_AddValue(hData1, Xval*2);
              GRAPH_DATA_YT_AddValue(hData2, Yval*2);
              GUI_Exec();
      
              
                open=0;
    }

  }     
       // WM_DeleteWindow(hGraph);
	SetemWinRunning(0);
	GUI_CURSOR_Hide(); 
}



/*************************** End of file ****************************/

