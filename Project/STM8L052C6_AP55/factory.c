/**
  ******************************************************************************
  * @file    factory.c
  * @author  kk
  * @date    23-March-2020
  * @brief   factory test program body
  ******************************************************************************
  * @attention
  *
  * COPYRIGHT 2019 WISEALLY
  *
  ******************************************************************************
  */
	
/* Includes ------------------------------------------------------------------*/
#include "factory.h"
#include "app.h"
#include "init.h"
#include "stm8l15x_gpio.h"
#include "stdio.h"

void BatteryLevelCheck(void);
void CaptorLevelCheck(void);

/* Private function prototypes -----------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define KEY_GPIO_PORTI  GPIOD
#define KEY_GPIO_PINSI  GPIO_Pin_6 | GPIO_Pin_7
#define KEY_GPIO_PORTO1 GPIOD
#define KEY_GPIO_PINSO1 GPIO_Pin_4 | GPIO_Pin_5
#define KEY_GPIO_PORTO2 GPIOB
#define KEY_GPIO_PINSO2 GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 |\
											  GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7


#define FACTORY_INIT 0
#define FACTORY_ON_LED 1
#define FACTORY_SWITCH_CHECK 2

#define FACTORY_SCAN_1TO9_NO_KEY 0
#define FACTORY_SCAN_1TO9_KEY 1
#define FACTORY_SCAN_10TO18_NO_KEY 2
#define FACTORY_SCAN_10TO18_KEY 3
#define FACTORY_SCAN_KEY_FINISH 4
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
const uint8_t FACTORY_KEY_COL_PIN[]={GPIO_Pin_1,GPIO_Pin_2,GPIO_Pin_3,GPIO_Pin_4,GPIO_Pin_5,GPIO_Pin_6,GPIO_Pin_7,GPIO_Pin_4,GPIO_Pin_5};
const uint16_t FACTORY_KEY_COL_PORT[]={GPIOB_BASE,GPIOB_BASE,GPIOB_BASE,GPIOB_BASE,GPIOB_BASE,GPIOB_BASE,GPIOB_BASE,GPIOD_BASE,GPIOD_BASE};
	
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void factory_putchar(uint8_t c)
{
  /* Write a character to the USART */
  USART_SendData8(USART1, c);
  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
}

void factory_photo_sensor(void)
{
	  uint16_t i,sensor_voltage = 0;
	
	  GPIO_WriteBit(GPIOA,GPIO_Pin_6,RESET);
		//GPIO_WriteBit(GPIOA,GPIO_Pin_6,SET);
		SoftwareDelay(100);
		
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
		printf("sensor voltage=%d\n",sensor_voltage);
}

uint16_t factory_captors_sensor(void)
{
	  uint16_t i,captors_voltage=0;
	
  	ADC_ChannelCmd(ADC1, ADC_Channel_4, ENABLE);
		ADC_SoftwareStartConv(ADC1);
		for(i=0; i<5000; i++);
		while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==0);
		ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
		apply.captadc = ADC_GetConversionValue(ADC1);
		ADC_ChannelCmd(ADC1, ADC_Channel_4, DISABLE);
		//printf("captors adc=%d\r\n",apply.captadc);
		captors_voltage = 1225*(apply.captadc/apply.battadc)+((uint32_t)(apply.captadc%apply.battadc)*1225)/apply.battadc;
		//printf("captors voltage=%d\r\n",captors_voltage);
		return captors_voltage;
}



uint8_t tm_key_check(void)
{
	BitStatus tm_key1,tm_key2;
	
	do{
	tm_key1 = GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_0);	
	SoftwareDelay(10);
	tm_key2 = GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_0);
  }while(tm_key1!=tm_key2);
	
	if(tm_key1!=0){
		return 1;
	}
	return 0;	
}

void factory_on_solenoid(void)
{
  uint16_t chargetime,captors_voltage = 0;

	TIM2_CtrlPWMOutputs(ENABLE);
	GPIO_WriteBit(GPIOE,GPIO_Pin_5,SET);
	BatteryLevelCheck();
	captors_voltage=factory_captors_sensor();
	chargetime=0;
	while(captors_voltage<CHGCAPTHRD&&chargetime<200){
		SoftwareDelay(500);
		chargetime++;
		BatteryLevelCheck();
		captors_voltage=factory_captors_sensor();
	}

	TIM2_CtrlPWMOutputs(DISABLE);
	GPIO_WriteBit(GPIOE,GPIO_Pin_5,RESET);
	GPIO_WriteBit(GPIOB,GPIO_Pin_0,RESET);
	/* Ready open the valve */
	GPIO_WriteBit(GPIOC,GPIO_Pin_7,SET);
	GPIO_WriteBit(GPIOE,GPIO_Pin_6,RESET);	
	SoftwareDelay(3000);  //~45ms
	GPIO_WriteBit(GPIOC,GPIO_Pin_7,RESET);
	GPIO_WriteBit(GPIOE,GPIO_Pin_6,RESET);			
	
	printf("captors voltage=%d\n",captors_voltage);	
	
}

void factory_off_solenoid(void)
{
  uint16_t chargetime,captors_voltage = 0;

	TIM2_CtrlPWMOutputs(ENABLE);
	GPIO_WriteBit(GPIOE,GPIO_Pin_5,SET);
	BatteryLevelCheck();
	captors_voltage=factory_captors_sensor();
	chargetime=0;
	while(captors_voltage<CHGCAPTHRD&&chargetime<200){
		SoftwareDelay(500);
		chargetime++;
		BatteryLevelCheck();
		captors_voltage=factory_captors_sensor();
	}
		
	TIM2_CtrlPWMOutputs(DISABLE);
	GPIO_WriteBit(GPIOE,GPIO_Pin_5,RESET);
	GPIO_WriteBit(GPIOB,GPIO_Pin_0,RESET);
			/* Ready stop the valve */
	GPIO_WriteBit(GPIOE,GPIO_Pin_6,SET);
	GPIO_WriteBit(GPIOC,GPIO_Pin_7,RESET);
	SoftwareDelay(3000);  //~45ms
	
	GPIO_WriteBit(GPIOC,GPIO_Pin_7,RESET);
	GPIO_WriteBit(GPIOE,GPIO_Pin_6,RESET);		
}


void factory_key_code(void)
{
		/* Parchse key operat mode */
		switch(key.cur)
		{
			case OPERATON:		//open water
			  printf("SWITCH = 6 (ON)\n");
				//apply.opnwattim = MINS60;
				break;
			case OPERATOFF:		//stop water
			  printf("SWITCH = 7 (OFF)\n");
				break;
			case DBK1MIN5:		//24*3600,5*60
			  printf("SWITCH = 16 (DBK 5)\n");
				break;
			case DBK1MIN15:		//24*3600,10*60
			  printf("SWITCH = 17 (DBK 15)\n");
				break;
			case DBK1MIN30:		//24*3600,30*60
			  printf("SWITCH = 18 (DBK 30)\n");
				break;
			case DBK1MIN60:		//24*3600,60*60
			  printf("SWITCH = 1 (DBK 60)\n");
				break;
			case DBK2MIN5:		//48*3600,5*60
			  printf("SWITCH = 2 (DBK 2nd 5)\n");
				break;	
			case DBK2MIN15:		//48*3600,10*60
			  printf("SWITCH = 3 (DBK 2nd 15)\n");
				break;	
			case DBK2MIN30:		//48*3600,30*60
			  printf("SWITCH = 4 (DBK 2nd 30)\n");			
				break;
			case DBK2MIN60:		//48*3600,60*60
			  printf("SWITCH = 5 (DBK 2nd 60)\n");			
				break;
			case SET1MIN5:		//24*3600,5*60
			  printf("SWITCH = 8 (Daily 5)\n");			
				break;
			case SET1MIN15:		//24*3600,10*60
				printf("SWITCH = 9 (Daily 15)\n");			
				break;
			case SET1MIN30:		//24*3600,30*60
			  printf("SWITCH = 10 (Daily 30)\n");			
				break;	
			case SET1MIN60:		//24*3600,60*60
			  printf("SWITCH = 11 (Daily 60)\n");
				break;
			case SET2MIN5:		//48*3600,5*60
			  printf("SWITCH = 12 (Daily 2nd 5)\n");
				break;
			case SET2MIN15:		//48*3600,10*60
			  printf("SWITCH = 13 (Daily 2nd 15)\n");
				break;
			case SET2MIN30:		//48*3600,30*60
			  printf("SWITCH = 14 (Daily 2nd 30)\n");
				break;
			case SET2MIN60:		//48*3600,60*60
				printf("SWITCH = 15 (Daily 2nd 60)\n");			
				break;			
			default:
			  printf("NO SWITCH\n");			
				break;
		}
}

void factory_sleep(void)
{
	/* Enable usart1 port */
	USART_Cmd(USART1,DISABLE);
	
	while(1)
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

uint16_t factory_key_scan(void)
{
	uint16_t key,flag;
	uint8_t i;
		
	for(key=0,flag=0x01,i=0;i<9;i++){
	  if(GPIO_ReadInputDataBit((GPIO_TypeDef *)FACTORY_KEY_COL_PORT[i],FACTORY_KEY_COL_PIN[i])!=RESET){
			key|=flag;
		}
		flag<<=1;
  }
	return key;
}

/**
  * @brief  factory_func
  * @param  None
  * @retval None
  */
void factory_func(void)
{
	uint8_t count;	
	uint8_t scan_key_step;
	uint16_t cur_key,last_key,battery_voltage;
		

	if(tm_key_check()!=0){
		return;
	}
	
	/* Key port inport pin configure */
	GPIO_Init(KEY_GPIO_PORTI,KEY_GPIO_PINSI,GPIO_Mode_Out_OD_HiZ_Fast);
	
	/* Key port output pin configure */
	GPIO_Init(KEY_GPIO_PORTO1,KEY_GPIO_PINSO1,GPIO_Mode_In_PU_No_IT);
	GPIO_Init(KEY_GPIO_PORTO2,KEY_GPIO_PINSO2,GPIO_Mode_In_PU_No_IT);
	
	printf("AP55 FIRMWARE VERSION = %d.%d \n",FIRMWARE_VER_H,FIRMWARE_VER_L);
	
	//turn on green and red led
  printf("TURN ON RED LED \n");
	GPIO_WriteBit(GPIOD,GPIO_Pin_1,SET);
	GPIO_WriteBit(GPIOD,GPIO_Pin_2,RESET);	
	
	//check battery voltage
	BatteryLevelCheck();
	
	//check photo sensor
	factory_photo_sensor();
	
	//turn on solenoid
	printf("open solenoid \n");
	factory_on_solenoid();
	
  //key scan
	KeyMatrixScan();	
	count = 2;
	last_key = key.cur;
		
	//GPIO_Pin_6 = low
	scan_key_step = FACTORY_SCAN_1TO9_NO_KEY;
	GPIO_WriteBit(GPIOD,GPIO_Pin_6,RESET);
	GPIO_WriteBit(GPIOD,GPIO_Pin_7,SET);
	
	printf("CHECK PIN 1 TO PIN 9 NO CONNECT \n");
	
	while(tm_key_check()==0){		
  	SoftwareDelay(10000);
    cur_key=factory_key_scan();		
		
		if(cur_key != last_key){
			last_key= cur_key;
			count = 2;
		}else if(count!=0){
			count--;
			if(count==0){
				switch(scan_key_step){
					case FACTORY_SCAN_1TO9_NO_KEY:	
            if(cur_key==0x1ff){
							printf("KEY CODE = 0x1FF ");
							printf("PIN 1 TO PIN 9 OPEN ARE CHECKED PASS \n ");
							printf("NEXT STEP, SHORT PIN 1 TO 9 WITH PIN 19\n ");
							scan_key_step = FACTORY_SCAN_1TO9_KEY;
						}else{
							printf("KEY CODE = 0x0%X ",cur_key);
							printf("PIN 1 TO PIN 9 OPEN ARE CHECKED FAIL \n ");							
						}						
					break;
					case FACTORY_SCAN_1TO9_KEY:
					  if(cur_key==0){
							printf("KEY CODE = 0x000 ");
							printf("PIN 1 TO PIN 9 SHORT ARE CHECKED PASS \n ");
							GPIO_WriteBit(GPIOD,GPIO_Pin_6,SET);
							GPIO_WriteBit(GPIOD,GPIO_Pin_7,RESET);							
							scan_key_step = FACTORY_SCAN_10TO18_NO_KEY;							
							printf("TURN ON GREEN LED \n");
							GPIO_WriteBit(GPIOD,GPIO_Pin_1,RESET);
							GPIO_WriteBit(GPIOD,GPIO_Pin_2,SET);							
						}else{
							printf("KEY CODE = 0x0%X ",cur_key);
							printf("PIN 1 TO PIN 9 SHORT ARE CHECKED FAIL \n ");														
						}
					break;
					case FACTORY_SCAN_10TO18_NO_KEY:
            if(cur_key==0x1ff){
							printf("KEY CODE = 0x1FF ");
							printf("PIN 10 TO PIN 18 OPEN ARE CHECKED PASS \n ");
							printf("NEXT STEP, SHORT PIN 10 TO 18 WITH PIN 20\n ");
							scan_key_step = FACTORY_SCAN_10TO18_KEY;
						}else{
							printf("KEY CODE = 0x0%X ",cur_key);
							printf("PIN 10 TO PIN 18 OPEN ARE CHECKED FAIL \n ");							
						}						
					break;
					case FACTORY_SCAN_10TO18_KEY:
					  if(cur_key==0){
							printf("KEY CODE = 0x000 ");
							printf("PIN 1 TO PIN 9 SHORT ARE CHECKED PASS \n ");
							printf("NEXT STEP, OPEN TM-KEY\n");
							GPIO_WriteBit(GPIOD,GPIO_Pin_6,RESET);
							GPIO_WriteBit(GPIOD,GPIO_Pin_7,RESET);							
							scan_key_step = FACTORY_SCAN_KEY_FINISH;							
						}else{
							printf("KEY CODE = 0x0%X ",cur_key);
							printf("PIN 10 TO PIN 18 SHORT ARE CHECKED FAIL \n ");														
						}					
					break;
					case FACTORY_SCAN_KEY_FINISH:
					  printf("KEY CODE = 0x0%X ",cur_key);
					break;
				}
			}
		}
	}
	
	printf("turn off green and red led \n");
	GPIO_WriteBit(GPIOD,GPIO_Pin_1,SET);
	GPIO_WriteBit(GPIOD,GPIO_Pin_2,SET);	
	
	printf("close solenoid \n");	
	factory_off_solenoid();	
	printf("goto  sleep, current < 10uA \n");	

	/* Key port inport pin configure */
	EXTI_SetPinSensitivity(EXTI_Pin_6,EXTI_Trigger_Falling);
	EXTI_SetPinSensitivity(EXTI_Pin_7,EXTI_Trigger_Falling);
	GPIO_Init(KEY_GPIO_PORTI,KEY_GPIO_PINSI,GPIO_Mode_In_FL_IT);
	
	/* Key port output pin configure */
	GPIO_Init(KEY_GPIO_PORTO1,KEY_GPIO_PINSO1,GPIO_Mode_Out_PP_High_Fast);
	GPIO_Init(KEY_GPIO_PORTO2,KEY_GPIO_PINSO2,GPIO_Mode_Out_PP_High_Fast);
	
	factory_sleep();
	
}



/********************** (C) COPYRIGHT WISEALLY ******************END OF FILE****/