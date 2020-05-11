/**
  ******************************************************************************
  * @file    key.h
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
#ifndef _KEY_H_
#define _KEY_H_

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum
{
	COLUMN0 = 0,
	COLUMN1,
	COLUMN2,
	COLUMN3,
	COLUMN4,
	COLUMN5,
	COLUMN6,
	COLUMN7,
	COLUMN8,
	COLUMN9
}columnx;

typedef enum
{
	ROW0 = 0,
	ROW1
}rowx;

typedef enum
{
	NONEKEY = 0,
	PRESSED,
	RELEASE,
	LONGPRS
}key_status;

typedef struct
{
	uint8_t len;
	uint8_t sta;
	uint8_t cur;
	uint8_t pre;
	uint8_t pos;
	uint8_t fst;
	uint32_t tim;
}key_packet;

/* Private define ------------------------------------------------------------*/
#define ROW 2
#define COL 9
#define DWNTIM	240000 //12.5us*240000 = 3s
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern key_packet key;
/* Private function prototypes -----------------------------------------------*/
void KeyMatrixScan(void);

#endif

/********************** (C) COPYRIGHT WISEALLY ******************END OF FILE****/