/**
  ******************************************************************************
  * @file    bsp.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    20-September-2013
  * @brief   bsp
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
#include "Image.h"
#include "timeMeasure.h"
#include "main.h"
#include "main_layerScroll.h"

#define STBLUE ((93 << 0) + (168 << 8) + (223 << 16))

extern volatile unsigned int tamperPushed;

static LTDC_Layer_InitTypeDef LTDC_Layer_InitStruct; 
static void LCD_LayerInit1(void);

/* callback function, it is called during vertical blank synchronization period */
void LayerScrollVBCallback(void)
{
		static int32_t h_offset = 0;
		static int32_t v_offset = 0;
		static int32_t h_direction = 1;
		static int32_t v_direction = 1;
		
		h_offset += h_direction;
		if (h_offset >= Image_width - LCD_PIXEL_WIDTH)
		{
				h_direction = -1;
		}	
		if (h_offset == 0)		
		{
				h_direction = 1;
		}
		
		v_offset += v_direction;
		if (v_offset >= Image_height - LCD_PIXEL_HEIGHT - 5) // sub 5 in order to see complete picture 
		{
				v_direction = -1;
		}	
		if (v_offset == 0)		
		{
				v_direction = 1;
		}		
		
		LTDC_Layer_InitStruct.LTDC_CFBStartAdress = (uint32_t)Image + 3*(h_offset + v_offset*Image_width);
		LTDC_LayerInit(LTDC_Layer2, &LTDC_Layer_InitStruct);
		
		/* LTDC configuration reload */  			
		LTDC_ReloadConfig(LTDC_IMReload);
}

void MainTask_LayerScroll(void) 
{
	LCD_LayerInit1();
	
	SetVerticalBlankCallback(&LayerScrollVBCallback);
	
	while(!tamperPushed)
	{
	}
	
	ResetVerticalBlankCallback();
	
}

static void LCD_LayerInit1(void)
{
  LTDC_LayerCmd(LTDC_Layer2, DISABLE);
	
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
  LTDC_Layer_InitStruct.LTDC_CFBPitch = (Image_width * 3);
  
  /* Configure the number of lines */  
  LTDC_Layer_InitStruct.LTDC_CFBLineNumber = LCD_PIXEL_HEIGHT;
  
  /* Start Address configuration : the LCD Frame buffer is defined in Flash memory*/    
	LTDC_Layer_InitStruct.LTDC_CFBStartAdress = (uint32_t)Image;
  
  /* Initialize LTDC layer 2 - foreground */
  LTDC_LayerInit(LTDC_Layer2, &LTDC_Layer_InitStruct);
    
  /* LTDC configuration reload */  
  LTDC_ReloadConfig(LTDC_IMReload);
  
  /* Enable foreground Layers */
  LTDC_LayerCmd(LTDC_Layer2, ENABLE);
  
  /* LTDC configuration reload */  
  LTDC_ReloadConfig(LTDC_IMReload);

}


/*************************** End of file ****************************/

