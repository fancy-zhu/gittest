/**
  ******************************************************************************
  * @file    initial.h
  * @author  a34245
  * @version V0.0.1
  * @date    20-September-2019
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * COPYRIGHT 2019 WISEALLY
  *
  * 
  * 
  * 
  *
  *        
  *
  * 
  * 
  * 
  * 
  * 
  *
  ******************************************************************************
  */ 
#ifndef _INITIAL_H_
#define _INITIAL_H_

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"
#include "stm8l15x_usart.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum
{
  COM1 = 0
} COM_TypeDef;
/* Private define ------------------------------------------------------------*/
#define COMn                        1
#define EVAL_COM1                   USART1
#define EVAL_COM1_GPIO              GPIOC
#define EVAL_COM1_CLK               CLK_Peripheral_USART1
#define EVAL_COM1_RxPin             GPIO_Pin_2
#define EVAL_COM1_TxPin             GPIO_Pin_3
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void SystemClockInitial(void);
void SystemHwRtcInitial(uint8_t clkdiv);
void SystemHwgpioInitial(void);
void SystemHwadcInitial(void);
void SystemHwpwmInitial(void);
void SysteDeviceInitial(void);
void SoftwareDelay(__IO uint32_t nCount);
void SystemTimer2Initial(void);
void SystemUartInitial(void);

#endif

/********************** (C) COPYRIGHT WISEALLY ******************END OF FILE****/