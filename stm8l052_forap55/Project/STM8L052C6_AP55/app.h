/**
  ******************************************************************************
  * @file    app.h
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
#ifndef _APP_H_
#define _APP_H_

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"
#include "key.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum
{
	OPERATON = 0x40,
	DBK1MIN5 = 0x31,
	DBK1MIN15 = 0x21,
	DBK1MIN30 = 0x11,
	DBK1MIN60 = 0x90,
	DBK2MIN5 = 0x80,
	DBK2MIN15 = 0x70,
	DBK2MIN30 = 0x60,
	DBK2MIN60 = 0x50,
	OPERATOFF = 0x30,
	SET1MIN5 = 0x20,
	SET1MIN15 = 0x10,
	SET1MIN30 = 0x91,
	SET1MIN60 = 0x81,
	SET2MIN5 = 0x71,
	SET2MIN15 = 0x61,
	SET2MIN30 = 0x51,
	SET2MIN60 = 0x41
}oprtmode;

typedef enum
{
	OPEN = 0,
	STOP = 0,
	NONE = 0,
	MINS5	= 300,
	MINS15 = 900,
	MINS30 = 1800,
	MINS60 = 3600,
	DAYS1 = 86400,
	DAYS2 = 172800,
	HOUSE22 = 79200,
	HOUSE46 = 165600,
	KEYSELONTIM = 80000,
	KEYSELOFFTM = 40000,
	LBTLEDONTIM = 20000,
	LBTLEDOFFTM = 800000,
	COILTIME = 4000,
	OFF2TIM = 560000,
	FSTCLKTIM = 400000,
	CHAGTIME = 400000,
	SLPTIME = 800000,
	CHRGNO = 0,
	NORMALMODE = 0,
	PCBATESTMD = 1,
	FASTCLKMOD = 2
}timeset;

typedef struct
{
	uint32_t timset:1;
	uint32_t opnwat:1;
	uint32_t timout:1;
	uint32_t fstout:1;
	uint32_t pulses:1;
	uint32_t lowbat:1;
	uint32_t lbd2d3:1;
	uint32_t powron:1;
	uint32_t turnon:1;
	uint32_t turoff:1;
	uint32_t timlag:1;
	uint32_t keydwn:1;
	uint32_t keyset:1;
	uint32_t dayngt:1;
	uint32_t rtcwkp:1;
	uint32_t daybrk:1;
	uint32_t posopn:1;
	uint32_t posoff:1;
	uint32_t learnd:1;
	uint32_t stschg:1;
}event_packet;

typedef struct
{
	uint8_t dutyset;
	uint8_t periods;
	uint8_t nlgtchk;
	uint8_t detcngt;
	uint8_t detcday;
	uint8_t offcunt;
	uint8_t fstchkn;
	uint8_t workmod;
	uint16_t battadc;
	uint16_t captadc;
	uint16_t ligtadc;
	uint16_t pwoncnt;
	uint16_t coilcnt;
	uint16_t opencnt;
	uint32_t fstclkcnt;
	uint32_t rledcount;
	uint32_t gledcount;
	uint32_t pulse2cnt;
	uint32_t chargtime;
	uint32_t slpentcnt;
	uint32_t opnwattim;
	uint32_t opntimset;
}apply_packet;

/* Private define ------------------------------------------------------------*/
#define BATLOWTHRD	2400
#define MINLBDTHRD	2300
#define CHGCAPTHRD 	1200
#define LGTSENTHRD 	330
#define DAYBREAK		1
#define EVENING 		0
#define PWMDUTY			160
#define PERIODS			199
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

extern event_packet event;
extern apply_packet apply;
/* Private function prototypes -----------------------------------------------*/
void WorkModeCheck(void);
void SleepStatusHandle(void);
void StatusMachineHandle(void);
#endif

/********************** (C) COPYRIGHT WISEALLY ******************END OF FILE****/