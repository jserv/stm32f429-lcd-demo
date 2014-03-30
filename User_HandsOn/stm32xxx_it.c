/**
  ******************************************************************************
  * @file    stm32xxx_it.c
  * @author  MCD Application Team
  * @version V1.1.0RC1
  * @date    22-July-2013
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
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
#include "global_includes.h"
#include "main.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

extern void LTDC_ISR_Handler(void);
extern void DMA2D_ISR_Handler(void);
extern void LTDC_ISR_simpleHandler(void);


#ifdef USE_MB1063
extern __IO uint32_t TS_Pressed;

#endif
extern __IO uint8_t open;
extern __IO uint8_t openTimer;
extern __IO int32_t OS_TimeMS;
extern __IO uint32_t tamperPushed;

static volatile uint32_t touch_timer=0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*             Cortex-M Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {}
}



/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */


void SysTick_Handler(void)
{

  BSP_Background();
  openTimer++;
  if(openTimer>=25)
  {
    openTimer=0;
    open = 1;
  }




}

void EXTI0_IRQHandler(void)
{   
  EXTI_ClearITPendingBit(USER_BUTTON_EXTI_LINE);
	tamperPushed = 1;
}



#ifdef USE_MB1063
/**
  * @brief  This function handles External line 2 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI9_5_IRQHandler(void)
{
  __IO uint16_t tmpsr = 0;
  
  if(EXTI_GetITStatus(IOE16_IT_EXTI_LINE) != RESET)
  {
    tmpsr = IOE16_GetITStatus();
    
    if((tmpsr & IOE16_TS_IT) != 0 ) 
    {
     
      TS_Pressed = 1;
      
      IOE16_GetITStatus();
      EXTI_ClearITPendingBit(IOE16_IT_EXTI_LINE);
    }
  }
}
#endif

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{


}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{}



/**
  * @brief  This function handles LTDC global interrupt request.
  * @param  None
  * @retval None
  */
void LTDC_IRQHandler(void)
{
	if (emWinRunning())
	{
		LTDC_ISR_Handler();
	}
	else
	{
		LTDC_ISR_simpleHandler();
	}
}

/**
  * @brief  This function handles DMA2D global interrupt request.
  * @param  None
  * @retval None
  */
void DMA2D_IRQHandler(void)
{
  DMA2D_ISR_Handler();
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
