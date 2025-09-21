/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum
{
  PLAYER_NONE = 0U,
  PLAYER_ONE,
  PLAYER_TWO
} PlayerId;

typedef enum
{
  GAME_PHASE_ARMING = 0U,
  GAME_PHASE_WAIT_INPUT,
  GAME_PHASE_FINISHED
} GamePhase;

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
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
static uint32_t generateRandomNumber(void);
static void showWinner(PlayerId winner);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */
  HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);

  const uint32_t minDelayMs = 0U;
  const uint32_t maxDelayMs = 20000U;
  uint32_t armDelayMs = minDelayMs + (generateRandomNumber() % (maxDelayMs - minDelayMs + 1U));
  uint32_t armStartTick = HAL_GetTick();
  GamePhase phase = GAME_PHASE_ARMING;
  PlayerId winner = PLAYER_NONE;
  bool prevP1Pressed = false;
  bool prevP2Pressed = false;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    uint32_t now = HAL_GetTick();
    bool p1Pressed = (HAL_GPIO_ReadPin(P1_GPIO_Port, P1_Pin) == GPIO_PIN_RESET);
    bool p2Pressed = (HAL_GPIO_ReadPin(P2_GPIO_Port, P2_Pin) == GPIO_PIN_RESET);
    bool p1Edge = p1Pressed && !prevP1Pressed;
    bool p2Edge = p2Pressed && !prevP2Pressed;

    if (phase == GAME_PHASE_ARMING)
    {
      if (p1Edge && p2Edge)
      {
        winner = PLAYER_NONE;
        phase = GAME_PHASE_FINISHED;
      }
      else if (p1Edge)
      {
        winner = PLAYER_TWO;
        phase = GAME_PHASE_FINISHED;
      }
      else if (p2Edge)
      {
        winner = PLAYER_ONE;
        phase = GAME_PHASE_FINISHED;
      }
      else if ((now - armStartTick) >= armDelayMs)
      {
        HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
        phase = GAME_PHASE_WAIT_INPUT;
      }
    }
    else if (phase == GAME_PHASE_WAIT_INPUT)
    {
      if (p1Edge && p2Edge)
      {
        winner = PLAYER_NONE;
        phase = GAME_PHASE_FINISHED;
      }
      else if (p1Edge)
      {
        winner = PLAYER_ONE;
        phase = GAME_PHASE_FINISHED;
      }
      else if (p2Edge)
      {
        winner = PLAYER_TWO;
        phase = GAME_PHASE_FINISHED;
      }
    }

    prevP1Pressed = p1Pressed;
    prevP2Pressed = p2Pressed;

    if (phase == GAME_PHASE_FINISHED)
    {
      break;
    }

    HAL_Delay(1);
  }

  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
  showWinner(winner);

  while (1)
  {
    /* Game finished: wait for hardware reset */
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LD1_Pin|LD2_Pin|LD3_Pin, GPIO_PIN_RESET);

  /* Configure GPIO pins : LD1_Pin LD2_Pin LD3_Pin */
  GPIO_InitStruct.Pin = LD1_Pin|LD2_Pin|LD3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* Configure GPIO pins : P1_Pin P2_Pin */
  GPIO_InitStruct.Pin = P1_Pin|P2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */
static void showWinner(PlayerId winner)
{
  GPIO_PinState p1State = GPIO_PIN_RESET;
  GPIO_PinState p2State = GPIO_PIN_RESET;

  if (winner == PLAYER_ONE)
  {
    p1State = GPIO_PIN_SET;
  }
  else if (winner == PLAYER_TWO)
  {
    p2State = GPIO_PIN_SET;
  }
  else if (winner == PLAYER_NONE)
  {
    p1State = GPIO_PIN_SET;
    p2State = GPIO_PIN_SET;
  }

  HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, p1State);
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, p2State);
}

static uint32_t generateRandomNumber(void)
{
  static uint32_t lcgState = 0U;

  if (lcgState == 0U)
  {
    uint32_t seed = HAL_GetTick() ^ SysTick->VAL ^ 0xA5A5A5A5U;

    if (seed == 0U)
    {
      seed = 0x13579BDFU;
    }

    lcgState = seed;
  }

  lcgState = (lcgState * 1664525U) + 1013904223U;
  return lcgState;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
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
  (void)file;
  (void)line;
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

