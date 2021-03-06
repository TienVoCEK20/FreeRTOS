/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stm32f4xx.h"

#include "FreeRTOS.h"
#include "task.h"


TaskHandle_t xTaskHandle1 = NULL;
TaskHandle_t xTaskHandle2 = NULL;

//Task function prototypes
void vTask1_handler(void *param);
void vTask2_handler(void *param);

#ifdef USE_SEMIHOSTING
extern void initialise_monitor_handles();
#endif

static void prvSetupHardware(void);
static void prvSetupUart(void);

void printmsg(char *msg);

//
#define TRUE 1
#define FALSE 0
#define AVAILABLE TRUE
#define NOT_AVAILABLE FALSE

//Global variables
char usg_msg[250];
uint8_t USART_ACCESS_KEY = AVAILABLE;

int main(void)
{
#ifdef USE_SEMIHOSTING
	initialise_monitor_handles();
#endif

	DWT->CTRL |=  (1 << 0);//Enable CYCCNT in DWT_CTRL

	//1. Reset the RCC clock configuration to the default reset state
	//HSI ON, PLL OOF, HSE OFF, system clock = 16MHZ, cpu_clock =
	RCC_DeInit();

	//2. Update the SystemCoreClock variable
	SystemCoreClockUpdate();

	prvSetupHardware();

	sprintf(usg_msg,"This is hello world application starting\r\n");
	printmsg(usg_msg);

	//Start recording
	SEGGER_SYSVIEW_Conf();
	SEGGER_SYSVIEW_Start();

	//3. Lets create 2 tasks, task-1 and task-2
    xTaskCreate(vTask1_handler,"Task-1/Description",configMINIMAL_STACK_SIZE,NULL,2,&xTaskHandle1);

    xTaskCreate(vTask2_handler,"Task-1/Description",configMINIMAL_STACK_SIZE,NULL,2,&xTaskHandle2);

    //4. Start the scheduler
    vTaskStartScheduler();

	for(;;);
}


void vTask1_handler(void *param)
{
	while(1)
	{
		if(USART_ACCESS_KEY == AVAILABLE)
		{
			USART_ACCESS_KEY = NOT_AVAILABLE;
			printmsg("Hello world from task-1\r\n");
			USART_ACCESS_KEY = AVAILABLE;
			taskYIELD();
		}
	}
}


void vTask2_handler(void *param)
{
	while(1)
	{
		if(USART_ACCESS_KEY == AVAILABLE)
		{
			USART_ACCESS_KEY = NOT_AVAILABLE;
			printmsg("Hello world from task-2\r\n");
			USART_ACCESS_KEY = AVAILABLE;
			taskYIELD();
		}
	}
}

static void prvSetupUart(void)
{
	GPIO_InitTypeDef gpio_uart_pins;
	USART_InitTypeDef uart2_init;

	//1. Enable the UART2 peripheral clock
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	//PA2 is UART2_TX, PA3 is UART2_RX

	//2. Alternate function configuration of MCU pins to behave as UART2 TX and RX

	//zeroing
	memset(&gpio_uart_pins,0,sizeof(gpio_uart_pins));
	gpio_uart_pins.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	gpio_uart_pins.GPIO_Mode = GPIO_Mode_AF;
	gpio_uart_pins.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &gpio_uart_pins);

	//3. AF mode setting for the pins
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); //PA2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); //PA3

	//4. UART parameter initializations
	memset(&uart2_init,0,sizeof(uart2_init));
	uart2_init.USART_BaudRate = 115200;
	uart2_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	uart2_init.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	uart2_init.USART_Parity = USART_Parity_No;
	uart2_init.USART_StopBits = USART_StopBits_1;
	uart2_init.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2,&uart2_init);

	//5.Enable the UART 2 peripheral
	USART_Cmd(USART2,ENABLE);
}

static void prvSetupHardware(void)
{
	prvSetupUart();
}

void printmsg(char *msg)
{
	for(uint32_t i = 0; i < strlen(msg); i++)
	{
		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE) != SET);
		USART_SendData(USART2,msg[i]);

	}
}
