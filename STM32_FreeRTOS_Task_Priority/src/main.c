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
TaskHandle_t xTaskHandle2 = NULL;

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
uint8_t switch_prio = 0;

//Task function prototypes
void vTask1_handler(void *param);
void vTask2_handler(void *param);

static void prvSetupHardware(void);
void printmsg(char *msg);
static void prvSetupUart(void);
static void LedSetup(void);
static void ButtonHandler(void);
void  rtos_delay(uint32_t delay_in_ms);

			

int main(void)
{
	//HSI ON, PLL OOF, HSE OFF, system clock = 16MHZ, cpu_clock =
	RCC_DeInit();

	//2. Update the SystemCoreClock variable
	SystemCoreClockUpdate();

	xTaskCreate(vTask1_handler,"Task1 Handler",200,NULL,2,&xTaskHandle1);

	xTaskCreate(vTask2_handler,"Task2 Handler",200,NULL,3,&xTaskHandle2);

	prvSetupHardware();

	vTaskStartScheduler();
	for(;;);
}
void vTask1_handler(void *param)
{
	while(1)
	{
		UBaseType_t p1,p2;

			sprintf(usg_msg,"Task-1 is running\r\n");
			printmsg(usg_msg);

			sprintf(usg_msg,"Task-1 Priority %ld\r\n",uxTaskPriorityGet(xTaskHandle1));
			printmsg(usg_msg);

			sprintf(usg_msg,"Task-2 Priority %ld\r\n",uxTaskPriorityGet(xTaskHandle2));
			printmsg(usg_msg);

			while(1)
			{
				if( switch_prio )
				{
					switch_prio = FALSE;

					p1 = uxTaskPriorityGet(xTaskHandle1);
					p2 = uxTaskPriorityGet(xTaskHandle2);

					//switch prio
					vTaskPrioritySet(xTaskHandle1,p2);
					vTaskPrioritySet(xTaskHandle2,p1);



				}else
				{
					GPIO_ToggleBits(GPIOA,GPIO_Pin_5);
					rtos_delay(200);
				}

			}
	}
}
void vTask2_handler(void *param)
{
	while(1)
	{
		UBaseType_t p1,p2;

		sprintf(usg_msg,"Task-2 is running\r\n");
		printmsg(usg_msg);

		sprintf(usg_msg,"Task-1 Priority %ld\r\n",uxTaskPriorityGet(xTaskHandle1));
		printmsg(usg_msg);

		sprintf(usg_msg,"Task-2 Priority %ld\r\n",uxTaskPriorityGet(xTaskHandle2));
		printmsg(usg_msg);

		while(1)
		{
			if( switch_prio )
			{
				switch_prio = FALSE;

				p1 = uxTaskPriorityGet(xTaskHandle1);
				p2 = uxTaskPriorityGet(xTaskHandle2);

				//switch prio
				vTaskPrioritySet(xTaskHandle1,p2);
				vTaskPrioritySet(xTaskHandle2,p1);


			}else
			{
				GPIO_ToggleBits(GPIOA,GPIO_Pin_5);
				rtos_delay(1000);
			}


		}
	}
}
static void prvSetupHardware(void)
{
	LedSetup();
	ButtonHandler();
	prvSetupUart();
}
static void LedSetup(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	GPIO_InitTypeDef Led_Handler;
	Led_Handler.GPIO_Pin = GPIO_Pin_12;
	Led_Handler.GPIO_OType = GPIO_OType_PP;
	Led_Handler.GPIO_PuPd = GPIO_PuPd_NOPULL;
	Led_Handler.GPIO_Mode = GPIO_Mode_OUT;
	Led_Handler.GPIO_Speed = GPIO_Low_Speed;
	GPIO_Init(GPIOD,&Led_Handler);
	GPIO_WriteBit(GPIOD,GPIO_Pin_12,Bit_RESET);
}
static void ButtonHandler(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
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
	vTaskPrioritySet(&xTaskHandle1,3);
	vTaskPrioritySet(&xTaskHandle2,2);
}
void printmsg(char *msg)
{
	for(uint32_t i=0; i < strlen(msg); i++)
	{
		while ( USART_GetFlagStatus(USART2,USART_FLAG_TXE) != SET);
		USART_SendData(USART2,msg[i]);
	}

	while ( USART_GetFlagStatus(USART2,USART_FLAG_TC) != SET);

}
static void prvSetupUart(void)
{
	GPIO_InitTypeDef gpio_uart_pins;
	USART_InitTypeDef uart2_init;

	//1. Enable the UART2  and GPIOA Peripheral clock
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);

	//PA2 is UART2_TX, PA3 is UART2_RX

	//2. Alternate function configuration of MCU pins to behave as UART2 TX and RX

	//zeroing each and every member element of the structure
	memset(&gpio_uart_pins,0,sizeof(gpio_uart_pins));

	gpio_uart_pins.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	gpio_uart_pins.GPIO_Mode = GPIO_Mode_AF;
	gpio_uart_pins.GPIO_PuPd = GPIO_PuPd_UP;
	gpio_uart_pins.GPIO_OType= GPIO_OType_PP;
	gpio_uart_pins.GPIO_Speed = GPIO_High_Speed;
	GPIO_Init(GPIOA, &gpio_uart_pins);


	//3. AF mode settings for the pins
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); //PA2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); //PA3

	//4. UART parameter initializations
	//zeroing each and every member element of the structure
	memset(&uart2_init,0,sizeof(uart2_init));

	uart2_init.USART_BaudRate = 115200;
	uart2_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	uart2_init.USART_Mode =  USART_Mode_Tx | USART_Mode_Rx;
	uart2_init.USART_Parity = USART_Parity_No;
	uart2_init.USART_StopBits = USART_StopBits_1;
	uart2_init.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2,&uart2_init);


	//5. Enable the UART2 peripheral
	USART_Cmd(USART2,ENABLE);

}
void  rtos_delay(uint32_t delay_in_ms)
{
	uint32_t current_tick_count = xTaskGetTickCount();

	uint32_t delay_in_tick = (delay_in_ms * configTICK_RATE_HZ) / 1000;

	while(xTaskGetTickCount() < (current_tick_count + delay_in_tick));
}

