/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

#include "main.h"
#include "cmsis_os.h"
#include <stdio.h>
#include <stdbool.h>

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);

int __io_putchar(int ch);
void vDummyTask1(void *pvParameters);
void vDummyTask2(void *pvParameters);
void vDummyTask3(void *pvParameters);

typedef uint32_t TaskProfiler;

UART_HandleTypeDef huart1;

TaskProfiler 	Dummy1TaskProfiler, Dummy2TaskProfiler, Dummy3TaskProfiler;
TaskHandle_t	dummy_1_handle, dummy_2_handle, dummy_3_handle;

uint32_t		task_suspend_monitor;
uint32_t	 	task_resume_monitor;

bool			is_suspended = false;

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_USART1_UART_Init();

  xTaskCreate(vDummyTask1,
		  	  "Dummy Task 1 Controller",
			  100,
			  NULL,
			  1,
			  &dummy_1_handle);

  xTaskCreate(vDummyTask2,
		  	  "Dummy Task 2 Controller",
			  100,
			  NULL,
			  1,
			  &dummy_2_handle);

  xTaskCreate(vDummyTask3,
 		  	  "Dummy Task 3 Controller",
 			  100,
 			  NULL,
 			  1,
			  &dummy_3_handle);

  vTaskStartScheduler();

  while (1)
  {

  }

}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInitStruct.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

static void MX_USART1_UART_Init(void)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
}

static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();

  /* Reset Pin */
  HAL_GPIO_WritePin(GPIOI, GPIO_PIN_1, GPIO_PIN_RESET);

  /* Pin Configuration */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

  /* Pin initialization */
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);


}


/* Used for USB message debugging */
int __io_putchar(int ch)
{
	HAL_UART_Transmit(&huart1, (uint8_t *) &ch, 1, 0xFFFF);
	return ch;
}

void vDummyTask1(void *pvParameters)
{
	int i;
	while(1)
	{
		Dummy1TaskProfiler++;
		task_suspend_monitor++;

		HAL_GPIO_WritePin(GPIOI, GPIO_PIN_1, GPIO_PIN_SET);
		for(i = 0; i < 50000; i++) {}


		if(task_suspend_monitor < 25)
		{
			printf("Dummy Task 1 running... \n\r");

		}
		else
		{
			is_suspended = true;
			task_suspend_monitor = 0;
			printf("Dummy Task 1 Suspended... \n\r");
			vTaskSuspend(dummy_1_handle);
		}
	};
}

void vDummyTask2(void *pvParameters)
{
	while(1)
	{
		int i;
		Dummy2TaskProfiler++;
		for(i = 0; i < 75000; i++) {}

		if(is_suspended)
		{
			task_resume_monitor++;

			if(task_resume_monitor >= 50)
			{
				vTaskResume(dummy_1_handle);
				task_resume_monitor = 0;
				is_suspended = false;
			}

		}
	};
}

void vDummyTask3(void *pvParameters)
{
	int i;
	while(1)
	{

		Dummy3TaskProfiler++;

		if(is_suspended == false)
		{
			HAL_GPIO_WritePin(GPIOI, GPIO_PIN_1, GPIO_PIN_RESET);
			for(i = 0; i < 40000; i++) {}
		}

	};
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM6)
  {
    HAL_IncTick();
  }

}
void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
