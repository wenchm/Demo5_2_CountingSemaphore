/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "semphr.h"
#include <stdio.h>
#include "usart.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for Task_CheckIn */
osThreadId_t Task_CheckInHandle;
const osThreadAttr_t Task_CheckIn_attributes = {
  .name = "Task_CheckIn",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Sem_Tables */
osSemaphoreId_t Sem_TablesHandle;
const osSemaphoreAttr_t Sem_Tables_attributes = {
  .name = "Sem_Tables"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void AppTask_CheckIn(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of Sem_Tables */
  Sem_TablesHandle = osSemaphoreNew(5, 0, &Sem_Tables_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of Task_CheckIn */
  Task_CheckInHandle = osThreadNew(AppTask_CheckIn, NULL, &Task_CheckIn_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_AppTask_CheckIn */
/**
  * @brief  Function implementing the Task_CheckIn thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_AppTask_CheckIn */
void AppTask_CheckIn(void *argument)
{
  /* USER CODE BEGIN AppTask_CheckIn */
	UBaseType_t totalTables=uxSemaphoreGetCount(Sem_TablesHandle);						///<Get Semaphore Count
	printf("Total tables=  %ld.\r\n",totalTables);

	/* Infinite loop */
	for(;;)
	{
		GPIO_PinState keyState=HAL_GPIO_ReadPin(KeyRight_GPIO_Port, KeyRight_Pin); 	///<PA6=KeyRight
		if (keyState==GPIO_PIN_RESET)  																			///<KeyRight Active low
		{
			BaseType_t result=xSemaphoreTake(Sem_TablesHandle, pdMS_TO_TICKS(100));
			if (result==pdTRUE)
				printf("Check in OK. \r\n");
			else
				printf("Check in fail. \r\n");
			vTaskDelay(pdMS_TO_TICKS(300)); ///<Delay, remove jitter effects, and allow task scheduling to be executed
		}
		UBaseType_t availableTables=uxSemaphoreGetCount(Sem_TablesHandle);
		printf("Available tables=  %ld.\r\n",availableTables);
		vTaskDelay(pdMS_TO_TICKS(10));
	}
  /* USER CODE END AppTask_CheckIn */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/**	WakeUpTimerEvent ISR
 * The WakeUp is interrupted every 3s,
 * and the semaphore is released
 * and a scheduling task is applied for each interrupt.
 *
 */
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
	if (Sem_TablesHandle != NULL)
	{
		BaseType_t  highTaskWoken=pdFALSE;
		xSemaphoreGiveFromISR(Sem_TablesHandle, &highTaskWoken);
		portYIELD_FROM_ISR(highTaskWoken);
	}
}

int __io_putchar(int ch)
{
	HAL_UART_Transmit(&huart3,(uint8_t*)&ch,1,0xFFFF);
	return ch;
}
/* USER CODE END Application */

