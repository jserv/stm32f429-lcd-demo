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
#include "main.h"
#include "main_2layers.h"

#include "timeMeasure.h"
// !! TO BE MODIFIED !!
#include "AlphaImage.h"
#include "ImageCover.h"

#define ALPHA_COUNTER_THRESHOLD1 100
#define ALPHA_COUNTER_THRESHOLD2 120
#define ALPHA_COUNTER_THRESHOLD3 140
#define ALPHA_COUNTER_THRESHOLD4 160
#define ALPHA_COUNTER_THRESHOLD5 180

#define FRAME_RATE	60


extern volatile unsigned int tamperPushed;

static LTDC_Layer_InitTypeDef LTDC_Layer_InitStruct; 
static LTDC_Layer_InitTypeDef LTDC_LogoLayer_InitStruct;

static void LCD_BackgroundLayerInit(void);

static void LCD_LogoLayer(void);

static void LogoLayerScrollVBCallback(void)
{
    TP_STATE  *ts;
    
    static int32_t h_offset = 0;
    static int32_t v_offset = 0;
    static int32_t h_direction = 1;
    static int32_t v_direction = 1;
    static int32_t alpha = 255;
    static int32_t alpha_counter = 0;
            
    static float v_position = 0.0;
    static float acceleration = 3.0;
    static float speed = 0.0;
    
    float diff;

    /* bouncing */ 
    speed += acceleration / (float)FRAME_RATE;
    v_position += speed;
    speed = speed * 0.999;

		// !! TO BE MODIFIED !!
    diff = v_position + (float)AlphaImage_height - (float)LCD_PIXEL_HEIGHT;
    if (diff>=(float)0.0)
    {
			v_position = v_position - diff;
			speed = (-speed);
    }
            
    
    /* blending / blinking */
    alpha_counter ++;
    if ( alpha_counter < ALPHA_COUNTER_THRESHOLD1)
    {
    }
    else if ( alpha_counter < ALPHA_COUNTER_THRESHOLD2)
    {	
			alpha -= (255/(ALPHA_COUNTER_THRESHOLD2-ALPHA_COUNTER_THRESHOLD1+1));
    }
    else if ( alpha_counter < ALPHA_COUNTER_THRESHOLD3)
    {
			alpha += (255/(ALPHA_COUNTER_THRESHOLD3-ALPHA_COUNTER_THRESHOLD2+1));
    }
    else if ( alpha_counter < ALPHA_COUNTER_THRESHOLD4)			
    {
			alpha -= (255/(ALPHA_COUNTER_THRESHOLD4-ALPHA_COUNTER_THRESHOLD3+1));
    }
    else if ( alpha_counter < ALPHA_COUNTER_THRESHOLD5)			
    {
    	alpha += (255/(ALPHA_COUNTER_THRESHOLD5-ALPHA_COUNTER_THRESHOLD4+1));
    }
    else if ( alpha_counter >= ALPHA_COUNTER_THRESHOLD5)			
    {
			alpha_counter = 0;
			alpha = 255;
    }			
    
    h_offset += h_direction;
		// !! TO BE MODIFIED !!
    if (h_offset >= LCD_PIXEL_WIDTH - AlphaImage_width)
    {
    	h_direction = -1;
    }	
    if (h_offset <= 0)		
    {
    	h_direction = 1;
    }
    
    v_offset += v_direction;
		// !! TO BE MODIFIED !!
    if (v_offset >= LCD_PIXEL_HEIGHT - AlphaImage_height) // sub 5 in order to see complete picture 
    {
    	v_direction = -1;
    }	
    if (v_offset <= 0)		
    {
    	v_direction = 1;
    }		
    
    v_offset = (uint32_t)v_position;
    
    LTDC_LogoLayer_InitStruct.LTDC_ConstantAlpha = alpha;
    
    LTDC_LogoLayer_InitStruct.LTDC_HorizontalStart = 30 + h_offset;
		// !! TO BE MODIFIED !!
    LTDC_LogoLayer_InitStruct.LTDC_HorizontalStop = (AlphaImage_width + 30 - 1) + h_offset; 
    LTDC_LogoLayer_InitStruct.LTDC_VerticalStart = 4 + v_offset;
		// !! TO BE MODIFIED !!
    LTDC_LogoLayer_InitStruct.LTDC_VerticalStop = (AlphaImage_height + 4 - 1) + v_offset;
    
    LTDC_LayerInit(LTDC_Layer2, &LTDC_LogoLayer_InitStruct);
    
    /* LTDC configuration reload */  			
    LTDC_ReloadConfig(LTDC_IMReload);
    
    /* manage touch screen */
    ts = IOE_TP_GetState();
       
    if(ts->TouchDetected == 0x80)
    {
      speed = 0.0;
      v_position = (float)ts->Y - ((float)AlphaImage_height / 2.0);
      if (v_position >= LCD_PIXEL_HEIGHT - AlphaImage_height)
      {
        v_position = LCD_PIXEL_HEIGHT - AlphaImage_height;
      }
      if (v_position < 0)
      {
        v_position = 0;
      }
      
      h_offset = ts->X - (int32_t)(AlphaImage_width / 2.0);
      if (h_offset >= LCD_PIXEL_WIDTH - AlphaImage_width)
      {
        h_offset = LCD_PIXEL_WIDTH - AlphaImage_width;
      }
      if (h_offset < 0)
      {
        h_offset = 0;
      }    
    }	
}

void MainTask_2Layers(void) 
{		
	LCD_Init();
        
        IOE_Config();
		
	LCD_ReSetColorKeying();
		
	LTDC_DitherCmd(ENABLE);
	
	LTDC_LayerCmd(LTDC_Layer2, DISABLE);
	  
	/* LTDC configuration reload */  
  LTDC_ReloadConfig(LTDC_IMReload);
	
	LCD_BackgroundLayerInit();

	/* LTDC configuration reload */  
  LTDC_ReloadConfig(LTDC_IMReload);
	
	LCD_LogoLayer();
	
	SetVerticalBlankCallback(&LogoLayerScrollVBCallback);

	while (!tamperPushed)
	{
	}	
	
	ResetVerticalBlankCallback();
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
  LTDC_Layer_InitStruct.LTDC_CFBPitch = (ImageCover_width * 3);
  
  /* Configure the number of lines */  
  LTDC_Layer_InitStruct.LTDC_CFBLineNumber = LCD_PIXEL_HEIGHT;
  
  /* Start Address configuration : the LCD Frame buffer is defined in Flash memory*/    
	LTDC_Layer_InitStruct.LTDC_CFBStartAdress = (uint32_t)ImageCover;
  
  /* Initialize LTDC layer 1 */
  LTDC_LayerInit(LTDC_Layer1, &LTDC_Layer_InitStruct);
    
  /* LTDC configuration reload */  
  LTDC_ReloadConfig(LTDC_IMReload);
  
  /* Enable foreground Layers */
  LTDC_LayerCmd(LTDC_Layer1, ENABLE);
  
  /* LTDC configuration reload */  
  LTDC_ReloadConfig(LTDC_IMReload);

}

static void LCD_LogoLayer(void)
{
  LTDC_LayerCmd(LTDC_Layer2, DISABLE);
	
  /* Windowing configuration */
  /* In this case all the active display area is used to display a picture then :
  Horizontal start = horizontal synchronization + Horizontal back porch = 30 
  Horizontal stop = Horizontal start + window width -1 = 30 + 240 -1
  Vertical start   = vertical synchronization + vertical back porch     = 4
  Vertical stop   = Vertical start + window height -1  = 4 + 320 -1      */      
  LTDC_LogoLayer_InitStruct.LTDC_HorizontalStart = 30;
	// !! TO BE MODIFIED !!
  LTDC_LogoLayer_InitStruct.LTDC_HorizontalStop = (AlphaImage_width + 30 - 1); 
  LTDC_LogoLayer_InitStruct.LTDC_VerticalStart = 4;
	// !! TO BE MODIFIED !!
  LTDC_LogoLayer_InitStruct.LTDC_VerticalStop = (AlphaImage_height + 4 - 1);
  
  /* Pixel Format configuration*/
	// !! TO BE MODIFIED !!
  LTDC_LogoLayer_InitStruct.LTDC_PixelFormat = LTDC_Pixelformat_ARGB4444;
  /* Alpha constant (255 totally opaque) */
  LTDC_LogoLayer_InitStruct.LTDC_ConstantAlpha = 255; 
  /* Default Color configuration (configure A,R,G,B component values) */          
  LTDC_LogoLayer_InitStruct.LTDC_DefaultColorBlue = 0;        
  LTDC_LogoLayer_InitStruct.LTDC_DefaultColorGreen = 0;       
  LTDC_LogoLayer_InitStruct.LTDC_DefaultColorRed = 0;         
  LTDC_LogoLayer_InitStruct.LTDC_DefaultColorAlpha = 0;
  /* Configure blending factors */       
  LTDC_LogoLayer_InitStruct.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_PAxCA;    
  LTDC_LogoLayer_InitStruct.LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_PAxCA;
  
  /* the length of one line of pixels in bytes + 3 then :
  Line Lenth = Active width x number of bytes per pixel + 3 
  Active width         = LCD_PIXEL_WIDTH 
  number of bytes per pixel = 4    (pixel_format : ARGB8888) 
  */
	
	// !! TO BE MODIFIED !!
  LTDC_LogoLayer_InitStruct.LTDC_CFBLineLength = ((AlphaImage_width * 2) + 3);///
  /* the pitch is the increment from the start of one line of pixels to the 
  start of the next line in bytes, then :
  Pitch = Active high width x number of bytes per pixel */ 
	// !! TO BE MODIFIED !!
  LTDC_LogoLayer_InitStruct.LTDC_CFBPitch = (AlphaImage_width * 2);///
  
  /* Configure the number of lines */  
	// !! TO BE MODIFIED !!
  LTDC_LogoLayer_InitStruct.LTDC_CFBLineNumber = AlphaImage_height;
  
  /* Start Address configuration : the LCD Frame buffer is defined in Flash memory*/    
	// !! TO BE MODIFIED !!
	LTDC_LogoLayer_InitStruct.LTDC_CFBStartAdress = (uint32_t)AlphaImage;
  
  /* Initialize LTDC layer 2 */
  LTDC_LayerInit(LTDC_Layer2, &LTDC_LogoLayer_InitStruct);
    
  /* LTDC configuration reload */  
  LTDC_ReloadConfig(LTDC_IMReload);
  
  /* Enable foreground Layers */
  LTDC_LayerCmd(LTDC_Layer2, ENABLE);
  
  /* LTDC configuration reload */  
  LTDC_ReloadConfig(LTDC_IMReload);
}


/*************************** End of file ****************************/

