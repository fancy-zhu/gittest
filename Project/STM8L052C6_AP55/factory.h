/**
  ******************************************************************************
  * @file    factory.h
  * @author  kk
  * @date    23-March-2020
  * @brief   factory test header file
  ******************************************************************************
  * @attention
  *
  * COPYRIGHT 2019 WISEALLY
  ******************************************************************************
  */ 
#ifndef _FACTORY_H_
#define _FACTORY_H_

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define FIRMWARE_VER_L 21 
#define FIRMWARE_VER_H 0
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void factory_func(void);
void factory_on_solenoid(void);
void factory_off_solenoid(void);


#endif

/********************** (C) COPYRIGHT WISEALLY ******************END OF FILE****/