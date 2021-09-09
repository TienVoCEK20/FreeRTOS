/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "stm32f4xx.h"

#include "FreeRTOS.h"
#include "task.h"

TaskHandle_t xTaskHandle1 = NULL;

#define TRUE 1
#define FALSE 0
#define HIGH 1
#define LOW 0
#define AVAILABLE TRUE
#define NOT_AVAILABLE FALSE

//Global variables
char usg_msg[250];
uint8_t ACCESS_KEY = AVAILABLE;
uint8_t button_pressed = 0;
uint8_t button_status_flag = 0;

//Task function prototypes
void vLED_Task_handler(void *param);
void Button_Flag(void *param);

static void prvSetupHardware(void);
static void LedSetup(void);
static void ButtonHandler(void);


int main(void)
{
	//HSI ON, PLL OOF, HSE OFF, system clock = 16MHZ, cpu_clock =
	RCC_DeInit();

	//2. Update the SystemCoreClock variable
	SystemCoreClockUpdate();

	xTaskCreate(vLED_Task_handler,"Led Handler",200,NULL,1,&xTaskHandle1);

	prvSetupHardware();

	vTaskStartScheduler();
	EXTI_InitTypeDef IT_HANDLER1;

	for(;;);
}
void vLED_Task_handler(void *param)
{
	while(1)
	{
		if(button_status_flag == TRUE)
		{
			GPIO_WriteBit(GPIOD,GPIO_Pin_12,Bit_SET);
		}
		else
		{
			GPIO_WriteBit(GPIOD,GPIO_Pin_12,Bit_RESET);
		}
	}
}
void Button_Flag(void *param)
{
	button_status_flag ^= 1;
}
static void prvSetupHardware(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
	LedSetup();
	ButtonHandler();
}
static void LedSetup(void)
{
	GPIO_InitTypeDef Led_Handler;
	memset(&Led_Handler,0,sizeof(Led_Handler));
	Led_Handler.GPIO_Pin = GPIO_Pin_12;
	Led_Handler.GPIO_OType = GPIO_OType_PP;
	Led_Handler.GPIO_PuPd = GPIO_PuPd_NOPULL;
	Led_Handler.GPIO_Mode = GPIO_Mode_OUT;
	Led_Handler.GPIO_Speed = GPIO_Low_Speed;
	GPIO_Init(GPIOD,&Led_Handler);
}
static void ButtonHandler(void)
{
	GPIO_InitTypeDef Button_Handler;
	memset(&Button_Handler,0,sizeof(Button_Handler));
	Button_Handler.GPIO_Pin = GPIO_Pin_0;
	Button_Handler.GPIO_Mode = GPIO_Mode_IN;
	Button_Handler.GPIO_OType = GPIO_OType_PP;
	Button_Handler.GPIO_PuPd = GPIO_PuPd_NOPULL;
	Button_Handler.GPIO_Speed = GPIO_Low_Speed;
	GPIO_Init(GPIOA,&Button_Handler);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,GPIO_PinSource0);

	EXTI_InitTypeDef IT1_Handler;
	IT1_Handler.EXTI_Line = EXTI_Line0;
	IT1_Handler.EXTI_Trigger = EXTI_Trigger_Rising;
	IT1_Handler.EXTI_LineCmd = ENABLE;
	IT1_Handler.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_Init(&IT1_Handler);

	NVIC_SetPriority(EXTI0_IRQn,5);
	NVIC_EnableIRQ(EXTI0_IRQn);
}

void EXTI0_IRQHandler()
{
	//1. Clear the interrupt pending bit of the EXTI line (0)
	EXTI_ClearITPendingBit(EXTI_Line0);
	Button_Flag(NULL);


}
