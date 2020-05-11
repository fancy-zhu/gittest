/**
  ******************************************************************************
  * @file    main.c
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
#include "stdio.h"
#include "stm8l15x.h"
#include "init.h"
#include "key.h"
#include "app.h"
#include "factory.h"

/* Private function prototypes -----------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
#define PUTCHAR_PROTOTYPE char putchar (char c)
#define GETCHAR_PROTOTYPE char getchar (void)
extern RTC_TimeTypeDef RTC_Time;
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void main(void)
{
  /* Configure main clock */
  SystemClockInitial();
	
	/* Device port gpio configure */
	SystemHwgpioInitial();
	
	/* System memory initial */
	SysteDeviceInitial();
	
	/* Configure rtc clock */
	SystemHwRtcInitial(0x40);

  /* Init Timer2 pwm interrupt */
  SystemTimer2Initial();
	
	/* Hardware ADC configure */
	SystemHwadcInitial();
	
	/* Usart1 confgiure */									
	SystemUartInitial();
	
  /* Enable Interrupts */
  enableInterrupts();
	
	/* Factory test mode */
	factory_func();
	
  /* Infinite loop */
  while (1)
  {
		/* Key matrix scan handle */
		KeyMatrixScan();
		
		/* Fast clock mode check */
		if(!event.fstout)
			WorkModeCheck();
			
		/* Status machine process */
		StatusMachineHandle();
		
		/* Enter sleep mode handle */
		SleepStatusHandle();
	}
}
/**
  * @brief Retargets the C library printf function to the USART.
  * @param[in] c Character to send
  * @retval char Character sent
  * @par Required preconditions:
  * - None
  */
PUTCHAR_PROTOTYPE
{
  /* Write a character to the USART */
  USART_SendData8(USART1, c);
  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);

  return (c);
}
/**
  * @brief Retargets the C library scanf function to the USART.
  * @param[in] None
  * @retval char Character to Read
  * @par Required preconditions:
  * - None
  */
GETCHAR_PROTOTYPE
{
  int c = 0;
  /* Loop until the Read data register flag is SET */
  while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
    c = USART_ReceiveData8(USART1);
	return (c);
}
/**
  * @brief  Configure PVD
  * @param  None
  * @retval None
  */
#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
		
  }
}
#endif

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
