/**
  ******************************************************************************
  * @file    main_colorKeying.c
  * @author  TOMAS team
  * @version V1.0.0
  * @date    3-October-2013
  * @brief   Demonstration of color keying feature
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
//#include "image.h"
#include "main.h"
#include "main_colorKeying.h"

#include "timeMeasure.h"
#include "ImageSTLogo.h"
#include "ImageRGB888.h"

#define STBLUE ((93 << 0) + (168 << 8) + (223 << 16))
#define LOGO_OFFSET_LINES 30
#define LOGO_OFFSET (LOGO_OFFSET_LINES * ImageSTLogo_width * 2) /* address offset for logo = number of lines * pixels per line * bytes per pixel */


extern volatile unsigned int tamperPushed;

static LTDC_Layer_InitTypeDef LTDC_Layer_InitStruct; 

static void LCD_BackgroundLayerInit(void);

static void DMA2D_CopySTLogo(uint32_t lineOffset);
static void DrawText(uint8_t* text);

static uint32_t *frameBuffer = (uint32_t*)SDRAM_BANK_ADDR;

void MainTask_ColorKeying(void) 
{	
	uint32_t lastMiliseconds = 0;
	
	LCD_Init();
	SDRAM_Init();
		
	while(!tamperPushed)
	{
			LCD_ReSetColorKeying();
		
			LTDC_DitherCmd(ENABLE);
				
			DrawText((uint8_t*)" Color Keying OFF");
			DMA2D_CopySTLogo(LOGO_OFFSET);
			LCD_BackgroundLayerInit();
		
			/* wait 2s */
			lastMiliseconds = getMiliseconds();
			while (!tamperPushed && ((getMiliseconds()-lastMiliseconds)<2000))
			{
			}
		
			LCD_SetColorKeying(0xFFFFFF);
			DrawText((uint8_t*)" Color Keying ON");
			DMA2D_CopySTLogo(LOGO_OFFSET);

			/* wait 2s */
			lastMiliseconds = getMiliseconds();
			while (!tamperPushed && ((getMiliseconds()-lastMiliseconds)<2000))
			{
			}			
						
			LTDC_CLUTCmd(LTDC_Layer1, DISABLE);			
			LTDC_CLUTCmd(LTDC_Layer2, DISABLE);			
			LCD_ReSetColorKeying();
			
			LTDC_ReloadConfig(LTDC_IMReload);
	}
	
}

static void DrawText(uint8_t* text)
{
  /* Windowing configuration */
  /* In this case all the active display area is used to display a picture then :
     Horizontal start = horizontal synchronization + Horizontal back porch = 30 
     Horizontal stop = Horizontal start + window width -1 = 30 + 240 -1
     Vertical start   = vertical synchronization + vertical back porch     = 4
     Vertical stop   = Vertical start + window height -1  = 4 + 320 -1      */      
  LTDC_Layer_InitStruct.LTDC_HorizontalStart = 30;
  LTDC_Layer_InitStruct.LTDC_HorizontalStop = (LCD_PIXEL_WIDTH + 30 - 1); 
  LTDC_Layer_InitStruct.LTDC_VerticalStart = 4 + 10;
  LTDC_Layer_InitStruct.LTDC_VerticalStop = (ImageSTLogo_height + 4 + 10 + LOGO_OFFSET_LINES - 1);
  
  /* Pixel Format configuration*/
  LTDC_Layer_InitStruct.LTDC_PixelFormat = LTDC_Pixelformat_RGB565;
  /* Alpha constant (255 totally opaque) */
  LTDC_Layer_InitStruct.LTDC_ConstantAlpha = 255; 
  /* Default Color configuration (configure A,R,G,B component values) */          
  LTDC_Layer_InitStruct.LTDC_DefaultColorBlue = 0;        
  LTDC_Layer_InitStruct.LTDC_DefaultColorGreen = 0;       
  LTDC_Layer_InitStruct.LTDC_DefaultColorRed = 0;         
  LTDC_Layer_InitStruct.LTDC_DefaultColorAlpha = 0; /* 0 = transparent */
  
  /* the length of one line of pixels in bytes + 3 then :
  Line Lenth = Active high width x number of bytes per pixel + 3 
  Active high width         = LCD_PIXEL_WIDTH 
  number of bytes per pixel = 2    (pixel_format : RGB565) 
  */
  LTDC_Layer_InitStruct.LTDC_CFBLineLength = ((LCD_PIXEL_WIDTH * 2) + 3);
  /* the pitch is the increment from the start of one line of pixels to the 
  start of the next line in bytes, then :
  Pitch = Active high width x number of bytes per pixel */ 
  LTDC_Layer_InitStruct.LTDC_CFBPitch = (LCD_PIXEL_WIDTH * 2);
  
  /* Configure the number of lines */  
  LTDC_Layer_InitStruct.LTDC_CFBLineNumber = LCD_PIXEL_HEIGHT;
    
  /* Configure Layer2 */
  /* Start Address configuration : the LCD Frame buffer is defined on SDRAM w/ Offset */     
  LTDC_Layer_InitStruct.LTDC_CFBStartAdress = LCD_FRAME_BUFFER + BUFFER_OFFSET;
  
    /* Configure blending factors */       
  LTDC_Layer_InitStruct.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_PAxCA;    
  LTDC_Layer_InitStruct.LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_PAxCA;

  /* Initialize LTDC layer 2 */
  LTDC_LayerInit(LTDC_Layer2, &LTDC_Layer_InitStruct);

  /* LTDC configuration reload */  
  LTDC_ReloadConfig(LTDC_IMReload);
  
  /* Enable foreground & background Layers */
  LTDC_LayerCmd(LTDC_Layer2, ENABLE);
	
		/* Set default font */    
  LCD_SetFont(&LCD_DEFAULT_FONT); 
	
  /* Set LCD foreground layer */
  LCD_SetLayer(LCD_FOREGROUND_LAYER);
  
  /* LCD display message */
	LCD_Clear(LCD_COLOR_WHITE);		
	LCD_SetFont(&Font12x12);
  LCD_SetTextColor(LCD_COLOR_RED); 
	LCD_SetBackColor(LCD_COLOR_WHITE);
  LCD_DisplayStringLine(LCD_LINE_1,(uint8_t*)text);
}

static void LCD_BackgroundLayerInit(void)
{
  LTDC_LayerCmd(LTDC_Layer1, DISABLE);
	
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
  LTDC_Layer_InitStruct.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_PAxCA;    
  LTDC_Layer_InitStruct.LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_PAxCA;
  
  /* the length of one line of pixels in bytes + 3 then :
  Line Lenth = Active width x number of bytes per pixel + 3 
  Active width         = LCD_PIXEL_WIDTH 
  number of bytes per pixel = 3    (pixel_format : RGB888) 
  */
  LTDC_Layer_InitStruct.LTDC_CFBLineLength = ((LCD_PIXEL_WIDTH * 3) + 3);
  /* the pitch is the increment from the start of one line of pixels to the 
  start of the next line in bytes, then :
  Pitch = Active high width x number of bytes per pixel */ 
  LTDC_Layer_InitStruct.LTDC_CFBPitch = (ImageRGB888_width * 3);
  
  /* Configure the number of lines */  
  LTDC_Layer_InitStruct.LTDC_CFBLineNumber = LCD_PIXEL_HEIGHT;
  
  /* Start Address configuration : the LCD Frame buffer is defined in Flash memory*/    
  //LTDC_Layer_InitStruct.LTDC_CFBStartAdress = (uint32_t)frameBuffer;
	LTDC_Layer_InitStruct.LTDC_CFBStartAdress = (uint32_t)ImageRGB888;
  
  /* Initialize LTDC layer 1 */
  LTDC_LayerInit(LTDC_Layer1, &LTDC_Layer_InitStruct);
    
  /* LTDC configuration reload */  
  LTDC_ReloadConfig(LTDC_IMReload);
  
  /* Enable foreground Layers */
  LTDC_LayerCmd(LTDC_Layer1, ENABLE);
  
  /* LTDC configuration reload */  
  LTDC_ReloadConfig(LTDC_IMReload);
}

static void DMA2D_CopySTLogo(uint32_t lineOffset)
{
  DMA2D_InitTypeDef      DMA2D_InitStruct;
  DMA2D_FG_InitTypeDef   DMA2D_FG_InitStruct;
  DMA2D_BG_InitTypeDef   DMA2D_BG_InitStruct;

  /* Enable the DMA2D Clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2D, ENABLE);
  
  /* Transfer mode => Memory to memory with pixel format conversion*/
  DMA2D_InitStruct.DMA2D_Mode = DMA2D_M2M_PFC; 
  /* Output color mode */
  DMA2D_InitStruct.DMA2D_CMode = DMA2D_RGB565; 
  /* Output Address at SRAM memory */
  DMA2D_InitStruct.DMA2D_OutputMemoryAdd = (uint32_t)(LCD_FRAME_BUFFER + BUFFER_OFFSET + lineOffset);
  /* Initialize the alpha and RGB values */
  DMA2D_InitStruct.DMA2D_OutputGreen = 0;
  DMA2D_InitStruct.DMA2D_OutputBlue = 0;
  DMA2D_InitStruct.DMA2D_OutputRed = 0;
  DMA2D_InitStruct.DMA2D_OutputAlpha = 0;
  /* Initialize the output offset */
  DMA2D_InitStruct.DMA2D_OutputOffset = 0;
  /* Number of lines : height */
  DMA2D_InitStruct.DMA2D_NumberOfLine = ImageSTLogo_height; 
  /* Number of pixel per line : width */
  DMA2D_InitStruct.DMA2D_PixelPerLine = ImageSTLogo_width;
  
  /* Initialize DMA2D */
  DMA2D_Init(&DMA2D_InitStruct);

  /* Configure default values for foreground */
  DMA2D_FG_StructInit(&DMA2D_FG_InitStruct);
  
  /* Input Address at flash memory */
  DMA2D_FG_InitStruct.DMA2D_FGMA = (uint32_t)ImageSTLogo;
  
  DMA2D_FG_InitStruct.DMA2D_FGPFC_ALPHA_VALUE = 100;
    
  /* Input color mode */
  DMA2D_FG_InitStruct.DMA2D_FGCM = CM_RGB565;
  
  /* Initialize foreground */
  DMA2D_FGConfig(&DMA2D_FG_InitStruct);
  
  
  /* Configure default values for background */
  DMA2D_BG_StructInit(&DMA2D_BG_InitStruct);
  
  /* Input Address at flash memory */
  DMA2D_BG_InitStruct.DMA2D_BGMA = (uint32_t)frameBuffer;
  
  DMA2D_BG_InitStruct.DMA2D_BGPFC_ALPHA_VALUE = 100;
  /* Input color mode */
  DMA2D_BG_InitStruct.DMA2D_BGCM = CM_RGB565;
  
  /* Initialize background */
  DMA2D_BGConfig(&DMA2D_BG_InitStruct);
	
	  /* Start Transfer */
  DMA2D_StartTransfer();  
	
	while (!DMA2D_GetFlagStatus(DMA2D_FLAG_TC))
	{
	}
}


/*************************** End of file ****************************/

