/**
  ******************************************************************************
  * @file    initial.c
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
#include "init.h"
#include "app.h"

/* Private function prototypes -----------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TIM2_PERIOD  (uint8_t) 199
#define TIM2_PULSE   (uint8_t) 160

#define LED_GPIO_PORT  GPIOD
#define LED_GPIO_PINS  GPIO_Pin_1 | GPIO_Pin_2

#define SLD_GPIO_PORT	 GPIOC
#define SLD_GPIO_PINS  GPIO_Pin_2 | GPIO_Pin_3

#define KEY_GPIO_PORTI  GPIOD
#define KEY_GPIO_PINSI  GPIO_Pin_6 | GPIO_Pin_7
#define KEY_GPIO_PORTO1 GPIOD
#define KEY_GPIO_PINSO1 GPIO_Pin_4 | GPIO_Pin_5
#define KEY_GPIO_PORTO2 GPIOB
#define KEY_GPIO_PINSO2 GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 |\
											  GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
RTC_InitTypeDef	RTC_hInit;
RTC_TimeTypeDef	RTC_Time;
RTC_DateTypeDef	RTC_Date;
RTC_AlarmTypeDef RTC_Alarm;

USART_TypeDef* COM_USART[COMn] = {EVAL_COM1};

GPIO_TypeDef* COM_PORT[COMn] = {EVAL_COM1_GPIO};

const uint8_t COM_USART_CLK[COMn] = {EVAL_COM1_CLK};

const uint8_t COM_TX_PIN[COMn] = {EVAL_COM1_TxPin};

const uint8_t COM_RX_PIN[COMn] = {EVAL_COM1_RxPin};
/* Private function prototypes -----------------------------------------------*/
void SystemClockInitial(void);
void SystemHwRtcInitial(uint8_t clkdiv);
void SystemHwgpioInitial(void);
void SystemHwadcInitial(void);
void SysteDeviceInitial(void);
void SoftwareDelay(__IO uint32_t nCount);
void SystemTimer2Initial(void);
void SystemUartInitial(void);

/* Private functions ---------------------------------------------------------*/
#if 0
/**
* @brief  Enables or disables the specified peripheral clock.
  * @note   After reset, the peripheral clock (used for registers read/write access)
  *         is disabled and the application software has to enable this clock before 
  *         using it.   
  * @param  CLK_Peripheral: specifies the peripheral to gate its clock.
  *          This parameter can be any combination of the following values:
  *            @arg CLK_Peripheral_TIM2:       TIM2 clock
  *            @arg CLK_Peripheral_TIM3:       TIM3 clock 
  *            @arg CLK_Peripheral_TIM4:       TIM4 clock
  *            @arg CLK_Peripheral_I2C1:       I2C1 clock
  *            @arg CLK_Peripheral_SPI1:       SPI1 clock
  *            @arg CLK_Peripheral_USART1:     USART1 clock
  *            @arg CLK_Peripheral_BEEP:       BEEP clock
  *            @arg CLK_Peripheral_DAC:        DAC clock
  *            @arg CLK_Peripheral_ADC1:       ADC1 clock
  *            @arg CLK_Peripheral_TIM1:       TIM1 clock
  *            @arg CLK_Peripheral_RTC:        RTC clock
  *            @arg CLK_Peripheral_LCD:        LCD clock
  *            @arg CLK_Peripheral_DMA1:       DMA1 clock
  *            @arg CLK_Peripheral_COMP:       COMP clock
  *            @arg CLK_Peripheral_BOOTROM:    BOOTROM clock
  *            @arg CLK_Peripheral_AES:        AES clock
  *            @arg CLK_Peripheral_TIM5:       TIM5 clock
  *            @arg CLK_Peripheral_SPI2:       SPI2 clock
  *            @arg CLK_Peripheral_USART2:     USART2 clock
  *            @arg CLK_Peripheral_USART3:     USART3 clock
  *            @arg CLK_Peripheral_CSSLSE:     CSS on LSE clock
  * @param  NewState: new state of the specified peripheral clock.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void PeripheralClockConfig(CLK_Peripheral_TypeDef CLK_Peripheral, FunctionalState NewState)
{
  uint8_t reg = 0;

  /* Check the parameters */
  assert_param(IS_CLK_PERIPHERAL(CLK_Peripheral));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  /* get flag register */
  reg = (uint8_t)((uint8_t)CLK_Peripheral & (uint8_t)0xF0);

  if ( reg == 0x00)
  {
    if (NewState != DISABLE)
    {
      /* Enable the peripheral Clock */
      CLK->PCKENR1 |= (uint8_t)((uint8_t)1 << ((uint8_t)CLK_Peripheral & (uint8_t)0x0F));
    }
    else
    {
      /* Disable the peripheral Clock */
      CLK->PCKENR1 &= (uint8_t)(~(uint8_t)(((uint8_t)1 << ((uint8_t)CLK_Peripheral & (uint8_t)0x0F))));
    }
  }
  else if (reg == 0x10)
  {
    if (NewState != DISABLE)
    {
      /* Enable the peripheral Clock */
      CLK->PCKENR2 |= (uint8_t)((uint8_t)1 << ((uint8_t)CLK_Peripheral & (uint8_t)0x0F));
    }
    else
    {
      /* Disable the peripheral Clock */
      CLK->PCKENR2 &= (uint8_t)(~(uint8_t)(((uint8_t)1 << ((uint8_t)CLK_Peripheral & (uint8_t)0x0F))));
    }
  }
  else
  {
    if (NewState != DISABLE)
    {
      /* Enable the peripheral Clock */
      CLK->PCKENR3 |= (uint8_t)((uint8_t)1 << ((uint8_t)CLK_Peripheral & (uint8_t)0x0F));
    }
    else
    {
      /* Disable the peripheral Clock */
      CLK->PCKENR3 &= (uint8_t)(~(uint8_t)(((uint8_t)1 << ((uint8_t)CLK_Peripheral & (uint8_t)0x0F))));
    }
  }
}
/**
  * @brief  Configures the RTC clock (RTCCLK).
  * @param  CLK_RTCCLKSource: specifies the RTC clock source.
  *          This parameter can be one of the following values:
  *            @arg CLK_RTCCLKSource_Off: RTC clock Off  
  *            @arg CLK_RTCCLKSource_LSE: LSE selected as RTC clock
  *            @arg CLK_RTCCLKSource_LSI: LSI selected as RTC clock
  *            @arg CLK_RTCCLKSource_HSE: HSE selected as RTC clock
  *            @arg CLK_RTCCLKSource_HSI: HSI selected as RTC clock  
  *  
  * @param  CLK_RTCCLKDiv: specifies the RTC clock source divider.
  *          This parameter can be one of the following values:
  *            @arg CLK_RTCCLKDiv_1: Clock RTC Div 1  
  *            @arg CLK_RTCCLKDiv_2: Clock RTC Div 2
  *            @arg CLK_RTCCLKDiv_4: Clock RTC Div 4
  *            @arg CLK_RTCCLKDiv_8: Clock RTC Div 8
  *            @arg CLK_RTCCLKDiv_16: Clock RTC Div 16
  *            @arg CLK_RTCCLKDiv_32: Clock RTC Div 32
  *            @arg CLK_RTCCLKDiv_64: Clock RTC Div 64
  *  
  * @note   If the LSE or LSI is used as RTC clock source, the RTC continues to
  *         work in HALT and Active HALT modes, and can be used as wakeup source.
  *         However, when the HSE clock is used as RTC clock source.    
  * @note   The maximum input clock frequency for RTC is 1MHz (when using HSE/HSI as
  *         RTC clock source).
  *  
  * @retval None
  */
void RTCClockConfig(CLK_RTCCLKSource_TypeDef CLK_RTCCLKSource, CLK_RTCCLKDiv_TypeDef CLK_RTCCLKDiv)
{
  /* check the parameters */
  assert_param(IS_CLK_CLOCK_RTC(CLK_RTCCLKSource));
  assert_param(IS_CLK_CLOCK_RTC_DIV(CLK_RTCCLKDiv));

  /* Selects the source provided on to RTC and its divider*/
  CLK->CRTCR = (uint8_t)((uint8_t)CLK_RTCCLKSource | (uint8_t)CLK_RTCCLKDiv);
}

 /**
  * @brief  Enables or disables the specified CLK interrupts.
  * @param  CLK_IT: specifies the CLK interrupt sources to be enabled or disabled.
  *          This parameter can be any combination of the following values:
  *            @arg CLK_IT_CSSD: Clock security system detection interrupt
  *            @arg CLK_IT_SWIF: Clock switch interrupt 
  *            @arg CLK_IT_LSECSSF: LSE Clock security system detection interrupt
  * @param  NewState: new state of the specified CLK interrupts.
  *          This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void InterruptClockConfig(CLK_IT_TypeDef CLK_IT, FunctionalState NewState)
{

  /* check the parameters */
  assert_param(IS_CLK_IT(CLK_IT));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    if (CLK_IT == CLK_IT_SWIF)
    {
      /* Enable the clock switch interrupt */
      CLK->SWCR |= CLK_SWCR_SWIEN;
    }
    else if (CLK_IT == CLK_IT_LSECSSF)
    {
      /* Enable the CSS on LSE  interrupt */
      CSSLSE->CSR |= CSSLSE_CSR_CSSIE;
    }
    else
    {
      /* Enable the clock security system detection interrupt */
      CLK->CSSR |= CLK_CSSR_CSSDIE;
    }
  }
  else  /*(NewState == DISABLE)*/
  {
    if (CLK_IT == CLK_IT_SWIF)
		{
      /* Disable the clock switch interrupt */
      CLK->SWCR  &= (uint8_t)(~CLK_SWCR_SWIEN);
    }
    else if (CLK_IT == CLK_IT_LSECSSF)
    {
      /* Disable the CSS on LSE  interrupt */
      CSSLSE->CSR &= (uint8_t)(~CSSLSE_CSR_CSSIE);
    }
    else
    {
      /* Disable the clock security system detection interrupt */
      CLK->CSSR &= (uint8_t)(~CLK_CSSR_CSSDIE);
    }
  }
}
/**
  * @brief  Configures clock during halt and active halt modes.
  * @param  CLK_Halt : Specifies the clock state and wake-up mode from halt way.
  *         This parameter can be a value of @ref CLK_Halt_TypeDef.
  * @param  NewState : Specifies the System clock (SYSCLK) state in active halt mode.
  *         This parameter can be ENABLE or DISABLE.
  * @retval None
  */
void HaltClockConfig(CLK_Halt_TypeDef CLK_Halt, FunctionalState NewState)
{
  /* check the parameters */
  assert_param(IS_CLK_HALT(CLK_Halt));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    CLK->ICKCR |= (uint8_t)(CLK_Halt);
  }
  else
  {
    CLK->ICKCR &= (uint8_t)(~CLK_Halt);
  }
}

/**
  * @brief  Configures the System clock (SYSCLK) dividers.
  * @param  CLK_SYSCLKDiv : Specifies the system clock divider to apply.
  *          This parameter can be one of the following values:
  *            @arg CLK_SYSCLKDiv_1 
  *            @arg CLK_SYSCLKDiv_2
  *            @arg CLK_SYSCLKDiv_4
  *            @arg CLK_SYSCLKDiv_8
  *            @arg CLK_SYSCLKDiv_16
  *            @arg CLK_SYSCLKDiv_64
  *            @arg CLK_SYSCLKDiv_128
  * @retval None
  */
void SysclkDivideConfig(CLK_SYSCLKDiv_TypeDef CLK_SYSCLKDiv)
{
  /* check the parameters */
  assert_param(IS_CLK_SYSTEM_DIVIDER(CLK_SYSCLKDiv));

  CLK->CKDIVR = (uint8_t)(CLK_SYSCLKDiv);
}
/**
  * @brief  Configures the system clock (SYSCLK).
  * @note   The HSI is used (enabled by hardware) as system clock source after
  *         startup from Reset, wake-up from Halt and active Halt modes, or in case
  *         of failure of the HSE used as system clock (if the Clock Security System CSS is enabled).
  * @note   A switch from one clock source to another occurs only if the target
  *         clock source is ready (clock stable after startup delay or PLL locked). 
  *         You can use CLK_GetSYSCLKSource() function to know which clock is
  *         currently used as system clock source. 
  * @param  CLK_SYSCLKSource: specifies the clock source used as system clock.
  *          This parameter can be one of the following values:
  *            @arg CLK_SYSCLKSource_HSI: HSI selected as system clock source
  *            @arg CLK_SYSCLKSource_HSE: HSE selected as system clock source
  *            @arg CLK_SYSCLKSource_LSI: LSI selected as system clock source
  *            @arg CLK_SYSCLKSource_LSE: LSE selected as system clock source
  * @retval None
  */
void SysclkSourceConfig(CLK_SYSCLKSource_TypeDef CLK_SYSCLKSource)
{
  /* check teh parameters */
  assert_param(IS_CLK_SOURCE(CLK_SYSCLKSource));

  /* Selection of the target clock source */
  CLK->SWR = (uint8_t)CLK_SYSCLKSource;
}
/**
  * @brief  Enables or disables the clock switch execution.
  * @param  NewState : new state of clock switch, value accepted ENABLE, DISABLE.
  * @retval None
  */
void SysclkSourceSwitch(FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    /* Set SWEN bit */
    CLK->SWCR |= CLK_SWCR_SWEN;
  }
  else
  {
    /* Reset SWEN  bit */
    CLK->SWCR &= (uint8_t)(~CLK_SWCR_SWEN);
  }
}

#endif
/**
  * @brief  main clock initial function
  * @param  None
  * @retval None
  */
void SystemClockInitial(void)
{
	/* Select HSI as system clock source */
  CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);

  /* High speed external clock prescaler: 1 */
  CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);
  while (CLK_GetSYSCLKSource() != CLK_SYSCLKSource_HSI);
}
/**
  * @brief  RTC clock initial function
  * @param  None
  * @retval None
  */
void SystemHwRtcInitial(uint8_t clkdiv)
{
	#ifdef TIM_MOD
	
  /* Select LSE (32.768 KHz) as RTC clock source */
  CLK_RTCClockConfig(CLK_RTCCLKSource_LSE, clkdiv);
	
  /* Wait for LSE clock to be ready */
  while (CLK_GetFlagStatus(CLK_FLAG_LSERDY) == RESET);

	/* Enable RTC peripheral */
  CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE);
	
  /* Configures the RTC wakeup timer_step = LSE/16 = 488.28125us */
	RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
	
	RTC_hInit.RTC_HourFormat = RTC_HourFormat_24;
  RTC_hInit.RTC_AsynchPrediv = 0x7F;
  RTC_hInit.RTC_SynchPrediv = 0x00FF;
  RTC_Init(&RTC_hInit);

  RTC_DateStructInit(&RTC_Date);
  RTC_Date.RTC_WeekDay = RTC_Weekday_Friday;
  RTC_Date.RTC_Date = 20;
  RTC_Date.RTC_Month = RTC_Month_May;
  RTC_Date.RTC_Year = 19;
  RTC_SetDate(RTC_Format_BIN, &RTC_Date);

  RTC_TimeStructInit(&RTC_Time);
  RTC_Time.RTC_Hours   = 00;
  RTC_Time.RTC_Minutes = 00;
  RTC_Time.RTC_Seconds = 00;
  RTC_SetTime(RTC_Format_BIN, &RTC_Time);
	/*
  RTC_AlarmStructInit(&RTC_Alarm);
  RTC_Alarm.RTC_AlarmTime.RTC_Hours   = 01;
  RTC_Alarm.RTC_AlarmTime.RTC_Minutes = 05;
  RTC_Alarm.RTC_AlarmTime.RTC_Seconds = 00;
  RTC_Alarm.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;
  RTC_SetAlarm(RTC_Format_BIN, &RTC_Alarm);

  RTC_ITConfig(RTC_IT_ALRA, ENABLE);
  RTC_AlarmCmd(ENABLE);
	*/
	
	/* Configure WakeUp After 1 hour */
	RTC_SetWakeUpCounter(apply.opntimset);
	
	/* Enable WakeUp Interrupt */
	RTC_ITConfig(RTC_IT_WUT, ENABLE);
	
	/* Disable WakeUp */
	RTC_WakeUpCmd(ENABLE);
	#else

  /* Select LSE (32.768 KHz) as RTC clock source */
  CLK_RTCClockConfig(CLK_RTCCLKSource_LSE, clkdiv);
	
  /* Wait for LSE clock to be ready */
  while (CLK_GetFlagStatus(CLK_FLAG_LSERDY) == RESET);

	/* Enable RTC peripheral */
  CLK_PeripheralClockConfig(CLK_Peripheral_RTC, ENABLE);

  /* Configures the RTC wakeup timer_step = LSE/16 = 488.28125us */
	RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
	
	/* Configure WakeUp After 1 hour */
	RTC_SetWakeUpCounter(apply.opntimset);
	
	/* Enable WakeUp Interrupt */
	RTC_ITConfig(RTC_IT_WUT, ENABLE);
	
	/* Enable WakeUp */
	RTC_WakeUpCmd(ENABLE);
	
	/* RTC wake-up event every 500 ms (timer_step x (1023 + 1) )*/
  //RTC_SetWakeUpCounter(1023);
  //RTC_WakeUpCmd(ENABLE);
	#endif
}
/**
  * @brief  Gpio port initial function
  * @param  None
  * @retval None
  */
void SystemTimer2Initial(void)
{
	#if !PWM_MOD
	/* TIM2 configuration:
     - TIM2 ETR is mapped to LSE
     - TIM2 counter is clocked by LSE div 4
      so the TIM2 counter clock used is LSE / 4 = 32.768 / 4 = 8.192 KHz
     TIM2 Channel1 output frequency = TIM2CLK / (TIM2 Prescaler * (TIM2_PERIOD + 1))
                                   = 8192 / (1 * 8) = 1024 Hz */
																	 
	/* Enable TIM2 clock */
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE);
	
	/* TIM2 Channel1 configuration On PB0 */
  //GPIO_Init(GPIOB, GPIO_Pin_0, GPIO_Mode_Out_PP_Low_Fast);
	
	/* Configure Timer2 Counter Mode and Period */
	TIM2_TimeBaseInit(TIM2_Prescaler_1,TIM2_CounterMode_Up,apply.periods);
	
  /* TIM2 channel Duty cycle is 100*TIM2_PULSE/(TIM2_PERIOD + 1) = 100 * 150/200 = 75 % */
  TIM2_OC1Init(TIM2_OCMode_PWM1,TIM2_OutputState_Enable,apply.dutyset,TIM2_OCPolarity_High,TIM2_OCIdleState_Set);
	
	/* TIM2 Main Output Enable */
  TIM2_CtrlPWMOutputs(ENABLE);
	
	/* Configure Time2 Interrupt */
	TIM2_ITConfig(TIM2_IT_Update,ENABLE);

  /* TIM2 counter enable */
  TIM2_Cmd(ENABLE);
	#else
	
	/* Enable TIM2 clock */
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE);

	/* Set the Prescaler value */
	TIM2_PrescalerConfig(TIM2_Prescaler_16,TIM2_PSCReloadMode_Update);
	
	/* Set the counter mode up */
	TIM2_CounterModeConfig(TIM2_CounterMode_Down);
	
	/* Set the Autoreload value 10us */
	TIM2_SetAutoreload(10); 
	
	/* Configure Time2 Interrupt */
	TIM2_ITConfig(TIM2_IT_Update,ENABLE);
	
  /* TIM2 counter enable */
  TIM2_Cmd(ENABLE);
	
	#endif
	
}
/**
  * @brief  GPIO port initial function
  * @param  None
  * @retval None
  */
void SystemHwgpioInitial(void)
{
	/* Led port configure */
	GPIO_Init(LED_GPIO_PORT,LED_GPIO_PINS,GPIO_Mode_Out_PP_High_Fast);
	
	/* Key port inport pin configure */
	EXTI_SetPinSensitivity(EXTI_Pin_6,EXTI_Trigger_Falling);
	EXTI_SetPinSensitivity(EXTI_Pin_7,EXTI_Trigger_Falling);
	GPIO_Init(KEY_GPIO_PORTI,KEY_GPIO_PINSI,GPIO_Mode_In_FL_IT);
	
	/* Key port output pin configure */
	GPIO_Init(KEY_GPIO_PORTO1,KEY_GPIO_PINSO1,GPIO_Mode_Out_PP_High_Fast);
	GPIO_Init(KEY_GPIO_PORTO2,KEY_GPIO_PINSO2,GPIO_Mode_Out_PP_High_Fast);
	
	/* Solenoid port configure */
	GPIO_Init(GPIOE, GPIO_Pin_5,GPIO_Mode_Out_PP_High_Fast);
	GPIO_Init(GPIOE, GPIO_Pin_6,GPIO_Mode_Out_PP_High_Fast);
	GPIO_Init(GPIOC, GPIO_Pin_7,GPIO_Mode_Out_PP_High_Fast);
	
	/* Low battery detect power */
	//GPIO_Init(GPIOE, GPIO_Pin_4,GPIO_Mode_Out_PP_High_Fast);
	
	/* Light sensor detect power*/
	GPIO_Init(GPIOA, GPIO_Pin_6,GPIO_Mode_Out_PP_High_Fast);
	
	/* PWM output port configure */
	GPIO_Init(GPIOB, GPIO_Pin_0, GPIO_Mode_Out_PP_Low_Fast);
	
	/* Configure test mode key */
	GPIO_Init(GPIOF, GPIO_Pin_0, GPIO_Mode_In_PU_No_IT);
}
/**
  * @brief  Adc interface initial function
  * @param  None
  * @retval None
  */
void SystemHwadcInitial(void)
{
	/* Enable ADC1 clock */
  CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, ENABLE);
	
  /* Initialise and configure ADC1 */
  ADC_Init(ADC1,ADC_ConversionMode_Single,ADC_Resolution_12Bit,ADC_Prescaler_2);
	
	/* ADC channel1 Sampling time configure */ 
  ADC_SamplingTimeConfig(ADC1,ADC_Group_FastChannels,ADC_SamplingTime_384Cycles);
	
	/* ADC channel1 Sampling time configure */ 
  ADC_SamplingTimeConfig(ADC1,ADC_Group_SlowChannels,ADC_SamplingTime_384Cycles);

  /* Enable ADC1 Channel 2 for low battery detect*/
  ADC_ChannelCmd(ADC1, ADC_Channel_2, DISABLE);
	
  /* Enable ADC1 Channel 4 for startup valtage*/
  ADC_ChannelCmd(ADC1, ADC_Channel_4, DISABLE);
	
	/* Enable ADC1 Channel vref for startup valtage*/
	ADC_ChannelCmd(ADC1, ADC_Channel_Vrefint, DISABLE);

  /* Enable End of conversion ADC1 Interrupt */
  //ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
	
	/* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);
	
	#ifdef VREFOUTEN
	/* Output internal refence voltage */
	flag = PWR_GetFlagStatus(PWR_FLAG_VREFINTF);
	CLK_PeripheralClockConfig(CLK_Peripheral_COMP, ENABLE);
	COMP->CSR3 |= (uint8_t)COMP_CSR3_VREFOUTEN;
	RI->IOSR1 &= (~(uint8_t)1 << 2); 
	RI->IOSR2 &= (~(uint8_t)1 << 2);
	RI->IOSR3 |= ((uint8_t)1 << 2);
	#endif
}
/**
  * @brief  Configures COM port
  * @param  Specifies the COM port to be configured
  * @retval None
  */
void SystemUartInitial(void)
{
	/* Enable USART clock */
  CLK_PeripheralClockConfig(CLK_Peripheral_USART1, ENABLE);
	
	/* Configure USART Tx as alternate function push-pull */
  GPIO_ExternalPullUpConfig(GPIOC, GPIO_Pin_3, ENABLE);

  /* Configure USART Rx as alternate function push-pull */
  GPIO_ExternalPullUpConfig(GPIOC, GPIO_Pin_2, ENABLE);
	
  /* USART1 configuration 115200,8 Bits, One Stop Bit,No parity */
  USART_Init(USART1,115200,USART_WordLength_8b,USART_StopBits_1,USART_Parity_No,(USART_Mode_Tx|USART_Mode_Tx));

	/* Enable usart1 port */
	USART_Cmd(USART1,ENABLE);
}

/**
  * @brief  Adc interface initial function
  * @param  None
  * @retval None
  */
void SysteDeviceInitial(void)
{
	/* Power on close water */
	event.timset = true;
	event.opnwat = true;
	event.powron = false;
	event.turnon = false;
	
	apply.offcunt = 0;
	apply.nlgtchk = 0;
	apply.detcngt = 0;
	apply.detcday = 0;

	apply.opntimset = MINS30;
	apply.chargtime = CHRGNO;
	apply.workmod = NORMALMODE;
	
	/* Set captor charge pwm */
	apply.dutyset = PWMDUTY;
	apply.periods = PERIODS;
	
	/* open photor sensor power */
	GPIO_WriteBit(GPIOA,GPIO_Pin_6,RESET);
	
	/* Valve no open or close */
	GPIO_WriteBit(GPIOE,GPIO_Pin_6,RESET);
	GPIO_WriteBit(GPIOC,GPIO_Pin_7,RESET);
}
/**
  * @brief  Inserts a delay time.
  * @param  nCount: specifies the delay time length.
  * @retval None
  */
void SoftwareDelay(__IO uint32_t nCount)
{
  for (; nCount != 0; nCount--);
}
/********************** (C) COPYRIGHT WISEALLY ******************END OF FILE****/