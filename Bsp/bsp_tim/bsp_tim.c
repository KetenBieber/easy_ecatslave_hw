/**
 * @file bsp_tim.c
 * @author Keten (2863861004@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-04-09
 *
 * @copyright Copyright (c) 2025
 *
 * @attention :
 * @note :
 * @versioninfo :
 */
#include "bsp_tim.h"

void TIM_Configuration(void) {  // 1ms

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  ESC_LOCAL_TIM.Instance = LOCAL_TIM;
  ESC_LOCAL_TIM.Init.Prescaler = 840 - 1;
  ESC_LOCAL_TIM.Init.CounterMode = TIM_COUNTERMODE_UP;
  ESC_LOCAL_TIM.Init.Period = 100 - 1;
  ESC_LOCAL_TIM.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  ESC_LOCAL_TIM.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&ESC_LOCAL_TIM) != HAL_OK) {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&ESC_LOCAL_TIM, &sClockSourceConfig) !=
      HAL_OK) {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&ESC_LOCAL_TIM, &sMasterConfig) !=
      HAL_OK) {
    Error_Handler();
  }
}

void CAN0TASK_TIM_Configuration(void) {
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  CAN0_TASK_TIM_INSTANCE.Instance = CAN0_TASK_TIM;
  CAN0_TASK_TIM_INSTANCE.Init.Prescaler = 840 - 1;               // 预分频值
  CAN0_TASK_TIM_INSTANCE.Init.CounterMode = TIM_COUNTERMODE_UP;  // 向上计数
  CAN0_TASK_TIM_INSTANCE.Init.Period = 50 - 1;                   // 10000hz
  CAN0_TASK_TIM_INSTANCE.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  CAN0_TASK_TIM_INSTANCE.Init.AutoReloadPreload =
      TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&CAN0_TASK_TIM_INSTANCE) != HAL_OK) {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&CAN0_TASK_TIM_INSTANCE, &sClockSourceConfig) !=
      HAL_OK) {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&CAN0_TASK_TIM_INSTANCE,
                                            &sMasterConfig) != HAL_OK) {
    Error_Handler();
  }
}

void CAN1TASK_TIM_Configuration(void) {
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  CAN1_TASK_TIM_INSTANCE.Instance = CAN1_TASK_TIM;
  CAN1_TASK_TIM_INSTANCE.Init.Prescaler = 840 - 1;               // 预分频值
  CAN1_TASK_TIM_INSTANCE.Init.CounterMode = TIM_COUNTERMODE_UP;  // 向上计数
  CAN1_TASK_TIM_INSTANCE.Init.Period = 50 - 1;                   // 10000hz
  CAN1_TASK_TIM_INSTANCE.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  CAN1_TASK_TIM_INSTANCE.Init.AutoReloadPreload =
      TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&CAN1_TASK_TIM_INSTANCE) != HAL_OK) {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&CAN1_TASK_TIM_INSTANCE, &sClockSourceConfig) !=
      HAL_OK) {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&CAN1_TASK_TIM_INSTANCE,
                                            &sMasterConfig) != HAL_OK) {
    Error_Handler();
  }
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle) {
  if (tim_baseHandle->Instance == LOCAL_TIM) {
    /* USER CODE BEGIN TIM2_MspInit 0 */

    /* USER CODE END TIM2_MspInit 0 */
    /* TIM2 clock enable */
    LOCAL_TIM_RCC_CLK_ENABLE;

    /* TIM2 interrupt Init */
    HAL_NVIC_SetPriority(LOCAL_TIM_IRQ, 1, 0);
    // HAL_NVIC_EnableIRQ(LOCAL_TIM_IRQ);// 不允许使能，交由ecat协议栈处理
  } else if (tim_baseHandle->Instance == CAN0_TASK_TIM) {
    /* USER CODE BEGIN TIM3_MspInit 0 */

    /* USER CODE END TIM3_MspInit 0 */
    /* TIM3 clock enable */
    CAN0_TASK_TIM_RCC_CLK_ENABLE;

    /* TIM3 interrupt Init */
    HAL_NVIC_SetPriority(CAN0_TASK_TIM_IRQ, 1, 0);
    HAL_NVIC_EnableIRQ(CAN0_TASK_TIM_IRQ);
  } else if (tim_baseHandle->Instance == CAN1_TASK_TIM) {
    /* USER CODE BEGIN TIM3_MspInit 0 */

    /* USER CODE END TIM3_MspInit 0 */
    /* TIM3 clock enable */
    CAN1_TASK_TIM_RCC_CLK_ENABLE;

    /* TIM3 interrupt Init */
    HAL_NVIC_SetPriority(CAN1_TASK_TIM_IRQ, 1, 0);
    HAL_NVIC_EnableIRQ(CAN1_TASK_TIM_IRQ);
  }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle) {
  if (tim_baseHandle->Instance == LOCAL_TIM) {
    /* USER CODE BEGIN TIM2_MspDeInit 0 */

    /* USER CODE END TIM2_MspDeInit 0 */
    /* Peripheral clock disable */
    LOCAL_TIM_RCC_CLK_DISABLE;

    /* TIM2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(LOCAL_TIM_IRQ);
    /* USER CODE BEGIN TIM2_MspDeInit 1 */

    /* USER CODE END TIM2_MspDeInit 1 */
  } else if (tim_baseHandle->Instance == CAN0_TASK_TIM) {
    /* USER CODE BEGIN TIM3_MspDeInit 0 */

    /* USER CODE END TIM3_MspDeInit 0 */
    /* Peripheral clock disable */
    CAN0_TASK_TIM_RCC_CLK_DISABLE;

    /* TIM3 interrupt Deinit */
    HAL_NVIC_DisableIRQ(CAN0_TASK_TIM_IRQ);
    /* USER CODE BEGIN TIM3_MspDeInit 1 */

    /* USER CODE END TIM3_MspDeInit 1 */
  } else if (tim_baseHandle->Instance == CAN1_TASK_TIM) {
    /* USER CODE BEGIN TIM3_MspDeInit 0 */

    /* USER CODE END TIM3_MspDeInit 0 */
    /* Peripheral clock disable */
    CAN1_TASK_TIM_RCC_CLK_DISABLE;

    /* TIM3 interrupt Deinit */
    HAL_NVIC_DisableIRQ(CAN1_TASK_TIM_IRQ);
    /* USER CODE BEGIN TIM3_MspDeInit 1 */

    /* USER CODE END TIM3_MspDeInit 1 */
  }
}

void StartTimIT(void) {
  HAL_TIM_Base_Start_IT(&ESC_LOCAL_TIM);           // 开启定时器计数器
  HAL_TIM_Base_Start_IT(&CAN0_TASK_TIM_INSTANCE);  // 开启can任务定时器计数器
  HAL_TIM_Base_Start_IT(&CAN1_TASK_TIM_INSTANCE);  // 开启can任务定时器计数器
}
