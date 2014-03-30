/**
  ******************************************************************************
  * @file    timemeasure.c
  * @author  TOMAS team
  * @version 
  * @date    
  * @brief   This file provides functions for time measurement
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

/* Includes ------------------------------------------------------------------*/
#include "bsp.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

static uint32_t timeStart;
static uint32_t timeEnd;
static uint32_t miliseconds;

void timeMeasureMilisecondTick(void)
{
	miliseconds++;
}

void timeMeasureStart(void)
{
	miliseconds = 0;
  timeStart = SysTick->VAL;

}

/**
* @brief  timeMeasureEnd
* @param  None
* @retval returns the time elapsed from last call of timeMeasureStart
*/ 
uint32_t timeMeasureEnd (void)
{

  timeEnd = SysTick->VAL;
  
  if (timeEnd < timeStart)
  {
		return (miliseconds * 10) + (10*(timeStart - timeEnd) / SysTick->LOAD);
  }
  else // underflow of the Systick
  {
		return ((miliseconds - 1) * 10) + (10*(timeStart + SysTick->LOAD - timeEnd) / SysTick->LOAD);
  }
	
}  

uint32_t getMiliseconds(void)
{
	return miliseconds;
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
