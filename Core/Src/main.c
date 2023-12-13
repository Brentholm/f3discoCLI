/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "main.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "LedRelated.h"
#include <stdlib.h>  //for malloc
#include "console.h"  //for the cmd line interface
#include "stm32f3_discovery_accelerometer.h"

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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void USART1_SendString(const char* str);
int __io_putchar(int ch);
int __io_getchar(int ch);
int my_getchar(void);


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// for printing out address of an initialized global variable
  uint32_t globalVarInit= 0x0ba10ba1;


// for printing out address of an uninitialized global variable
   uint32_t globalVar;

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  //HAL_UART_Register_Callback(&huart1, HAL_UART_RX_, ByteReceivedUart1);
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  //MX_NVIC_Init();
  LedRoseSet();
  HAL_Delay(50);
  LedRoseToggle();


  USART1_SendString("Hello, World!\r\n");
  //printf("Hi Brent\r\n");

  printf("\r\n");
  // the following bunch of hacking is for Homework #8
  // 1. Print out the stack pointer
  // Credit to https://github.com/dslik/red-jellies/tree/main/lesson-8

  printf(" The stack pointer can be accessed from the ARM r13 register \r\n");
  printf(" 1. define a volatile uint32_t to hold the value of the stack pointer \r\n");
  printf(" 2. use an assembly instruction to move MCU register R13 \r\n");
  printf("    in to the variable known as stackPointer \r\n");
  printf(" 3. Now printf can be used to print out the value in stackPointer\r\n");
  printf("    that is: 'stackPointer' contains R13, the Stack Pointer \r\n");

  volatile uint32_t stackPointer;
  __asm__ volatile ("mov %0, r13;" : "=r"(stackPointer));  //now stackPointer will contain R13, i.e. the Cortex M Stack Pointer
  //printf("   Stack pointer, format specifier  X:          0x%08X\r\n", (uint32_t) stackPointer);
  //printf("   Stack pointer, format specifier lux:          0x%08lux\r\n", (uint32_t) stackPointer);
  printf("   Stack pointer:               0x%08lX\r\n", (uint32_t) stackPointer);
  printf("\r\n");


// to inspect the heap pointer, malloc something
  printf("for the Heap pointer, declare a local variable and print its address out \r\n");

  void foo(void){
	  uint32_t bar;
	  printf("   Heap pointer:                0x%08lX\r\n", (uint32_t) &bar);

	  uint32_t *p;
	  // Allocate memory for a single uint32
	  p = malloc(sizeof(uint32_t));
	  // Store a value in the allocated memory
	  *p = 12345;
      // Print the value to the console
	  printf("   Heap from malloc:            0x%08lX\r\n", (uint32_t) &p);

	  // Free the allocated memory
	  free(p);
	  return;
  }

  //call foo so that it's actually used in code and will print :
  foo();
  printf("\r\n");

  printf("   Initialized global:          0x%08lX\r\n", (uint32_t) &globalVarInit);


  printf("   Un-initialized global:       0x%08lX\r\n", (uint32_t) &globalVar);

  // print out address of a static variable and another variable inside a function
  void foo2(void){
  	  static uint32_t staticVar;
  	  uint32_t varInFunc = 1;
  	  printf("   Static local:                0x%08lX\r\n", (uint32_t) &staticVar);
  	  printf("   Var in a Function:           0x%08lX\r\n", (uint32_t) &varInFunc);
  	  return;
    }

  // call the second function:
  foo2();
  printf("\r\n");

  BSP_ACCELERO_Init();

  int16_t accelData[3] = {0};
  BSP_ACCELERO_GetXYZ(accelData);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  //Call my_getchar to read characters interactively
	  //int ch;
	  //while ((ch = my_getchar()) != EOF) {
	  //  __io_putchar(ch); // Print the character received



// enable interrupts for USART1
	  //HAL_UART_Receive_IT(&huart1,);
	  ConsoleInit();

	  while(1)
	  {
		  ConsoleProcess();
	  }

  }

  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void USART1_SendString(const char* str) {
	HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);
}
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
/*PUTCHAR_PROTOTYPE
{
   Place your implementation of fputc here
   e.g. write a character to the USART1 and Loop until the end of transmission
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}*/

// these two functions are redefines that I added per Daniel
int __io_putchar(int ch){
	if (HAL_UART_Transmit(&huart1, (uint8_t*)&ch, 1, 0xFFFF) != HAL_OK)
		return -1;
	return 1;
}


/*
int __io_getchar(void){
	int c;
	HAL_UART_Receive(&huart1, (uint8_t*)&c, 1, 0xFFFF);
	return c;
}
*/




/*int my_getchar(void) {
	      return __io_getchar();
	  }*/
void customRcvComplete_CB(UART_HandleTypeDef* huart){
	HAL_UART_Transmit(&huart1, (uint8_t*) "hi", 4, 300);
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
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
