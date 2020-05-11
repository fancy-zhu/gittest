/**
  ******************************************************************************
  * @file    app.c
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
#include "app.h"
#include "init.h"
#include "stdio.h"

/* Private function prototypes -----------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
event_packet event;
apply_packet apply;

/* Private function prototypes -----------------------------------------------*/
void WorkModeCheck(void);
void SleepStatusHandle(void);
void StatusMachineHandle(void);
void LedSatusIndicate(void);
void BatteryLevelCheck(void);
void CaptorLevelCheck(void);
void CaptorChargeStart(void);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  OperateModeCheck
  * @param  None
  * @retval None
  */
void WorkModeCheck(void)
{
	/* Check if is fast clock mode */
	if(key.fst!=key.cur && key.cur !=0)
	{
		if( key.fst==0x30&&key.cur==0x40)
			apply.fstchkn++;
			
		key.fst = key.cur;
	}
	/* Enter fast clock test mode */
	if(apply.fstchkn==2&&key.pre==0x31)
	{
		apply.workmod = FASTCLKMOD;
		SystemHwRtcInitial( 0x00 );
	}
}

/**
  * @brief  ReloadRtcWakeupCnt
  * @param  None
  * @retval None
  */
void ReloadRtcWakeupCnt(uint32_t time)
{
	uint16_t count;
	
	if(apply.workmod==NORMALMODE)
		count = (uint16_t)(time/4);
		
	if(apply.workmod==FASTCLKMOD)
		count = (uint16_t)(time/60);
	
	RTC_SetWakeUpCounter(count);
}

/**
  * @brief  CaptorChargeStart
  * @param  None
  * @retval None
  */
void CaptorChargeStart(void)
{
	/* Open Captor Charge Power */
	if(event.timset==true)
	{
		TIM2_CtrlPWMOutputs(ENABLE);
		GPIO_WriteBit(GPIOE,GPIO_Pin_5,SET);
	}
	
	/* Stop Captor Charge Power */
	if(event.timset==false)
	{
		TIM2_CtrlPWMOutputs(DISABLE);
		GPIO_WriteBit(GPIOE,GPIO_Pin_5,RESET);
	}
}
/**
  * @brief  SleepStatusHandle
  * @param  None
  * @retval None
  */
void BatteryLevelCheck(void)
{
	uint16_t i,battery_voltage = 0;

	/* Power on low battery detect */
	//GPIO_WriteBit(GPIOE,GPIO_Pin_4,RESET);

	/* Start Check Battery Voltage */
	ADC_ChannelCmd(ADC1, ADC_Channel_Vrefint, ENABLE);
	ADC_VrefintCmd(ENABLE);
	ADC_SoftwareStartConv(ADC1);
	for(i=0; i<5000; i++);
	while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==0);
	ADC_ClearFlag(ADC1,ADC_FLAG_EOC);
	apply.battadc = ADC_GetConversionValue(ADC1);
	ADC_ChannelCmd(ADC1, ADC_Channel_Vrefint, DISABLE);
	ADC_VrefintCmd(DISABLE);
	
	//printf("battery adc=%d\r\n",apply.battadc);
	battery_voltage = (uint16_t)(5017600/apply.battadc);
	printf("battery voltage=%d\r\n",battery_voltage);
	
	/* Low Battery Threshold 2.4V */
	if(event.powron && !event.timset)
	{
		if(battery_voltage<BATLOWTHRD)
			event.lowbat = true;
		else 
			event.lowbat = false;
		
		if(battery_voltage<MINLBDTHRD)
			event.lbd2d3 = true;
		else
			event.lbd2d3 = false;
	}

	/* Power on low battery detect */
	//GPIO_WriteBit(GPIOE,GPIO_Pin_4,SET);
}
/**
  * @brief  SleepStatusHandle
  * @param  None
  * @retval None
  */
void CaptorLevelCheck(void)
{
	uint16_t i,captors_voltage = 0;
	uint32_t opntim = 0,settim = 0;

	if(event.timset == true&&event.pulses == false)
	{	
		ADC_ChannelCmd(ADC1, ADC_Channel_4, ENABLE);
		ADC_SoftwareStartConv(ADC1);
		for(i=0; i<5000; i++);
		while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==0);
		ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
		apply.captadc = ADC_GetConversionValue(ADC1);
		ADC_ChannelCmd(ADC1, ADC_Channel_4, DISABLE);
		//printf("captors adc=%d\r\n",apply.captadc);
		captors_voltage = 1225*(apply.captadc/apply.battadc)+\
		((uint32_t)(apply.captadc%apply.battadc)*1225)/apply.battadc;
		//printf("captors voltage=%d\r\n",captors_voltage);
	}
	/* Open Water Switch Threshold 2.5V */
	if(captors_voltage>CHGCAPTHRD||apply.chargtime>CHAGTIME)
	{
		/* Turn on valve */
		if(event.opnwat==false)
		{
			apply.chargtime = 0;
			event.pulses = true;
			event.opnwat = true;
			event.turnon = true;
			
			/* Ready open the valve */
			GPIO_WriteBit(GPIOC,GPIO_Pin_7,SET);
			GPIO_WriteBit(GPIOE,GPIO_Pin_6,RESET);
			
			/* Reload waitting time */
			RTC_WakeUpCmd(DISABLE);
			opntim = apply.opnwattim;
			ReloadRtcWakeupCnt(opntim);
			RTC_WakeUpCmd(ENABLE);
		}
		
		/* Shut off valve */
		else if(apply.offcunt<=2)
		{
			apply.offcunt++;
			apply.chargtime = 0;
			event.pulses = true;
			
			/* Ready stop the valve */
			GPIO_WriteBit(GPIOE,GPIO_Pin_6,SET);
			GPIO_WriteBit(GPIOC,GPIO_Pin_7,RESET);
			
			/* Second shutoff valve */
			if(apply.offcunt == 2)
			{
				/* Reload interval time */
				RTC_WakeUpCmd(DISABLE);
				if(event.daybrk == false)
				{
					settim = apply.opntimset;
					ReloadRtcWakeupCnt(settim);
				}
				else if( event.learnd == false)
				{
					event.turoff = true;
					ReloadRtcWakeupCnt(MINS30);
				}
				else if(apply.opntimset==DAYS1)
				{
					event.turoff = true;
					event.timlag = true;
					ReloadRtcWakeupCnt(HOUSE22);
				}
				
				else if(apply.opntimset==DAYS2)
				{
					event.turoff = true;
					event.timlag = true;
					ReloadRtcWakeupCnt(HOUSE46);
				}
			
				/* Open or stop no wakeup */ 
				if(!event.posoff&&!event.posopn)
					RTC_WakeUpCmd(ENABLE);
			}
		}
	}
	
	/* 50ms open valve pulse */
	if(apply.coilcnt>=COILTIME&&event.pulses)
	{
		event.timset = false;
		event.pulses = false;
		
		/* Ensure shut off valve */
		if(apply.offcunt == 2)
		{
			apply.offcunt = 0;
			event.powron=true;
			event.opnwat=false;
		}
		
		/* Complete the valve open */
		GPIO_WriteBit(GPIOE,GPIO_Pin_6,RESET);
		GPIO_WriteBit(GPIOC,GPIO_Pin_7,RESET);
	}
}
/**
  * @brief  SleepStatusHandle
  * @param  None
  * @retval None
  */
void LightSensorCheck(void)
{
	uint16_t i,sensor_voltage = 0;
	
	if(event.rtcwkp&&event.daybrk&&!event.opnwat&&!event.turnon)
	{	
		event.rtcwkp = false;
		ADC_ChannelCmd(ADC1, ADC_Channel_2, ENABLE);
		ADC_SoftwareStartConv(ADC1);
		for(i=0; i<5000; i++);
		while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==0);
		ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
		apply.ligtadc = ADC_GetConversionValue(ADC1);
		ADC_ChannelCmd(ADC1, ADC_Channel_2, DISABLE);
		//printf("sensor adc=%d\r\n",apply.ligtadc);
		sensor_voltage = 1225*(apply.ligtadc/apply.battadc)+\
		((uint32_t)(apply.ligtadc%apply.battadc)*1225)/apply.battadc;
		printf("sensor voltage=%d\r\n",sensor_voltage);
		
		if(event.timout&&!event.stschg)
		{
			apply.nlgtchk++;
			RTC_WakeUpCmd(DISABLE);
			ReloadRtcWakeupCnt(MINS30);
			RTC_WakeUpCmd(ENABLE);
		}

		if(sensor_voltage > LGTSENTHRD)
		{
			if(event.keyset == true)
			{
				event.timset = true;
				event.keyset = false;
				event.dayngt = DAYBREAK;
			}
			
			if(event.dayngt == EVENING)
			{	
				event.stschg = true;
				RTC_WakeUpCmd(DISABLE);
				ReloadRtcWakeupCnt(MINS5);
				RTC_WakeUpCmd(ENABLE);
			}//else if(apply.detcday < 4)
				//event.stschg = false;
			
			if(event.stschg == true)
				apply.detcday++;
				
			if(apply.detcday >= 4)
				event.dayngt = DAYBREAK;
		}
		
		if(sensor_voltage<=LGTSENTHRD)
		{
			if(event.keyset == true)
			{
				event.keyset = false;
				event.dayngt = EVENING;
			}
			
			if(event.dayngt == DAYBREAK)
			{
				event.stschg = true;
				RTC_WakeUpCmd(DISABLE);
				ReloadRtcWakeupCnt(MINS5);
				RTC_WakeUpCmd(ENABLE);				
			}//else if(apply.detcngt < 4)
				//event.stschg = false;
			
			if(event.stschg == true)
				apply.detcngt++;
				
			if(apply.detcngt >= 4)
				event.dayngt = EVENING;	
		}
			
		if((apply.detcday+apply.detcngt)==6)			
		{
			if(event.learnd == true)
				apply.nlgtchk++;
				
			else if(event.dayngt==0)
			{
				RTC_WakeUpCmd(DISABLE);
				ReloadRtcWakeupCnt(MINS30);
				RTC_WakeUpCmd(ENABLE);				
			}
			
			else if(apply.detcday>=4)
			{
				if(event.learnd==false)
					event.timset = true;
					
				event.learnd = true;
			}
				
			apply.detcday = 0;
			apply.detcngt = 0;
			event.stschg = false;
		}
		
		if(event.learnd&&apply.nlgtchk>=4)
		{
			apply.nlgtchk = 0;
			event.timout = false;
			
			if(event.dayngt==DAYBREAK)
			{
				apply.gledcount = 0;
				event.timset = true;
			}
		}
		
	}//else GPIO_WriteBit(GPIOA,GPIO_Pin_6,SET);
}
/**
  * @brief  SleepStatusHandle
  * @param  None
  * @retval None
  */
void LedSatusIndicate(void)
{
	/* Check dial if is selected */
	if(key.sta == PRESSED)
	{
		if(apply.gledcount<=KEYSELOFFTM)
			GPIO_WriteBit(GPIOD,GPIO_Pin_1,SET);

		else if(apply.gledcount<=KEYSELONTIM)
			GPIO_WriteBit(GPIOD,GPIO_Pin_1,RESET);
			
		else apply.gledcount = 0;
	}
	/* Dial isnot selected off led*/
	if(key.sta==NONEKEY||apply.gledcount>=LBTLEDOFFTM) 
	{
		apply.gledcount = LBTLEDOFFTM;
		GPIO_WriteBit(GPIOD,GPIO_Pin_1,SET);
	}
	
	/* Dial selected green led 10s */
	if(key.sta==LONGPRS&&apply.gledcount <LBTLEDOFFTM)
		GPIO_WriteBit(GPIOD,GPIO_Pin_1,RESET);
	
	/* Check battery if is low */
	if(event.lowbat==true)
	{
		if(apply.rledcount<=LBTLEDONTIM)
			GPIO_WriteBit(GPIOD,GPIO_Pin_2,RESET);

		else if(apply.rledcount<=LBTLEDOFFTM)
			GPIO_WriteBit(GPIOD,GPIO_Pin_2,SET);

		else apply.rledcount = 0;
		
	}else GPIO_WriteBit(GPIOD,GPIO_Pin_2,SET);
	
	/* Indicate enter fast test mode */
	if(apply.workmod==FASTCLKMOD&&!event.powron)
	{
		GPIO_WriteBit(GPIOD,GPIO_Pin_1,RESET);
		GPIO_WriteBit(GPIOD,GPIO_Pin_2,RESET);
	}
}
/**
  * @brief  SleepStatusHandle
  * @param  None
  * @retval None
  */
void OperatModeProcess(void)
{
	/* Key ensure pressed */
	if(event.keydwn&&!event.timset&&event.powron)
	{
		event.keydwn = false;
		
		/* Parchse key operat mode */
		switch(key.cur)
		{
			case OPERATON:		//open water
				event.timset = true;
				event.posopn = true;
				event.opnwat = false;
				apply.opnwattim = MINS60;
				break;
			case OPERATOFF:		//stop water
				event.posoff = true;
				event.timset = true;
				event.opnwat = true;
				break;
			case DBK1MIN5:		//24*3600,5*60
				event.daybrk = true;
				event.rtcwkp = true;
				event.keyset = true;
				event.learnd = false;
				apply.opnwattim = MINS5;
				apply.opntimset = DAYS1;
				break;
			case DBK1MIN15:		//24*3600,10*60
				event.daybrk = true;
				event.rtcwkp = true;
				event.keyset = true;
				event.learnd = false;
				apply.opnwattim = MINS15;
				apply.opntimset = DAYS1;
				break;
			case DBK1MIN30:		//24*3600,30*60
				event.daybrk = true;
				event.rtcwkp = true;
				event.keyset = true;
				event.learnd = false;
				apply.opnwattim = MINS30;
				apply.opntimset = DAYS1;
				break;
			case DBK1MIN60:		//24*3600,60*60
				event.daybrk = true;
				event.rtcwkp = true;
				event.keyset = true;
				event.learnd = false;
				apply.opnwattim = MINS60;
				apply.opntimset = DAYS1;
				break;
			case DBK2MIN5:		//48*3600,5*60
				event.daybrk = true;
				event.rtcwkp = true;
				event.keyset = true;
				event.learnd = false;
				apply.opnwattim = MINS5;
				apply.opntimset = DAYS2;
				break;	
			case DBK2MIN15:		//48*3600,10*60
				event.daybrk = true;
				event.rtcwkp = true;
				event.keyset = true;
				event.learnd = false;
				apply.opnwattim = MINS15;
				apply.opntimset = DAYS2;
				break;	
			case DBK2MIN30:		//48*3600,30*60
				event.daybrk = true;
				event.rtcwkp = true;
				event.keyset = true;
				event.learnd = false;
				apply.opnwattim = MINS30;
				apply.opntimset = DAYS2;
				break;
			case DBK2MIN60:		//48*3600,60*60
				event.daybrk = true;
				event.rtcwkp = true;
				event.keyset = true;
				event.learnd = false;
				apply.opnwattim = MINS60;
				apply.opntimset = DAYS2;
				break;
			case SET1MIN5:		//24*3600,5*60
				event.opnwat = false;
				event.daybrk = false;
				apply.opnwattim = MINS5;
				apply.opntimset = DAYS1;
				break;
			case SET1MIN15:		//24*3600,10*60
				event.opnwat = false;
				event.daybrk = false;
				apply.opnwattim = MINS15;
				apply.opntimset = DAYS1;
				break;
			case SET1MIN30:		//24*3600,30*60
				event.opnwat = false;
				event.daybrk = false;
				apply.opnwattim = MINS30;
				apply.opntimset = DAYS1;
				break;	
			case SET1MIN60:		//24*3600,60*60
				event.opnwat = false;
				event.daybrk = false;
				apply.opnwattim = MINS60;
				apply.opntimset = DAYS1;
				break;
			case SET2MIN5:		//48*3600,5*60
				event.opnwat = false;
				event.daybrk = false;
				apply.opnwattim = MINS5;
				apply.opntimset = DAYS2;
				break;
			case SET2MIN15:		//48*3600,10*60
				event.opnwat = false;
				event.daybrk = false;
				apply.opnwattim = MINS15;
				apply.opntimset = DAYS2;
				break;
			case SET2MIN30:		//48*3600,30*60
				event.opnwat = false;
				event.daybrk = false;
				apply.opnwattim = MINS30;
				apply.opntimset = DAYS2;
				break;
			case SET2MIN60:		//48*3600,60*60
				event.opnwat = false;
				event.daybrk = false;
				apply.opnwattim = MINS60;
				apply.opntimset = DAYS2;
				break;			
			default:
				break;
		}
		if(event.daybrk==false)
			event.timset = true;	
			
		if(key.cur != OPERATON)
			event.posopn = false;
		
		if(key.cur!= OPERATOFF)
		{	
			event.posoff = false;
			if(event.lbd2d3==true)
			{
				event.timset = false;
				event.rtcwkp = false;
			}
		}
	}
}
/**
  * @brief  SleepStatusHandle
  * @param  None
  * @retval None
  */
void StatusMachineHandle(void)
{
	/* Starting Captor Charge */
	CaptorChargeStart();
	
	/* Battery power level Check */
	BatteryLevelCheck();
	
	/* Captor charge level Check */
	CaptorLevelCheck();
	
	/* Light sensor level check */
	LightSensorCheck();
	
	/* Operate mode event process */
	OperatModeProcess();
	
	/* Process the led indicate */
	LedSatusIndicate();
}

/**
  * @brief  SleepStatusHandle
  * @param  None
  * @retval None
  */
void SleepStatusHandle(void)
{	
	if(apply.slpentcnt >= SLPTIME)
	{
		/* Ready to enter sleep */
		apply.slpentcnt = 0;
		
		/* Disable time2 update and adc */
		TIM2_Cmd(DISABLE);
		ADC_Cmd(ADC1,DISABLE);
		
		/* Close charge power and pwm output */
		GPIO_Init(GPIOA,0xFD,0xF0);
		GPIO_Write(GPIOA,0xED);
		
		/* Output GPIOB Pin1~7 0 for wakeup */
		GPIO_Init(GPIOB,0xFF,0xF0);
		GPIO_Write(GPIOB,0x00);
		
		/* Close valve pin7 and other pin 1 */
		GPIO_Init(GPIOC,0x9F,0xF0);
		GPIO_Write(GPIOC,0x0F);
		
		/* Close led pin1~2 and pin4~5 wakeup */
		GPIO_Init(GPIOD,0x3F,0xF0);
		GPIO_Write(GPIOD,0x0F);
		
		/* Close valve pin6 and other pin 1 */
		GPIO_Init(GPIOE,0xFF,0xF0);
		GPIO_Write(GPIOE,0x9F);
		
		/* Output pin1 to 1 and other pin 0 */
		GPIO_Init(GPIOF,0x01,0xF0);
		GPIO_Write(GPIOF,0x01);
		
		/* Stop mode enter depth sleep */ 
		halt();
	
		/* Restory adc channel pa4 pc4 */
		GPIO_DeInit(GPIOA);
		GPIO_DeInit(GPIOC);
		GPIO_Init(GPIOA,0x40,0xF0);
		GPIO_Init(GPIOC,0x80,0xF0);
		
		/* Key return initial status */
		GPIO_Write(GPIOB,0xFE);
		GPIO_Write(GPIOD,0x3F);
		GPIO_WriteBit(GPIOA,0x40,0);
		GPIO_WriteBit(GPIOC,0x80,0);

		/* Enable time2 update and adc*/
		TIM2_Cmd(ENABLE);
		ADC_Cmd(ADC1,ENABLE);
	}
}


/********************** (C) COPYRIGHT WISEALLY ******************END OF FILE****/