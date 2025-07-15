/**
 * @file bsp_can.c
 * @author Keten (2863861004@qq.com)
 * @brief
 * 板级can包，封装了本从站所需要使用的can的功能，具体引脚参考esc_config.h进行配置
 * @version 0.1
 * @date 2025-04-09
 *
 * @copyright Copyright (c) 2025
 *
 * @attention :
 * @note : 为大疆电机专门写了can发送函数，但是为 __weak
 *         属性，也就是需要用户自己去写
 *         从站作用最小化，只负责收指令发指令
 *         封装好的函数直接被ecat_app调用
 *
 *        2025-07-03 添加can自动重启功能
 *
 *        2025-07-14 改善can逻辑，使用发送完成中断事件驱动，理论接近硬件极限
 *                   加入各种总线监听功能，当进入任务状态时，会自动开启总线监听，此时如果总线上收取不到can数据包，会有一些操作
 *                   加入error错误处理函数，处理can错误，比如can短接时的bus-off，实测可以直接重启成功
 *
 *
 * @versioninfo :
 */
#include "bsp_can.h"

#if defined STM32F4

/* global varibals */
/* 发送句柄 */
CAN_TxHeaderTypeDef CAN0_TxHeader;
CAN_TxHeaderTypeDef CAN1_TxHeader;

/* 看门狗相关 */
bool start_can0_watchdog = false;    // 是否开启看门狗
float last_can0_rx_time = 0.0f;      // 上次can0接收时间
float cur_can0_rx_time = 0.0f;       // 本次can0看门狗时间
float can0_watchdog_timeout = 0.0f;  // can0看门狗超时时间，单位ms
#ifndef ONLY_CAN0
bool start_can1_watchdog = false;    // 是否开启can1看门狗
float last_can1_rx_time = 0.0f;      // 上次can1接收时间
float cur_can1_rx_time = 0.0f;       // 本次can1看门狗时间
float can1_watchdog_timeout = 0.0f;  // can1看门狗超时时间，单位ms
#endif

// can hal层 初始化
void CAN_Configuration(CAN_HandleTypeDef* hcan) {
  if (hcan == &CAN0_Handle) {
    CAN0_Handle.Instance = CAN0_Instance;
    CAN0_Handle.Init.Prescaler = 3;
    CAN0_Handle.Init.Mode = CAN_MODE_NORMAL;
    CAN0_Handle.Init.SyncJumpWidth = CAN_SJW_1TQ;
    CAN0_Handle.Init.TimeSeg1 = CAN_BS1_9TQ;
    CAN0_Handle.Init.TimeSeg2 = CAN_BS2_4TQ;
    CAN0_Handle.Init.TimeTriggeredMode = DISABLE;
    CAN0_Handle.Init.AutoBusOff = ENABLE;
    CAN0_Handle.Init.AutoWakeUp = DISABLE;
    CAN0_Handle.Init.AutoRetransmission = ENABLE;
    CAN0_Handle.Init.ReceiveFifoLocked = DISABLE;
    CAN0_Handle.Init.TransmitFifoPriority = DISABLE;
    if (HAL_CAN_Init(&CAN0_Handle) != HAL_OK) {
      Error_Handler();
    }
  } else if (hcan == &CAN1_Handle) {
#ifndef ONLY_CAN0
    CAN1_Handle.Instance = CAN1_Instance;
    CAN1_Handle.Init.Prescaler = 3;
    CAN1_Handle.Init.Mode = CAN_MODE_NORMAL;
    CAN1_Handle.Init.SyncJumpWidth = CAN_SJW_1TQ;
    CAN1_Handle.Init.TimeSeg1 = CAN_BS1_9TQ;
    CAN1_Handle.Init.TimeSeg2 = CAN_BS2_4TQ;
    CAN1_Handle.Init.TimeTriggeredMode = DISABLE;
    CAN1_Handle.Init.AutoBusOff = ENABLE;
    CAN1_Handle.Init.AutoWakeUp = DISABLE;
    CAN1_Handle.Init.AutoRetransmission = ENABLE;
    CAN1_Handle.Init.ReceiveFifoLocked = DISABLE;
    CAN1_Handle.Init.TransmitFifoPriority = DISABLE;
    if (HAL_CAN_Init(&CAN1_Handle) != HAL_OK) {
      Error_Handler();
    }
#endif
  }
}

// CAN MCU Support Package Init
void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if (canHandle->Instance == CAN0_Instance) {
    CAN0_RCC_CLK_ENABLE;

    CAN0_IO_RCC_CLK_ENABLE;

    GPIO_InitStruct.Pin = CAN0_Tx_Pin | CAN0_Rx_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(CAN0_IO_PORT, &GPIO_InitStruct);

    /* CAN0 interrupt Init */

    // cpu relative
    HAL_NVIC_SetPriority(CAN0_NVIC_RX0_IRQ, 2, 1);
    HAL_NVIC_EnableIRQ(CAN0_NVIC_RX0_IRQ);
    HAL_NVIC_SetPriority(CAN0_NVIC_RX1_IRQ, 2, 1);
    HAL_NVIC_EnableIRQ(CAN0_NVIC_RX1_IRQ);
    HAL_NVIC_SetPriority(CAN0_NVIC_SCE_IRQ, 2, 1);
    HAL_NVIC_EnableIRQ(CAN0_NVIC_SCE_IRQ);
#ifdef USE_CAN_TX_IDLE_INTERRUPT  // 不打算使用那个方案
    HAL_NVIC_SetPriority(CAN0_NVIC_TX_IRQ, 2, 1);
    HAL_NVIC_EnableIRQ(CAN0_NVIC_TX_IRQ);
#endif
  }
#ifndef ONLY_CAN0
  else if (canHandle->Instance == CAN1_Instance) {
    CAN1_RCC_CLK_ENABLE;

    CAN1_IO_RCC_CLK_ENABLE;

    GPIO_InitStruct.Pin = CAN1_Tx_Pin | CAN1_Rx_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN2;
    HAL_GPIO_Init(CAN1_IO_PORT, &GPIO_InitStruct);

    /* CAN0 interrupt Init */

    // can relative
    HAL_NVIC_SetPriority(CAN1_NVIC_RX0_IRQ, 2, 1);
    HAL_NVIC_EnableIRQ(CAN1_NVIC_RX0_IRQ);
    HAL_NVIC_SetPriority(CAN1_NVIC_RX1_IRQ, 2, 1);
    HAL_NVIC_EnableIRQ(CAN1_NVIC_RX1_IRQ);
    HAL_NVIC_SetPriority(CAN1_NVIC_SCE_IRQ, 2, 1);
    HAL_NVIC_EnableIRQ(CAN1_NVIC_SCE_IRQ);
#ifdef USE_CAN_TX_IDLE_INTERRUPT  // 不打算使用那个方案
    HAL_NVIC_SetPriority(CAN1_NVIC_TX_IRQ, 2, 1);
    HAL_NVIC_EnableIRQ(CAN1_NVIC_TX_IRQ);
#endif
  }
#endif
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle) {
  if (canHandle->Instance == CAN0_Instance) {
    CAN0_RCC_CLK_DISABLE;

    HAL_GPIO_DeInit(CAN0_IO_PORT, CAN0_Tx_Pin | CAN0_Rx_Pin);

    /* CAN1 controller interrupt Deinit */
    __HAL_CAN_DISABLE_IT(&CAN0_Handle, CAN_IT_TX_MAILBOX_EMPTY |
                                           CAN_IT_RX_FIFO0_MSG_PENDING |
                                           CAN_IT_RX_FIFO1_MSG_PENDING |
                                           CAN_IT_ERROR | CAN_IT_BUSOFF);

    /* CAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(CAN0_NVIC_RX0_IRQ);
    HAL_NVIC_DisableIRQ(CAN0_NVIC_RX1_IRQ);
    HAL_NVIC_DisableIRQ(CAN0_NVIC_SCE_IRQ);
    HAL_NVIC_DisableIRQ(CAN0_NVIC_TX_IRQ);

  }
#ifndef ONLY_CAN0
  else if (canHandle->Instance == CAN1_Instance) {
    CAN1_RCC_CLK_DISABLE;
    HAL_GPIO_DeInit(CAN1_IO_PORT, CAN1_Tx_Pin | CAN1_Rx_Pin);

    /* CAN2 controller interrupt Deinit */
    __HAL_CAN_DISABLE_IT(&CAN1_Handle, CAN_IT_TX_MAILBOX_EMPTY |
                                           CAN_IT_RX_FIFO0_MSG_PENDING |
                                           CAN_IT_RX_FIFO1_MSG_PENDING |
                                           CAN_IT_ERROR | CAN_IT_BUSOFF);

    /* CAN2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(CAN1_NVIC_RX0_IRQ);
    HAL_NVIC_DisableIRQ(CAN1_NVIC_RX1_IRQ);
    HAL_NVIC_DisableIRQ(CAN1_NVIC_SCE_IRQ);
    HAL_NVIC_DisableIRQ(CAN1_NVIC_TX_IRQ);
  }
#endif
}

// can 应用层 初始化
void CAN_Init(CAN_HandleTypeDef* hcan) {
  assert_param(hcan != NULL);
  // 启动can
  if (HAL_CAN_Start(hcan) != HAL_OK) Error_Handler();

  // 开启发送中断
  if (HAL_CAN_ActivateNotification(hcan, CAN_IT_TX_MAILBOX_EMPTY) != HAL_OK)
    Error_Handler();

  // 激活fifo0和fifo1的接收中断
  if (HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
    Error_Handler();
  if (HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO1_MSG_PENDING) != HAL_OK)
    Error_Handler();

  // 错误监测：激活can错误中断
  if (HAL_CAN_ActivateNotification(hcan, CAN_IT_ERROR) != HAL_OK)
    Error_Handler();
  if (HAL_CAN_ActivateNotification(hcan, CAN_IT_BUSOFF) != HAL_OK)
    Error_Handler();

  // 一次配置好发送的帧信息，减少后续赋值开销
  CAN_TxHeader_Init();
}

__weak void CAN_TxHeader_Init(void) {
  /* Note that CAN0 and CAN1 are two separate buses */
  CAN0_TxHeader.IDE = CAN_ID_STD;
  CAN0_TxHeader.RTR = CAN_RTR_DATA;
  CAN0_TxHeader.DLC = 8;
  CAN0_TxHeader.TransmitGlobalTime = DISABLE;

#ifndef ONLY_CAN0
  CAN1_TxHeader.IDE = CAN_ID_EXT;
  CAN1_TxHeader.RTR = CAN_RTR_DATA;
  CAN1_TxHeader.DLC = 8;
  CAN1_TxHeader.TransmitGlobalTime = DISABLE;
#endif
}

void configWatchdog(float timeout, bool ifCan1) {
  if (ifCan1 != true) {
    can0_watchdog_timeout = timeout;  // can0看门狗超时时间
  } else {
#ifndef ONLY_CAN0
    can1_watchdog_timeout = timeout;  // can1看门狗超时时间
#endif
  }
}

__weak void CAN0_watchdogCallback(void) {}

__weak void CAN1_watchdogCallback(void) {}

__weak void CAN0_RxCpltCallback(CAN_HandleTypeDef* hcan,
                                CAN_RxHeaderTypeDef* temp_rxheader,
                                const uint8_t* data) {}

__weak void CAN1_RxCpltCallback(CAN_HandleTypeDef* hcan,
                                CAN_RxHeaderTypeDef* temp_rxheader,
                                const uint8_t* data) {}

// can过滤器配置
void CAN_Filter_Init(CAN_HandleTypeDef* hcan, uint8_t object_para, uint32_t Id,
                     uint32_t MaskId) {
  CAN_FilterTypeDef CAN_FilterInitStructure;
  /* Check the parameters */
  assert_param(hcan != NULL);

  /* 根据对应的帧设置不同过滤方式 */
  /* Communication frame */
  if ((object_para & 0x02)) /*拓展帧or标准帧*/
  {
    /* 对扩展帧操作 */
    CAN_FilterInitStructure.FilterIdHigh = Id << 3
                                              << 16; /* 掩码后ID的高16bit */
    CAN_FilterInitStructure.FilterIdLow =
        Id << 3 | ((object_para & 0x03) << 1); /* 掩码后ID的低16bit */
    CAN_FilterInitStructure.FilterMaskIdHigh = MaskId
                                               << 3 << 16; /* ID掩码值高16bit */
    CAN_FilterInitStructure.FilterMaskIdLow =
        MaskId << 3 | ((object_para & 0x03) << 1);
    ;    /* ID掩码值低16bit */
  } else /* Other frame */
  {
    /* 对标准帧操作 */
    CAN_FilterInitStructure.FilterIdHigh = Id << 5; /* 掩码后ID的高16bit */
    CAN_FilterInitStructure.FilterIdLow =
        ((object_para & 0x03) << 1); /* 掩码后ID的低16bit */
    CAN_FilterInitStructure.FilterMaskIdHigh = MaskId
                                               << 5; /* ID掩码值高16bit */
    CAN_FilterInitStructure.FilterMaskIdLow = ((object_para & 0x03) << 1);
    ; /* ID掩码值低16bit */
  }

  CAN_FilterInitStructure.FilterBank = object_para >> 3; /* 滤波器组序号*/
  CAN_FilterInitStructure.FilterFIFOAssignment =
      (object_para >> 2) & 0x01;                     /* 滤波器绑定FIFO 0 */
  CAN_FilterInitStructure.FilterActivation = ENABLE; /* 使能滤波器 */
  CAN_FilterInitStructure.FilterMode =
      CAN_FILTERMODE_IDMASK; /* 滤波器模式，设置ID掩码模式 */
  CAN_FilterInitStructure.FilterScale = CAN_FILTERSCALE_32BIT; /* 32位滤波 */
  CAN_FilterInitStructure.SlaveStartFilterBank =
      14; /* 过滤器开始组别，单can芯片无意义 */

  if (HAL_CAN_ConfigFilter(hcan, &CAN_FilterInitStructure) != HAL_OK) {
    /* Filter configuration Error */
    Error_Handler();
  }
}

// Rxfifo0 回调
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan) {
  CAN_RxHeaderTypeDef temp_rxheader;
  uint8_t temp_rxdata[8] = {0};
  if (hcan == &CAN0_Handle) {
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &temp_rxheader, temp_rxdata) ==
        HAL_ERROR) {
    };
    CAN0_RxCpltCallback(hcan, &temp_rxheader, temp_rxdata);
    cur_can0_rx_time = DWT_GetTimeline_ms();  // 更新can0接收时间
    if (cur_can0_rx_time - last_can0_rx_time > can0_watchdog_timeout) {
      // 如果超过看门狗超时时间，则触发看门狗回调
      if (start_can0_watchdog) {
        CAN0_watchdogCallback();
      }
    }
    last_can0_rx_time = cur_can0_rx_time;  // 更新上次接收时间
  }
#ifndef ONLY_CAN0
  else if (hcan == &CAN1_Handle) {
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &temp_rxheader, temp_rxdata) ==
        HAL_ERROR) {
    };
    CAN1_RxCpltCallback(hcan, &temp_rxheader, temp_rxdata);
    cur_can1_rx_time = DWT_GetTimeline_ms();  // 更新can1接收时间
    if (cur_can1_rx_time - last_can1_rx_time > can1_watchdog_timeout) {
      // 如果超过看门狗超时时间，则触发看门狗回调
      if (start_can1_watchdog) {
        CAN1_watchdogCallback();
      }
    }
    last_can1_rx_time = cur_can1_rx_time;  // 更新上次接收时间
  }
#endif
}

// Rxfifo1 回调
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef* hcan) {
  CAN_RxHeaderTypeDef temp_rxheader;
  uint8_t temp_rxdata[8] = {0};

  if (hcan == &CAN0_Handle) {
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO1, &temp_rxheader, temp_rxdata) ==
        HAL_ERROR) {
    };
    CAN0_RxCpltCallback(hcan, &temp_rxheader, temp_rxdata);
    cur_can0_rx_time = DWT_GetTimeline_ms();  // 更新can0接收时间
    if (cur_can0_rx_time - last_can0_rx_time > can0_watchdog_timeout) {
      // 如果超过看门狗超时时间，则触发看门狗回调
      if (start_can0_watchdog) {
        CAN0_watchdogCallback();
      }
    }
    last_can0_rx_time = cur_can0_rx_time;  // 更新上次接收时间
  }
#ifndef ONLY_CAN0
  else if (hcan == &CAN1_Handle) {
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO1, &temp_rxheader, temp_rxdata) ==
        HAL_ERROR) {
    };
    CAN1_RxCpltCallback(hcan, &temp_rxheader, temp_rxdata);
    cur_can1_rx_time = DWT_GetTimeline_ms();  // 更新can1接收时间
    if (cur_can1_rx_time - last_can1_rx_time > can1_watchdog_timeout) {
      // 如果超过看门狗超时时间，则触发看门狗回调
      if (start_can1_watchdog) {
        CAN1_watchdogCallback();
      }
    }
    last_can1_rx_time = cur_can1_rx_time;  // 更新上次接收时间
  }
#endif
}

uint32_t tx1_error = 0;
uint32_t tx2_error = 0;
uint32_t tx0_error = 0;
uint32_t timeout_error = 0;
uint32_t alst_error = 0;
uint32_t relive_times = 0;  // 重启次数
uint32_t ack_error = 0;     // ack错误次数
uint32_t in_error = 0;
void HAL_CAN_ErrorCallback(CAN_HandleTypeDef* hcan) {
  // todo: 可以在这里记录日志到什么地方？然后事后可以读取日志

  uint32_t error_code = hcan->ErrorCode;

  if (error_code & HAL_CAN_ERROR_BOF) {
    // bus-off Error，直接重启can外设
    HAL_CAN_Stop(hcan);
    HAL_CAN_DeInit(hcan);
    CAN_Configuration(hcan);
    CAN_Init(hcan);
    if (hcan == &CAN0_Handle) {
      CAN_Filter_Init(hcan, CanFilter_0 | CanFifo_0 | Can_STDID | Can_DataType,
                      0, 0);
      CAN_Filter_Init(hcan, CanFilter_1 | CanFifo_1 | Can_STDID | Can_DataType,
                      0, 0);
    } else {
#ifndef ONLY_CAN0
      CAN_Filter_Init(hcan, CanFilter_14 | CanFifo_0 | Can_EXTID | Can_DataType,
                      0, 0);
      CAN_Filter_Init(hcan, CanFilter_15 | CanFifo_1 | Can_EXTID | Can_DataType,
                      0, 0);
#endif
    }
    relive_times++;
  }
  if (error_code & HAL_CAN_ERROR_TX_TERR0) {
    tx0_error++;
  }
  if (error_code & HAL_CAN_ERROR_TX_TERR1) {
    tx1_error++;
  }
  if (error_code & HAL_CAN_ERROR_TX_TERR2) {
    tx2_error++;
  }
  if (error_code & HAL_CAN_ERROR_TX_ALST0 ||
      error_code & HAL_CAN_ERROR_TX_ALST1 ||
      error_code & HAL_CAN_ERROR_TX_ALST2) {
    alst_error++;
  }
  if (error_code & HAL_CAN_ERROR_ACK) {
    ack_error++;
  }
  in_error++;
}

// 重启can
void CAN_AppRestart(CAN_HandleTypeDef* hcan) {
  HAL_CAN_Stop(hcan);
  HAL_CAN_DeInit(hcan);
  CAN_Configuration(hcan);
  CAN_Init(hcan);
  if (hcan == &CAN0_Handle) {
    CAN_Filter_Init(hcan, CanFilter_0 | CanFifo_0 | Can_STDID | Can_DataType, 0,
                    0);
    CAN_Filter_Init(hcan, CanFilter_1 | CanFifo_1 | Can_STDID | Can_DataType, 0,
                    0);
  } else {
#ifndef ONLY_CAN0
    CAN_Filter_Init(hcan, CanFilter_14 | CanFifo_0 | Can_EXTID | Can_DataType,
                    0, 0);
    CAN_Filter_Init(hcan, CanFilter_15 | CanFifo_1 | Can_EXTID | Can_DataType,
                    0, 0);
  }
#endif
}

#elif defined STM32H7

/* wait for developing ... */

#endif  // STM32H7
