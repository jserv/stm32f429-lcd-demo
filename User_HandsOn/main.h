/**
  ******************************************************************************
  * @file    main.h
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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "global_includes.h"

/* Types -------------------------------------------------------------------- */	 
typedef void Callback_t(void); 
	 
/* Exported functions ------------------------------------------------------- */	 
void BSP_Background (void);
uint32_t emWinRunning(void);
void SetemWinRunning(uint32_t flag);



	 
void SetVerticalBlankCallback(Callback_t *);
void ResetVerticalBlankCallback(void);

void MainTask_2D (void);
void MainTask_2D_text (void);
void MainTask_2D_AAtext (void);
void MainTask_2D_text_bmp(void);
void MainTask_WM_button(void);
void MainTask_WM_button_list(void);
void MainTask_WM_button_list_icons(void);
void MainTask_WM_button_skin(void);
void MainTask_Graph (void);
void MainTask_Gyro (void);
	 
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
