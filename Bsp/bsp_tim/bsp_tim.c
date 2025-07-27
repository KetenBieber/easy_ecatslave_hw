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
 * @note : 注意定时器任务配置，需要让消费能力跟上生产能力（指can发送任务）
 * @versioninfo :
 */
#include "bsp_tim.h"

void TIM_Configuration(void) {  // 1ms

#ifndef USE_LL_DRIVER
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
#else
  /* use the LL Driver */
  LL_TIM_InitTypeDef TIM_InitStruct = {0};

  // 使能外设时钟
  LL_APB1_GRP1_EnableClock(LOCAL_TIM_LL);

  // 配置定时器初始化结构体
  TIM_InitStruct.Prescaler = 840 - 1;
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = 100 - 1;  // 1ms
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;

  // 初始化定时器
  LL_TIM_Init(LOCAL_TIM, &TIM_InitStruct);

  // 关闭自动重装载
  LL_TIM_DisableARRPreload(LOCAL_TIM);
  // 设置时钟源，但是一般默认内部时钟源
  LL_TIM_SetClockSource(LOCAL_TIM, LL_TIM_CLOCKSOURCE_INTERNAL);

  // 主模式配置，降触发输出设置为重置，并禁止主从模式
  LL_TIM_SetTriggerOutput(LOCAL_TIM, LL_TIM_TRGO_RESET);
  LL_TIM_DisableMasterSlaveMode(LOCAL_TIM);

  // 清除定时器更新中断标志
  LL_TIM_ClearFlag_UPDATE(LOCAL_TIM);
  // 使能定时器更新中断
  LL_TIM_EnableIT_UPDATE(LOCAL_TIM);

  // 初始化中断
  NVIC_SetPriority(LOCAL_TIM_IRQ,
                   NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 1, 0));

#endif
}

void CAN0TASK_TIM_Configuration(void) {
#ifndef USE_LL_DRIVER
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
#else
  /* use the LL Driver */
  LL_TIM_InitTypeDef TIM_InitStruct = {0};

  // 使能外设时钟
  LL_APB1_GRP1_EnableClock(CAN0_TASK_TIM_LL);

  // 初始化中断
  NVIC_SetPriority(CAN0_TASK_TIM_IRQ,
                   NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 1, 0));
  NVIC_EnableIRQ(CAN0_TASK_TIM_IRQ);

  // 配置定时器初始化结构体
  TIM_InitStruct.Prescaler = 840 - 1;
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload =
      10 - 1;  // 0.1ms，执行一次定时器任务发送1帧的话，1s将有2000帧发送出
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;

  // 初始化定时器
  LL_TIM_Init(CAN0_TASK_TIM, &TIM_InitStruct);

  // 关闭自动重装载
  LL_TIM_DisableARRPreload(CAN0_TASK_TIM);
  // 设置时钟源，但是一般默认内部时钟源
  LL_TIM_SetClockSource(CAN0_TASK_TIM, LL_TIM_CLOCKSOURCE_INTERNAL);

  // 主模式配置，降触发输出设置为重置，并禁止主从模式
  LL_TIM_SetTriggerOutput(CAN0_TASK_TIM, LL_TIM_TRGO_RESET);
  LL_TIM_DisableMasterSlaveMode(CAN0_TASK_TIM);
#endif
}

void CAN1TASK_TIM_Configuration(void) {
#ifndef USE_LL_DRIVER
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
#else
  /* use the LL Driver */
  LL_TIM_InitTypeDef TIM_InitStruct = {0};

  // 使能外设时钟
  LL_APB1_GRP1_EnableClock(CAN1_TASK_TIM_LL);

  // 初始化中断
  NVIC_SetPriority(CAN1_TASK_TIM_IRQ,
                   NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 1, 0));
  NVIC_EnableIRQ(CAN1_TASK_TIM_IRQ);

  // 配置定时器初始化结构体
  TIM_InitStruct.Prescaler = 840 - 1;
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = 10 - 1;  // 0.1ms
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;

  // 初始化定时器
  LL_TIM_Init(CAN1_TASK_TIM, &TIM_InitStruct);

  // 关闭自动重装载
  LL_TIM_DisableARRPreload(CAN1_TASK_TIM);
  // 设置时钟源，但是一般默认内部时钟源
  LL_TIM_SetClockSource(CAN1_TASK_TIM, LL_TIM_CLOCKSOURCE_INTERNAL);

  // 主模式配置，降触发输出设置为重置，并禁止主从模式
  LL_TIM_SetTriggerOutput(CAN1_TASK_TIM, LL_TIM_TRGO_RESET);
  LL_TIM_DisableMasterSlaveMode(CAN1_TASK_TIM);
#endif
}

#ifndef USE_LL_DRIVER
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

#endif

#ifdef USE_LL_DRIVER
static inline void LL_LOCAL_TIM_Base_Start_IT() {
  LL_TIM_EnableCounter(LOCAL_TIM);    // 启动定时器计数器
  LL_TIM_EnableIT_UPDATE(LOCAL_TIM);  // 使能定时器更新中断
}

static inline void LL_CAN0_TASK_TIM_Base_Start_IT() {
  LL_TIM_EnableCounter(CAN0_TASK_TIM);    // 启动定时器计数器
  LL_TIM_EnableIT_UPDATE(CAN0_TASK_TIM);  // 使能定时器更新中断
}

static inline void LL_CAN1_TASK_TIM_Base_Start_IT() {
  LL_TIM_EnableCounter(CAN1_TASK_TIM);    // 启动定时器计数器
  LL_TIM_EnableIT_UPDATE(CAN1_TASK_TIM);  // 使能定时器更新中断
}

#endif

void StartTimer(void) {
#ifndef USE_LL_DRIVER

  HAL_TIM_Base_Start_IT(&ESC_LOCAL_TIM);           // 开启定时器计数器
  HAL_TIM_Base_Start_IT(&CAN0_TASK_TIM_INSTANCE);  // 开启can任务定时器计数器
  HAL_TIM_Base_Start_IT(&CAN1_TASK_TIM_INSTANCE);  // 开启can任务定时器计数器
#else
  LL_LOCAL_TIM_Base_Start_IT();      // 开启定时器计数器
  LL_CAN0_TASK_TIM_Base_Start_IT();  // 开启can任务定时器计数器
  LL_CAN1_TASK_TIM_Base_Start_IT();  // 开启can任务定时器计数器
#endif
}

void WS2812_TIM_Configuration(void) {
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  WS2812_TIM.Instance = WS2812_TIM_INSTANCE;
  WS2812_TIM.Init.Prescaler = 0;
  WS2812_TIM.Init.CounterMode = TIM_COUNTERMODE_UP;
  WS2812_TIM.Init.Period = 210 - 1;
  WS2812_TIM.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  WS2812_TIM.Init.RepetitionCounter = 0;
  WS2812_TIM.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&WS2812_TIM) != HAL_OK) {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&WS2812_TIM, &sMasterConfig) !=
      HAL_OK) {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&WS2812_TIM, &sConfigOC, WS2812_TIM_Channel) !=
      HAL_OK) {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&WS2812_TIM, &sBreakDeadTimeConfig) !=
      HAL_OK) {
    Error_Handler();
  }

  HAL_TIM_MspPostInit(&WS2812_TIM);
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* tim_pwmHandle) {
  if (tim_pwmHandle->Instance == WS2812_TIM_INSTANCE) {
    WS2812_TIM_RCC_CLK_ENABLE;
    WS2812_TIM_PWM_DMA_RCC_CLK_ENABLE;
    WS2812_TIM_PWM_DMA.Instance = WS2812_TIM_PWM_DMA_Instance;
    WS2812_TIM_PWM_DMA.Init.Channel = WS2812_TIM_PWM_DMA_Channel;
    WS2812_TIM_PWM_DMA.Init.Direction = DMA_MEMORY_TO_PERIPH;
    WS2812_TIM_PWM_DMA.Init.PeriphInc = DMA_PINC_DISABLE;
    WS2812_TIM_PWM_DMA.Init.MemInc = DMA_MINC_ENABLE;  // 内存地址自增
    WS2812_TIM_PWM_DMA.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    WS2812_TIM_PWM_DMA.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    WS2812_TIM_PWM_DMA.Init.Mode = DMA_CIRCULAR;
    WS2812_TIM_PWM_DMA.Init.Priority = DMA_PRIORITY_LOW;
    WS2812_TIM_PWM_DMA.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&WS2812_TIM_PWM_DMA) != HAL_OK) {
      Error_Handler();
    }
    __HAL_LINKDMA(tim_pwmHandle, hdma[TIM_DMA_ID_CC2], WS2812_TIM_PWM_DMA);
  } else {
  }
}

void HAL_TIM_MspPostInit(TIM_HandleTypeDef* timHandle) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  if (timHandle->Instance == WS2812_TIM_INSTANCE) {
    WS2812_GPIO_RCC_CLK_ENABLE;
    GPIO_InitStruct.Pin = WS2812_GPIO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
    HAL_GPIO_Init(WS2812_GPIO_Port, &GPIO_InitStruct);
  } else {
  }
}

void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef* tim_pwmHandle) {
  if (tim_pwmHandle->Instance == WS2812_TIM_INSTANCE) {
    WS2812_TIM_RCC_CLK_DISABLE;

    HAL_DMA_DeInit(tim_pwmHandle->hdma[TIM_DMA_ID_CC2]);
  }
}

void WS2812_DMA_Configuration(void) {
  /* DMA2_Stream6_IRQn interrupt configuration */
  WS2812_TIM_PWM_DMA_RCC_CLK_ENABLE;
  HAL_NVIC_SetPriority(WS2812_TIM_DMA_STREAM_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(WS2812_TIM_DMA_STREAM_IRQn);
}