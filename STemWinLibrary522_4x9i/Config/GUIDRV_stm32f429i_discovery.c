/*********************************************************************
*          Portions COPYRIGHT 2013 STMicroelectronics                *
*          Portions SEGGER Microcontroller GmbH & Co. KG             *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2013  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.22 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The  software has  been licensed  to STMicroelectronics International
N.V. a Dutch company with a Swiss branch and its headquarters in Plan-
les-Ouates, Geneva, 39 Chemin du Champ des Filles, Switzerland for the
purposes of creating libraries for ARM Cortex-M-based 32-bit microcon_
troller products commercialized by Licensee only, sublicensed and dis_
tributed under the terms and conditions of the End User License Agree_
ment supplied by STMicroelectronics International N.V.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : GUIDRV_stm32f429i_discovery.c
Purpose     : Driver for STM32429I-Discovery board RevB

The part between 'DISPLAY CONFIGURATION START' and 'DISPLAY CONFIGURA-
TION END' can be used to configure the following for each layer:

- Color mode
- Layer size
- Layer orientation

Further the background color used on positions without a valid layer
can be set here.

---------------------------END-OF-HEADER------------------------------
*/

/**
  ******************************************************************************
  * @attention
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

#include "GUI.h"
#include "GUI_Private.h"
#include "GUIDRV_Lin.h"
#include "global_includes.h"

/*********************************************************************
*
*       Display configuration (to be modified)
*
**********************************************************************
*/
/*********************************************************************
*
*       Common
*/

#undef  LCD_SWAP_XY
#undef  LCD_MIRROR_Y

#define LCD_SWAP_XY  0 
#define LCD_MIRROR_Y 1

//
// Physical display size
//
#define XSIZE_PHYS 240
#define YSIZE_PHYS 320

//
// Buffers / VScreens
//
#define NUM_BUFFERS  1 // Number of multiple buffers to be used
#define NUM_VSCREENS 1 // Number of virtual screens to be used

//
// BkColor shown if no layer is active
//
#define BK_COLOR GUI_DARKBLUE

//
// Redefine number of layers for this configuration file. Must be equal or less than in GUIConf.h!
//
#undef  GUI_NUM_LAYERS
#define GUI_NUM_LAYERS 2

/*********************************************************************
*
*       Color mode definitions
*/
#define _CM_ARGB8888 1
#define _CM_RGB888   2
#define _CM_RGB565   3
#define _CM_ARGB1555 4
#define _CM_ARGB4444 5
#define _CM_L8       6
#define _CM_AL44     7
#define _CM_AL88     8

/*********************************************************************
*
*       Layer 0
*/
//
// Color mode layer 0
//
#define COLOR_MODE_0 _CM_RGB565
//
// Layer size
//
#define XSIZE_0 240
#define YSIZE_0 320

/*********************************************************************
*
*       Layer 1
*/
#define COLOR_MODE_1 _CM_ARGB8888
//
// Layer size
//
#define XSIZE_1 240
#define YSIZE_1 320

/*********************************************************************
*
*       Automatic selection of driver and color conversion
*/
#if   (COLOR_MODE_0 == _CM_ARGB8888)
  #define COLOR_CONVERSION_0 GUICC_M8888I
  #define DISPLAY_DRIVER_0   GUIDRV_LIN_32
#elif (COLOR_MODE_0 == _CM_RGB888)
  #define COLOR_CONVERSION_0 GUICC_M888
  #define DISPLAY_DRIVER_0   GUIDRV_LIN_24
#elif (COLOR_MODE_0 == _CM_RGB565)
  #define COLOR_CONVERSION_0 GUICC_M565
  #define DISPLAY_DRIVER_0   GUIDRV_LIN_16
#elif (COLOR_MODE_0 == _CM_ARGB1555)
  #define COLOR_CONVERSION_0 GUICC_M1555I
  #define DISPLAY_DRIVER_0   GUIDRV_LIN_16
#elif (COLOR_MODE_0 == _CM_ARGB4444)
  #define COLOR_CONVERSION_0 GUICC_M4444I
  #define DISPLAY_DRIVER_0   GUIDRV_LIN_16
#elif (COLOR_MODE_0 == _CM_L8)
  #define COLOR_CONVERSION_0 GUICC_8666
  #define DISPLAY_DRIVER_0   GUIDRV_LIN_8
#elif (COLOR_MODE_0 == _CM_AL44)
  #define COLOR_CONVERSION_0 GUICC_1616I
  #define DISPLAY_DRIVER_0   GUIDRV_LIN_8
#elif (COLOR_MODE_0 == _CM_AL88)
  #define COLOR_CONVERSION_0 GUICC_88666I
  #define DISPLAY_DRIVER_0   GUIDRV_LIN_16
#else
  #error Illegal color mode 0!
#endif

#if (GUI_NUM_LAYERS > 1)

#if   (COLOR_MODE_1 == _CM_ARGB8888)
  #define COLOR_CONVERSION_1 GUICC_M8888I
  #define DISPLAY_DRIVER_1   GUIDRV_LIN_32
#elif (COLOR_MODE_1 == _CM_RGB888)
  #define COLOR_CONVERSION_1 GUICC_M888
  #define DISPLAY_DRIVER_1   GUIDRV_LIN_24
#elif (COLOR_MODE_1 == _CM_RGB565)
  #define COLOR_CONVERSION_1 GUICC_M565
  #define DISPLAY_DRIVER_1   GUIDRV_LIN_16
#elif (COLOR_MODE_1 == _CM_ARGB1555)
  #define COLOR_CONVERSION_1 GUICC_M1555I
  #define DISPLAY_DRIVER_1   GUIDRV_LIN_16
#elif (COLOR_MODE_1 == _CM_ARGB4444)
  #define COLOR_CONVERSION_1 GUICC_M4444I
  #define DISPLAY_DRIVER_1   GUIDRV_LIN_16
#elif (COLOR_MODE_1 == _CM_L8)
  #define COLOR_CONVERSION_1 GUICC_8666
  #define DISPLAY_DRIVER_1   GUIDRV_LIN_8
#elif (COLOR_MODE_1 == _CM_AL44)
  #define COLOR_CONVERSION_1 GUICC_1616I
  #define DISPLAY_DRIVER_1   GUIDRV_LIN_8
#elif (COLOR_MODE_1 == _CM_AL88)
  #define COLOR_CONVERSION_1 GUICC_88666I
  #define DISPLAY_DRIVER_1   GUIDRV_LIN_16
#else
  #error Illegal color mode 1!
#endif

#else

/*********************************************************************
*
*       Use complete display automatically in case of only one layer
*/
#undef XSIZE_0
#undef YSIZE_0
#define XSIZE_0 XSIZE_PHYS
#define YSIZE_0 YSIZE_PHYS

#endif

/*********************************************************************
*
*       H/V front/backporch and synchronization width/height
*/
#define HBP  29
#define VBP   3

#define HSW  9
#define VSW  1

#define HFP  10
#define VFP  4

/*********************************************************************
*
*       Configuration checking
*/
#ifndef   XSIZE_PHYS
  #error Physical X size of display is not defined!
#endif
#ifndef   YSIZE_PHYS
  #error Physical Y size of display is not defined!
#endif
#ifndef   NUM_VSCREENS
  #define NUM_VSCREENS 1
#else
  #if (NUM_VSCREENS <= 0)
    #error At least one screeen needs to be defined!
  #endif
#endif
#if (NUM_VSCREENS > 1) && (NUM_BUFFERS > 1)
  #error Virtual screens and multiple buffers are not allowed!
#endif

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static LTDC_Layer_TypeDef       * _apLayer[]        = { LTDC_Layer1, LTDC_Layer2 };
static const U32                  _aAddr[]          = { LCD_FRAME_BUFFER, LCD_FRAME_BUFFER + XSIZE_PHYS * YSIZE_PHYS * sizeof(U32) * NUM_VSCREENS * NUM_BUFFERS };
static int                        _aPendingBuffer[GUI_NUM_LAYERS];
static int                        _aBufferIndex[GUI_NUM_LAYERS];
static int                        _axSize[GUI_NUM_LAYERS];
static int                        _aySize[GUI_NUM_LAYERS];
static int                        _aBytesPerPixels[GUI_NUM_LAYERS];

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static const LCD_API_COLOR_CONV * _apColorConvAPI[] = {
  COLOR_CONVERSION_0,
#if GUI_NUM_LAYERS > 1
  COLOR_CONVERSION_1,
#endif
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _GetPixelformat
*/
static U32 _GetPixelformat(int LayerIndex) {
  const LCD_API_COLOR_CONV * pColorConvAPI;

  if (LayerIndex >= GUI_COUNTOF(_apColorConvAPI)) {
    return 0;
  }
  pColorConvAPI = _apColorConvAPI[LayerIndex];
  if        (pColorConvAPI == GUICC_M8888I) {
    return LTDC_Pixelformat_ARGB8888;
  } else if (pColorConvAPI == GUICC_M888) {
    return LTDC_Pixelformat_RGB888;
  } else if (pColorConvAPI == GUICC_M565) {
    return LTDC_Pixelformat_RGB565;
  } else if (pColorConvAPI == GUICC_M1555I) {
    return LTDC_Pixelformat_ARGB1555;
  } else if (pColorConvAPI == GUICC_M4444I) {
    return LTDC_Pixelformat_ARGB4444;
  } else if (pColorConvAPI == GUICC_8666) {
    return LTDC_Pixelformat_L8;
  } else if (pColorConvAPI == GUICC_1616I) {
    return LTDC_Pixelformat_AL44;
  } else if (pColorConvAPI == GUICC_88666I) {
    return LTDC_Pixelformat_AL88;
  }
  while (1); // Error
}

/*********************************************************************
*
*       _GetBytesPerLine
*/
static int _GetBytesPerLine(int LayerIndex, int xSize) {
  int BitsPerPixel, BytesPerLine;

  BitsPerPixel  = LCD_GetBitsPerPixelEx(LayerIndex);
  BytesPerLine = (BitsPerPixel * xSize + 7) / 8;
  return BytesPerLine;
}

/*********************************************************************
*
*       _LTDC_LayerEnableColorKeying
*/
static void _LTDC_LayerEnableColorKeying(LTDC_Layer_TypeDef * LTDC_Layerx, int NewState) {
  if (NewState != DISABLE) {
    LTDC_Layerx->CR |= (U32)LTDC_LxCR_COLKEN;
  } else {
    LTDC_Layerx->CR &= ~(U32)LTDC_LxCR_COLKEN;
  }
  LTDC->SRCR = LTDC_SRCR_VBR; // Reload on next blanking period
}

/*********************************************************************
*
*       _LTDC_LayerEnableLUT
*/
static void _LTDC_LayerEnableLUT(LTDC_Layer_TypeDef * LTDC_Layerx, int NewState) {
  if (NewState != DISABLE) {
    LTDC_Layerx->CR |= (U32)LTDC_LxCR_CLUTEN;
  } else {
    LTDC_Layerx->CR &= ~(U32)LTDC_LxCR_CLUTEN;
  }
  LTDC->SRCR = LTDC_SRCR_VBR; // Reload on next blanking period
}

/*********************************************************************
*
*       _LTDC_SetLayerPos
*/
static void _LTDC_SetLayerPos(int LayerIndex, int xPos, int yPos) {
  int xSize, ySize;
  U32 HorizontalStart, HorizontalStop, VerticalStart, VerticalStop;

  xSize = LCD_GetXSizeEx(LayerIndex);
  ySize = LCD_GetYSizeEx(LayerIndex);
  HorizontalStart = xPos + HBP + 1;
  HorizontalStop  = xPos + HBP + xSize;
  VerticalStart   = yPos + VBP + 1;
  VerticalStop    = yPos + VBP + ySize;
  //
  // Horizontal start and stop position
  //
  _apLayer[LayerIndex]->WHPCR &= ~(LTDC_LxWHPCR_WHSTPOS | LTDC_LxWHPCR_WHSPPOS);
  _apLayer[LayerIndex]->WHPCR = (HorizontalStart | (HorizontalStop << 16));
  //
  // Vertical start and stop position
  //
  _apLayer[LayerIndex]->WVPCR &= ~(LTDC_LxWVPCR_WVSTPOS | LTDC_LxWVPCR_WVSPPOS);
  _apLayer[LayerIndex]->WVPCR  = (VerticalStart | (VerticalStop << 16));
  //
  // Reload configuration
  //
  LTDC_ReloadConfig(LTDC_SRCR_VBR); // Reload on next blanking period
}

/*********************************************************************
*
*       _LTDC_SetLayerAlpha
*/
static void _LTDC_SetLayerAlpha(int LayerIndex, int Alpha) {
  //
  // Set constant alpha value
  //
  _apLayer[LayerIndex]->CACR &= ~(LTDC_LxCACR_CONSTA);
  _apLayer[LayerIndex]->CACR  = 255 - Alpha;
  //
  // Reload configuration
  //
  LTDC_ReloadConfig(LTDC_SRCR_IMR/*LTDC_SRCR_VBR*/); // Reload on next blanking period/**/
}

/*********************************************************************
*
*       _LTDC_SetLUTEntry
*/
static void _LTDC_SetLUTEntry(int LayerIndex, U32 Color, int Pos) {
  U32 r, g, b, a;

  r = ( Color        & 0xff) << 16;
  g = ((Color >>  8) & 0xff) <<  8;
  b = ((Color >> 16) & 0xff);
  a = Pos << 24;
  _apLayer[LayerIndex]->CLUTWR &= ~(LTDC_LxCLUTWR_BLUE | LTDC_LxCLUTWR_GREEN | LTDC_LxCLUTWR_RED | LTDC_LxCLUTWR_CLUTADD);
  _apLayer[LayerIndex]->CLUTWR  = r | g | b | a;
  //
  // Reload configuration
  //
  LTDC_ReloadConfig(LTDC_SRCR_IMR);
}

/*********************************************************************
*
*       _DMA_Copy
*/
static void _DMA_Copy(int LayerIndex, void * pSrc, void * pDst, int xSize, int ySize, int OffLine) {
  U32 PixelFormat;

  PixelFormat = _GetPixelformat(LayerIndex);
  DMA2D->CR      = 0x00000000UL | (1 << 9);         // Memory to memory and TCIE
  DMA2D->FGMAR   = (U32)pSrc;                       // Source address
  DMA2D->OMAR    = (U32)pDst;                       // Destination address
  DMA2D->FGOR    = OffLine;                         // Source line offset
  DMA2D->OOR     = OffLine;                         // Destination line offset
  DMA2D->FGPFCCR = PixelFormat;                     // Defines the number of pixels to be transfered
  DMA2D->NLR     = (U32)(xSize << 16) | (U16)ySize; // Size configuration of area to be transfered
  DMA2D->CR     |= 1;                               // Start operation
  //
  // Wait until transfer is done
  //
  while (DMA2D->CR & DMA2D_CR_START) {
    //__WFI();                                        // Sleep until next interrupt
  }
}

/*********************************************************************
*
*       _DMA_Fill
*/
static void _DMA_Fill(int LayerIndex, void * pDst, int xSize, int ySize, int OffLine, U32 ColorIndex) {
  U32 PixelFormat;

  PixelFormat = _GetPixelformat(LayerIndex);
  DMA2D->CR      = 0x00030000UL | (1 << 9);         // Register to memory and TCIE
  DMA2D->OCOLR   = ColorIndex;                      // Color to be used
  DMA2D->OMAR    = (U32)pDst;                       // Destination address
  DMA2D->OOR     = OffLine;                         // Destination line offset
  DMA2D->OPFCCR  = PixelFormat;                     // Defines the number of pixels to be transfered
  DMA2D->NLR     = (U32)(xSize << 16) | (U16)ySize; // Size configuration of area to be transfered
  DMA2D->CR     |= 1;                               // Start operation
  //
  // Wait until transfer is done
  //
  while (DMA2D->CR & DMA2D_CR_START) {
    //__WFI();                                        // Sleep until next interrupt
  }
}

/*********************************************************************
*
*       _LCD_DisplayOn
*/
static void _LCD_DisplayOn(void) {
  LCD_DisplayOn();
  LTDC_Cmd(ENABLE); /* display ON */
}

/*********************************************************************
*
*       _LCD_InitController
*/
static void _LCD_InitController(int LayerIndex) {
  LTDC_Layer_InitTypeDef LTDC_Layer_InitStruct = {0};
  static int xSize, ySize, BytesPerLine, BitsPerPixel, i;
  static U32 Pixelformat, Color;
  static int Done;

  if (LayerIndex >= GUI_COUNTOF(_apLayer)) {
    return;
  }
  if (Done == 0) {
    Done = 1;

    LCD_Init();
    //
    // Enable line interrupt
    //
    LTDC_ITConfig(LTDC_IER_LIE, ENABLE);
    NVIC_SetPriority(LTDC_IRQn, 0);
    NVIC_EnableIRQ(LTDC_IRQn);
    //
    // Enable DMA2D transfer complete interrupt
    //
    DMA2D_ITConfig(DMA2D_CR_TCIE, ENABLE);
    NVIC_SetPriority(DMA2D_IRQn, 0);
    NVIC_EnableIRQ(DMA2D_IRQn);
    //
    // Clear transfer complete interrupt flag
    //
    DMA2D->IFCR = (U32)DMA2D_IFSR_CTCIF;
  }
  //
  // Layer configuration
  //
  xSize = LCD_GetXSizeEx(LayerIndex);
  ySize = LCD_GetYSizeEx(LayerIndex);

  LTDC_Layer_InitStruct.LTDC_HorizontalStart = HBP + 1;
  LTDC_Layer_InitStruct.LTDC_HorizontalStop  = (xSize + HBP);
  LTDC_Layer_InitStruct.LTDC_VerticalStart   = VBP + 1;
  LTDC_Layer_InitStruct.LTDC_VerticalStop    = (ySize + VBP);
  //
  // Pixel Format configuration
  //
  Pixelformat = _GetPixelformat(LayerIndex);
  LTDC_Layer_InitStruct.LTDC_PixelFormat = Pixelformat;
  //
  // Alpha constant (255 totally opaque)
  //
  LTDC_Layer_InitStruct.LTDC_ConstantAlpha = 255;
  //
  // Default Color configuration (configure A, R, G, B component values)
  //
  LTDC_Layer_InitStruct.LTDC_DefaultColorBlue  = 0;
  LTDC_Layer_InitStruct.LTDC_DefaultColorGreen = 0;
  LTDC_Layer_InitStruct.LTDC_DefaultColorRed   = 0;
  LTDC_Layer_InitStruct.LTDC_DefaultColorAlpha = 0;
  //
  // Configure blending factors
  //
  BytesPerLine = _GetBytesPerLine(LayerIndex, xSize);
  LTDC_Layer_InitStruct.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_PAxCA;
  LTDC_Layer_InitStruct.LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_PAxCA;
  LTDC_Layer_InitStruct.LTDC_CFBLineLength    = BytesPerLine + 3;
  LTDC_Layer_InitStruct.LTDC_CFBPitch         = BytesPerLine;
  LTDC_Layer_InitStruct.LTDC_CFBLineNumber    = ySize;
  //
  // Input Address configuration
  //
  LTDC_Layer_InitStruct.LTDC_CFBStartAdress = _aAddr[LayerIndex];
  LTDC_LayerInit(_apLayer[LayerIndex], &LTDC_Layer_InitStruct);
  //
  // Enable LUT on demand
  //
  BitsPerPixel = LCD_GetBitsPerPixelEx(LayerIndex);
  if (BitsPerPixel <= 8) {
    //
    // Enable usage of LUT for all modes with <= 8bpp
    //
    _LTDC_LayerEnableLUT(_apLayer[LayerIndex], ENABLE);
  } else {
    //
    // Optional CLUT initialization for AL88 mode (16bpp)
    //
    if (_apColorConvAPI[LayerIndex] == GUICC_88666I) {
      _LTDC_LayerEnableLUT(_apLayer[LayerIndex], ENABLE);
      for (i = 0; i < 256; i++) {
        Color = LCD_API_ColorConv_8666.pfIndex2Color(i);
        _LTDC_SetLUTEntry(LayerIndex, Color, i);
      }
    }
  }
  //
  // Enable layer
  //
  LTDC_LayerCmd(_apLayer[LayerIndex], ENABLE);
  //
  // Reload configuration
  //
  LTDC_ReloadConfig(LTDC_SRCR_IMR);
}

/*********************************************************************
*
*       _GetBufferSize
*/
static U32 _GetBufferSize(int LayerIndex) {
  U32 BufferSize;

  BufferSize = _axSize[LayerIndex] * _aySize[LayerIndex] * _aBytesPerPixels[LayerIndex];
  return BufferSize;
}

/*********************************************************************
*
*       _LCD_CopyBuffer
*/
static void _LCD_CopyBuffer(int LayerIndex, int IndexSrc, int IndexDst) {
  U32 BufferSize, AddrSrc, AddrDst;

  BufferSize = _GetBufferSize(LayerIndex);
  AddrSrc    = _aAddr[LayerIndex] + BufferSize * IndexSrc;
  AddrDst    = _aAddr[LayerIndex] + BufferSize * IndexDst;
  _DMA_Copy(LayerIndex, (void *)AddrSrc, (void *)AddrDst, _axSize[LayerIndex], _aySize[LayerIndex], 0);
}

/*********************************************************************
*
*       _LCD_CopyRect
*/
static void _LCD_CopyRect(int LayerIndex, int x0, int y0, int x1, int y1, int xSize, int ySize) {
  U32 BufferSize, AddrSrc, AddrDst;

  BufferSize = _GetBufferSize(LayerIndex);
  AddrSrc = _aAddr[LayerIndex] + BufferSize * _aBufferIndex[LayerIndex] + (y0 * _axSize[LayerIndex] + x0) * _aBytesPerPixels[LayerIndex];
  AddrDst = _aAddr[LayerIndex] + BufferSize * _aBufferIndex[LayerIndex] + (y1 * _axSize[LayerIndex] + x1) * _aBytesPerPixels[LayerIndex];
  _DMA_Copy(LayerIndex, (void *)AddrSrc, (void *)AddrDst, xSize, ySize, _axSize[LayerIndex] - xSize);
}

/*********************************************************************
*
*       _LCD_FillRect
*/
static void _LCD_FillRect(int LayerIndex, int x0, int y0, int x1, int y1, U32 PixelIndex) {
  U32 BufferSize, AddrDst;
  int xSize, ySize;

  if (GUI_GetDrawMode() == GUI_DM_XOR) {
    LCD_SetDevFunc(LayerIndex, LCD_DEVFUNC_FILLRECT, NULL);
    LCD_FillRect(x0, y0, x1, y1);
    LCD_SetDevFunc(LayerIndex, LCD_DEVFUNC_FILLRECT, (void(*)(void))_LCD_FillRect);
  } else {
    xSize = x1 - x0 + 1;
    ySize = y1 - y0 + 1;
    BufferSize = _GetBufferSize(LayerIndex);
    AddrDst = _aAddr[LayerIndex] + BufferSize * _aBufferIndex[LayerIndex] + (y0 * _axSize[LayerIndex] + x0) * _aBytesPerPixels[LayerIndex];
    _DMA_Fill(LayerIndex, (void *)AddrDst, xSize, ySize, _axSize[LayerIndex] - xSize, PixelIndex);
  }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       DMA2D_ISR_Handler
*
* Purpose:
*   Transfer-complete-interrupt of DMA2D
*/
void DMA2D_ISR_Handler(void) {
  DMA2D->IFCR = (U32)DMA2D_IFSR_CTCIF;
}

/*********************************************************************
*
*       LTDC_ISR_Handler
*
* Purpose:
*   End-Of-Frame-Interrupt for managing multiple buffering
*/
void LTDC_ISR_Handler(void) {
  U32 Addr;
  int i;

  LTDC->ICR = (U32)LTDC_IER_LIE;
  for (i = 0; i < GUI_NUM_LAYERS; i++) {
    if (_aPendingBuffer[i] >= 0) {
      //
      // Calculate address of buffer to be used  as visible frame buffer
      //
      Addr = _aAddr[i] + _axSize[i] * _aySize[i] * _aPendingBuffer[i] * _aBytesPerPixels[i];
      //
      // Store address into SFR
      //
      _apLayer[i]->CFBAR &= ~(LTDC_LxCFBAR_CFBADD);
      _apLayer[i]->CFBAR = Addr;
      //
      // Reload configuration
      //
      LTDC_ReloadConfig(LTDC_SRCR_IMR);
      //
      // Tell emWin that buffer is used
      //
      GUI_MULTIBUF_ConfirmEx(i, _aPendingBuffer[i]);
      //
      // Clear pending buffer flag of layer
      //
      _aBufferIndex[i] = _aPendingBuffer[i];
      _aPendingBuffer[i] = -1;
    }
  }
}

/*********************************************************************
*
*       LCD_X_DisplayDriver
*
* Purpose:
*   This function is called by the display driver for several purposes.
*   To support the according task the routine needs to be adapted to
*   the display controller. Please note that the commands marked with
*   'optional' are not cogently required and should only be adapted if
*   the display controller supports these features.
*
* Parameter:
*   LayerIndex - Index of layer to be configured
*   Cmd        - Please refer to the details in the switch statement below
*   pData      - Pointer to a LCD_X_DATA structure
*
* Return Value:
*   < -1 - Error
*     -1 - Command not handled
*      0 - Ok
*/
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData) {
  int r = 0;

  switch (Cmd) {
  case LCD_X_INITCONTROLLER: {
    //
    // Called during the initialization process in order to set up the display controller and put it into operation.
    //
    _LCD_InitController(LayerIndex);
    break;
  }
  case LCD_X_SETORG: {
    //
    // Required for setting the display origin which is passed in the 'xPos' and 'yPos' element of p
    //
    LCD_X_SETORG_INFO * p;

    p = (LCD_X_SETORG_INFO *)pData;
    _apLayer[LayerIndex]->CFBAR = _aAddr[LayerIndex] + p->yPos * _axSize[LayerIndex] * _aBytesPerPixels[LayerIndex];
    LTDC_ReloadConfig(LTDC_SRCR_VBR); // Reload on next blanking period
    break;
  }
  case LCD_X_SHOWBUFFER: {
    //
    // Required if multiple buffers are used. The 'Index' element of p contains the buffer index.
    //
    LCD_X_SHOWBUFFER_INFO * p;

    p = (LCD_X_SHOWBUFFER_INFO *)pData;
    _aPendingBuffer[LayerIndex] = p->Index;
    break;
  }
  case LCD_X_SETLUTENTRY: {
    //
    // Required for setting a lookup table entry which is passed in the 'Pos' and 'Color' element of p
    //
    LCD_X_SETLUTENTRY_INFO * p;

    p = (LCD_X_SETLUTENTRY_INFO *)pData;
    _LTDC_SetLUTEntry(LayerIndex, p->Color, p->Pos);
    break;
  }
  case LCD_X_ON: {
    //
    // Required if the display controller should support switching on and off
    //
    _LCD_DisplayOn();
    break;
  }
  case LCD_X_OFF: {
    //
    // Required if the display controller should support switching on and off
    //
    LCD_DisplayOff();
    break;
  }
  case LCD_X_SETVIS: {
    //
    // Required for setting the layer visibility which is passed in the 'OnOff' element of pData
    //
    LCD_X_SETVIS_INFO * p;

    p = (LCD_X_SETVIS_INFO *)pData;
    LTDC_LayerCmd(_apLayer[LayerIndex], p->OnOff ? ENABLE : DISABLE);
    break;
  }
  case LCD_X_SETPOS: {
    //
    // Required for setting the layer position which is passed in the 'xPos' and 'yPos' element of pData
    //
    LCD_X_SETPOS_INFO * p;

    p = (LCD_X_SETPOS_INFO *)pData;
    _LTDC_SetLayerPos(LayerIndex, p->xPos, p->yPos);
    break;
  }
  case LCD_X_SETSIZE: {
    //
    // Required for setting the layer position which is passed in the 'xPos' and 'yPos' element of pData
    //
    LCD_X_SETSIZE_INFO * p;
    int xPos, yPos;

    GUI_GetLayerPosEx(LayerIndex, &xPos, &yPos);
    p = (LCD_X_SETSIZE_INFO *)pData;
    _axSize[LayerIndex] = p->xSize;
    _aySize[LayerIndex] = p->ySize;
    _LTDC_SetLayerPos(LayerIndex, xPos, yPos);
    break;
  }
  case LCD_X_SETALPHA: {
    //
    // Required for setting the alpha value which is passed in the 'Alpha' element of pData
    //
    LCD_X_SETALPHA_INFO * p;

    p = (LCD_X_SETALPHA_INFO *)pData;
    _LTDC_SetLayerAlpha(LayerIndex, p->Alpha);
    break;
  }
  case LCD_X_SETCHROMAMODE: {
    //
    // Required for setting the chroma mode which is passed in the 'ChromaMode' element of pData
    //
    LCD_X_SETCHROMAMODE_INFO * p;

    p = (LCD_X_SETCHROMAMODE_INFO *)pData;
    _LTDC_LayerEnableColorKeying(_apLayer[LayerIndex], (p->ChromaMode != 0) ? ENABLE : DISABLE);
    break;
  }
  case LCD_X_SETCHROMA: {
    //
    // Required for setting the chroma value which is passed in the 'ChromaMin' and 'ChromaMax' element of pData
    //
    LCD_X_SETCHROMA_INFO * p;
    U32 Color;

    p = (LCD_X_SETCHROMA_INFO *)pData;
    Color = ((p->ChromaMin & 0xFF0000) >> 16) | (p->ChromaMin & 0x00FF00) | ((p->ChromaMin & 0x0000FF) << 16);
    _apLayer[LayerIndex]->CKCR = Color;
    LTDC_ReloadConfig(LTDC_SRCR_VBR); // Reload on next blanking period
    break;
  }
  default:
    r = -1;
  }
  return r;
}

/*********************************************************************
*
*       LCD_X_Config
*
* Purpose:
*   Called during the initialization process in order to set up the
*   display driver configuration.
*
*/
void LCD_X_Config(void) {
  int i;

  //
  // At first initialize use of multiple buffers on demand
  //
  #if (NUM_BUFFERS > 1)
    for (i = 0; i < GUI_NUM_LAYERS; i++) {
      GUI_MULTIBUF_ConfigEx(i, NUM_BUFFERS);
    }
  #endif
  //
  // Set display driver and color conversion for 1st layer
  //
  GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER_0, COLOR_CONVERSION_0, 0, 0);
  //
  // Set size of 1st layer
  //
  LCD_SetSizeEx (0, XSIZE_0, YSIZE_0);
  LCD_SetVSizeEx(0, XSIZE_0, YSIZE_0 * NUM_VSCREENS);
  #if (GUI_NUM_LAYERS > 1)
    //
    // Set display driver and color conversion for 2nd layer
    //
    GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER_1, COLOR_CONVERSION_1, 0, 1);
    //
    // Set size of 2nd layer
    //
    LCD_SetSizeEx (1, XSIZE_1, YSIZE_1);
    LCD_SetVSizeEx(1, XSIZE_1, YSIZE_1 * NUM_VSCREENS);
  #endif
  //
  // Setting up VRam address and custom functions for CopyBuffer-, CopyRect- and FillRect operations
  //
  for (i = 0; i < GUI_NUM_LAYERS; i++) {
    _aPendingBuffer[i] = -1;
    //
    // Set VRAM address
    //
    LCD_SetVRAMAddrEx(i, (void *)(_aAddr[i]));
    //
    // Remember color depth for further operations
    //
    _aBytesPerPixels[i] = LCD_GetBitsPerPixelEx(i) >> 3;
    //
    // Set custom functions for several operations
    //
    LCD_SetDevFunc(i, LCD_DEVFUNC_COPYBUFFER, (void(*)(void))_LCD_CopyBuffer);
    LCD_SetDevFunc(i, LCD_DEVFUNC_COPYRECT,   (void(*)(void))_LCD_CopyRect);
    //
    // Filling via DMA2D does only work with 16bpp or more
    //
    if (_GetPixelformat(i) <= LTDC_Pixelformat_ARGB4444) {
      LCD_SetDevFunc(i, LCD_DEVFUNC_FILLRECT, (void(*)(void))_LCD_FillRect);
    }
  }
}

/*************************** End of file ****************************/
