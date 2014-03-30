/**
  ******************************************************************************
  * @file    main_contentCreation.c
  * @author  TOMAS team
  * @version V1.0.0
  * @date    3-October-2013
  * @brief   Demonstration of different color formats
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

#include "bsp.h"
#include "timeMeasure.h"
#include "main.h"
#include "main_contentCreation.h"


#include "ImageIconCamera.h"
#include "ImageIconHouse.h"
#include "ImageIconLight.h"
#include "ImageButtonMain.h"
#include "ImageBackground.h"
#include "ImageSnow1.h"
#include "ImageSnow2.h"
#include "ImageSnow3.h"
#include "ImageArmed.h"


#define ANIMATION_PERIOD 64
#define STBLUE ((93 << 0) + (168 << 8) + (223 << 16))

extern volatile unsigned int tamperPushed;

static LTDC_Layer_InitTypeDef LTDC_Layer_InitStruct; 

static int animationPeriodElapsed;
static int iconCounter = 0;

static void LCD_RGB888LayerInit(void);
static void DMA2D_Config(ts_DMA2D_descriptor* desc, uint8_t opacity, uint32_t imageBackground);


ts_DMA2D_descriptor sDescriptors[] = {
	{240, 320, 0, 0, (uint32_t*)ImageBackground, 3, CM_RGB888},
	{ImageButtonMain_width, ImageButtonMain_height, 13, 5, (uint32_t*)ImageButtonMain, 4, CM_ARGB8888},
	{ImageIconHouse_width, ImageIconHouse_height, 43, 47, (uint32_t*)ImageIconHouse, 4, CM_ARGB8888},
	{ImageIconLight_width, ImageIconLight_height, 137, 47, (uint32_t*)ImageIconLight, 4, CM_ARGB8888},
	{ImageIconCamera_width, ImageIconCamera_height, 91, 143, (uint32_t*)ImageIconCamera, 4, CM_ARGB8888},
	// !! TO BE MODIFIED !!
  	{ImageArmed_width, ImageArmed_height, 13, 252, (uint32_t*)ImageArmed, 4, CM_ARGB8888},
	
};

ts_DMA2D_descriptor sSnowDescriptors[] = {
	{ImageSnow1_width, ImageSnow1_height, 146, 227, (uint32_t*)ImageSnow1, 4, CM_ARGB8888},
	{ImageSnow2_width, ImageSnow2_height, 146, 227, (uint32_t*)ImageSnow2, 4, CM_ARGB8888},
	{ImageSnow3_width, ImageSnow3_height, 146, 227, (uint32_t*)ImageSnow3, 4, CM_ARGB8888},
};

static void myVeritcalCallback(void);

void MainTask_ContentCreation(void) 
{
	animationPeriodElapsed = 1;
	iconCounter = 0;
	
	LCD_RGB888LayerInit();
	SetVerticalBlankCallback(myVeritcalCallback);
	
	while(!tamperPushed)
	{
	}
	
	ResetVerticalBlankCallback();
}

void myVeritcalCallback(void)
{
	static uint32_t i = 0;
	static uint32_t j = 0;

	static int32_t blankCounter = 0;	
	static int32_t step = 1;
	
	// this flag is set every ~2sec to make icon appearing in intervals
	if (animationPeriodElapsed)
	{
          animationPeriodElapsed = 0;
          while (iconCounter < (sizeof(sDescriptors)/sizeof(ts_DMA2D_descriptor)))
        {
		
		// configure the DMA2D transfer of the next graphic element from descriptor list
                
		DMA2D_Config(&sDescriptors[iconCounter], 0xFF, SDRAM_BANK_ADDR);
		/* Start Transfer */
		DMA2D_StartTransfer(); 

		/* Wait for CTC Flag activation */
		while(DMA2D_GetFlagStatus(DMA2D_FLAG_TC) == RESET)
		{
		}

		DMA2D_ClearFlag(DMA2D_FLAG_TC);
		
		// increment the icon counter and wrap around totoal number of graphic elements
		iconCounter = (iconCounter + 1);// % (sizeof(sDescriptors)/sizeof(ts_DMA2D_descriptor));
	}
	}	
	blankCounter += step;
	if (blankCounter >= ANIMATION_PERIOD)
	{
		step = -1;
		j = (j + 1) % (sizeof(sSnowDescriptors)/sizeof(ts_DMA2D_descriptor));

		animationPeriodElapsed = 1;
	}
	else if (blankCounter == 0)
	{
		i = (i + 1) % (sizeof(sSnowDescriptors)/sizeof(ts_DMA2D_descriptor));
		step = 1;
	}
	
	DMA2D_Config(&sSnowDescriptors[i], blankCounter * 255/ANIMATION_PERIOD, (uint32_t)ImageBackground);
	 /* Start Transfer */
	DMA2D_StartTransfer(); 
	STM_EVAL_LEDToggle(LED3);
	/* Wait for CTC Flag activation */
	while(DMA2D_GetFlagStatus(DMA2D_FLAG_TC) == RESET)
	{
	}
	STM_EVAL_LEDToggle(LED3);
	DMA2D_ClearFlag(DMA2D_FLAG_TC);
	
	DMA2D_Config(&sSnowDescriptors[j], 255 - (blankCounter * 255/ANIMATION_PERIOD), SDRAM_BANK_ADDR);
	 /* Start Transfer */
	DMA2D_StartTransfer(); 
	STM_EVAL_LEDToggle(LED3);
	/* Wait for CTC Flag activation */
	while(DMA2D_GetFlagStatus(DMA2D_FLAG_TC) == RESET)
	{
	}
	STM_EVAL_LEDToggle(LED3);
	DMA2D_ClearFlag(DMA2D_FLAG_TC);	
}


static void LCD_RGB888LayerInit(void)
{
  LTDC_LayerCmd(LTDC_Layer2, DISABLE);
	
	/* LTDC configuration reload */  
  LTDC_ReloadConfig(LTDC_IMReload);
	
  /* Windowing configuration */
  /* In this case all the active display area is used to display a picture then :
  Horizontal start = horizontal synchronization + Horizontal back porch = 30 
  Horizontal stop = Horizontal start + window width -1 = 30 + 240 -1
  Vertical start   = vertical synchronization + vertical back porch     = 4
  Vertical stop   = Vertical start + window height -1  = 4 + 320 -1      */      
  LTDC_Layer_InitStruct.LTDC_HorizontalStart = 30;
  LTDC_Layer_InitStruct.LTDC_HorizontalStop = (LCD_PIXEL_WIDTH + 30 - 1); 
  LTDC_Layer_InitStruct.LTDC_VerticalStart = 4;
  LTDC_Layer_InitStruct.LTDC_VerticalStop = (LCD_PIXEL_HEIGHT + 4 - 1);
  
  /* Pixel Format configuration*/
  LTDC_Layer_InitStruct.LTDC_PixelFormat = LTDC_Pixelformat_RGB888;
  /* Alpha constant (255 totally opaque) */
  LTDC_Layer_InitStruct.LTDC_ConstantAlpha = 255; 
  /* Default Color configuration (configure A,R,G,B component values) */          
  LTDC_Layer_InitStruct.LTDC_DefaultColorBlue = 0;        
  LTDC_Layer_InitStruct.LTDC_DefaultColorGreen = 0;       
  LTDC_Layer_InitStruct.LTDC_DefaultColorRed = 0;         
  LTDC_Layer_InitStruct.LTDC_DefaultColorAlpha = 0;
  /* Configure blending factors */       
  LTDC_Layer_InitStruct.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_CA;    
  LTDC_Layer_InitStruct.LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_CA;
  
  /* the length of one line of pixels in bytes + 3 then :
  Line Lenth = Active width x number of bytes per pixel + 3 
  Active width         = LCD_PIXEL_WIDTH 
  number of bytes per pixel = 3    (pixel_format : RGB888) 
  */
  LTDC_Layer_InitStruct.LTDC_CFBLineLength = ((LCD_PIXEL_WIDTH * 3) + 3);
  /* the pitch is the increment from the start of one line of pixels to the 
  start of the next line in bytes, then :
  Pitch = Active high width x number of bytes per pixel */ 
  LTDC_Layer_InitStruct.LTDC_CFBPitch = (LCD_PIXEL_WIDTH * 3);
  
  /* Configure the number of lines */  
  LTDC_Layer_InitStruct.LTDC_CFBLineNumber = LCD_PIXEL_HEIGHT;
  
  /* Start Address configuration : the LCD Frame buffer is defined in Flash memory*/    
	LTDC_Layer_InitStruct.LTDC_CFBStartAdress = (uint32_t)SDRAM_BANK_ADDR;
  
  /* Initialize LTDC layer 2 - foreground */
  LTDC_LayerInit(LTDC_Layer2, &LTDC_Layer_InitStruct);
    
  /* LTDC configuration reload */  
  LTDC_ReloadConfig(LTDC_IMReload);
  
  /* Enable foreground Layers */
  LTDC_LayerCmd(LTDC_Layer2, ENABLE);
  
  /* LTDC configuration reload */  
  LTDC_ReloadConfig(LTDC_IMReload);

}


static void DMA2D_Config(ts_DMA2D_descriptor* desc, uint8_t opacity, uint32_t imageBackground)
{
	DMA2D_InitTypeDef      DMA2D_InitStruct;
	DMA2D_FG_InitTypeDef   DMA2D_FG_InitStruct;
	DMA2D_BG_InitTypeDef   DMA2D_BG_InitStruct;

  /* Enable the DMA2D Clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2D, ENABLE);
  
  /* Transfer mode => Memory to memory with pixel format conversion*/
  DMA2D_InitStruct.DMA2D_Mode = DMA2D_M2M_BLEND; 
  /* Output color mode */
  DMA2D_InitStruct.DMA2D_CMode = DMA2D_RGB888; 
  /* Output Address at SRAM memory */
  DMA2D_InitStruct.DMA2D_OutputMemoryAdd = (uint32_t)SDRAM_BANK_ADDR + desc->pos_x*3 + desc->pos_y*3*LCD_PIXEL_WIDTH;
  /* Initialize the alpha and RGB values */
  DMA2D_InitStruct.DMA2D_OutputGreen = 0;
  DMA2D_InitStruct.DMA2D_OutputBlue = 0;
  DMA2D_InitStruct.DMA2D_OutputRed = 0;
  DMA2D_InitStruct.DMA2D_OutputAlpha = 0;
  /* Initialize the output offset */
  DMA2D_InitStruct.DMA2D_OutputOffset = LCD_PIXEL_WIDTH - desc->width;
  /* Number of lines : height */
  DMA2D_InitStruct.DMA2D_NumberOfLine = desc->height; 
  /* Number of pixel per line : width */
  DMA2D_InitStruct.DMA2D_PixelPerLine = desc->width;
  
  /* Initialize DMA2D */
  DMA2D_Init(&DMA2D_InitStruct);

  /* Configure default values for foreground */
  DMA2D_FG_StructInit(&DMA2D_FG_InitStruct);
  DMA2D_BG_StructInit(&DMA2D_BG_InitStruct);
  
  /* Input Address at flash memory */
  DMA2D_FG_InitStruct.DMA2D_FGMA = (uint32_t)desc->source;
  /* Input color mode */
  DMA2D_FG_InitStruct.DMA2D_FGCM = desc->colorMode;
  DMA2D_FG_InitStruct.DMA2D_FGPFC_ALPHA_MODE = COMBINE_ALPHA_VALUE;
  DMA2D_FG_InitStruct.DMA2D_FGPFC_ALPHA_VALUE = opacity;
  
  /* Initialize foreground */
  DMA2D_FGConfig(&DMA2D_FG_InitStruct);
  
	
  DMA2D_BG_InitStruct.DMA2D_BGMA = (uint32_t)imageBackground + desc->pos_x*3 + desc->pos_y*3*LCD_PIXEL_WIDTH;
  /* Input color mode */
  DMA2D_BG_InitStruct.DMA2D_BGCM = CM_RGB888;
  DMA2D_BG_InitStruct.DMA2D_BGPFC_ALPHA_MODE = COMBINE_ALPHA_VALUE;
  DMA2D_BG_InitStruct.DMA2D_BGPFC_ALPHA_VALUE = 0xFF;
  DMA2D_BG_InitStruct.DMA2D_BGO = LCD_PIXEL_WIDTH - desc->width;
  DMA2D_BGConfig(&DMA2D_BG_InitStruct);
}



/*************************** End of file ****************************/

