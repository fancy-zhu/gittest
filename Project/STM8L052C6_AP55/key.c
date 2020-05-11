/**
  ******************************************************************************
  * @file    key.c
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
	
/* Includes ------------------------------------------------------------------*/
#include "key.h"
#include "app.h"
#include "stm8l15x_gpio.h"

/* Private function prototypes -----------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
key_packet key;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  KeyMatrixScan
  * @param  None
  * @retval None
  */
void KeyMatrixScan(void)
{
	uint8_t colx;
	key.cur = NONEKEY;
	
	/* Key Matrix Scan */
	for(colx=1; colx<=COL; colx++)
	{
		switch(colx)
		{
			case COLUMN1:
				GPIO_WriteBit(GPIOB,GPIO_Pin_1,RESET);
				break;
			case COLUMN2:
				GPIO_WriteBit(GPIOB,GPIO_Pin_2,RESET);
				break;
			case COLUMN3:
				GPIO_WriteBit(GPIOB,GPIO_Pin_3,RESET);
				break;
			case COLUMN4:
				GPIO_WriteBit(GPIOB,GPIO_Pin_4,RESET);
				break;
			case COLUMN5:
				GPIO_WriteBit(GPIOB,GPIO_Pin_5,RESET);
				break;
			case COLUMN6:
				GPIO_WriteBit(GPIOB,GPIO_Pin_6,RESET);
				break;
			case COLUMN7:
				GPIO_WriteBit(GPIOB,GPIO_Pin_7,RESET);
				break;
			case COLUMN8:
				GPIO_WriteBit(GPIOD,GPIO_Pin_4,RESET);
				break;
			case COLUMN9:
				GPIO_WriteBit(GPIOD,GPIO_Pin_5,RESET);
				break;
			default:
				break;
		}
		
		if(colx <= COL)
		{
			if(!GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_6))
			{ 
				key.cur = ((colx<<4) | ROW0);
				break;
			}
			if(!GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_7))
			{
				key.cur = ((colx<<4) | ROW1);
				break;
			}
		}
	}

	/* key if is pressed */
	if(key.cur != key.pre)
	{
		if(key.cur!=NONEKEY)
		{
			key.sta = PRESSED;
			key.pre = key.cur;
		}
		if(key.cur==NONEKEY)
		{
			key.sta = RELEASE;
			key.pre = key.cur;
		}
	}
	/* Key if is released */
	else if(key.cur == NONEKEY)
	{
		key.pre = NONEKEY;
		key.sta = NONEKEY;
	}
		
	/* Key if is long pressed */	
	else if( key.tim >= DWNTIM)
	{
		key.tim = 0;
		key.sta = LONGPRS;
		event.keydwn = true;
	}
	
	/* Key return initial status */
	GPIO_WriteBit(GPIOB,GPIO_Pin_1,SET);
	GPIO_WriteBit(GPIOB,GPIO_Pin_2,SET);
	GPIO_WriteBit(GPIOB,GPIO_Pin_3,SET);
	GPIO_WriteBit(GPIOB,GPIO_Pin_4,SET);
	GPIO_WriteBit(GPIOB,GPIO_Pin_5,SET);
	GPIO_WriteBit(GPIOB,GPIO_Pin_6,SET);
	GPIO_WriteBit(GPIOB,GPIO_Pin_7,SET);
	GPIO_WriteBit(GPIOD,GPIO_Pin_4,SET);
	GPIO_WriteBit(GPIOD,GPIO_Pin_5,SET);
}



/********************** (C) COPYRIGHT WISEALLY ******************END OF FILE****/