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
 * @versioninfo :
 */
#include "bsp_can.h"

#if defined STM32F4

/* global */
/* 发送句柄 */
CAN_TxHeaderTypeDef CAN0_TxHeader;
CAN_TxHeaderTypeDef CAN1_TxHeader;

#ifdef BACKEND_PROCESS_CANDATA  // 使用中断发送信号量，MainLoop处理函数中使用
uint8_t _can0_fifo0_irq = 0;
uint8_t _can1_fifo0_irq = 0;
uint8_t _can0_fifo1_irq = 0;
uint8_t _can1_fifo1_irq = 0;
#endif

// can hal层 初始化
void CAN_Configuration(void) {
  CAN0_Handle.Instance = CAN0_Instance;
  CAN0_Handle.Init.Prescaler = 3;
  CAN0_Handle.Init.Mode = CAN_MODE_NORMAL;
  CAN0_Handle.Init.SyncJumpWidth = CAN_SJW_1TQ;
  CAN0_Handle.Init.TimeSeg1 = CAN_BS1_9TQ;
  CAN0_Handle.Init.TimeSeg2 = CAN_BS2_4TQ;
  CAN0_Handle.Init.TimeTriggeredMode = DISABLE;
  CAN0_Handle.Init.AutoBusOff = DISABLE;
  CAN0_Handle.Init.AutoWakeUp = DISABLE;
  CAN0_Handle.Init.AutoRetransmission = DISABLE;
  CAN0_Handle.Init.ReceiveFifoLocked = DISABLE;
  CAN0_Handle.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&CAN0_Handle) != HAL_OK) {
    Error_Handler();
  }
#ifndef ONLY_CAN0
  CAN1_Handle.Instance = CAN1_Instance;
  CAN1_Handle.Init.Prescaler = 3;
  CAN1_Handle.Init.Mode = CAN_MODE_NORMAL;
  CAN1_Handle.Init.SyncJumpWidth = CAN_SJW_1TQ;
  CAN1_Handle.Init.TimeSeg1 = CAN_BS1_9TQ;
  CAN1_Handle.Init.TimeSeg2 = CAN_BS2_4TQ;
  CAN1_Handle.Init.TimeTriggeredMode = DISABLE;
  CAN1_Handle.Init.AutoBusOff = DISABLE;
  CAN1_Handle.Init.AutoWakeUp = DISABLE;
  CAN1_Handle.Init.AutoRetransmission = DISABLE;
  CAN1_Handle.Init.ReceiveFifoLocked = DISABLE;
  CAN1_Handle.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&CAN1_Handle) != HAL_OK) {
    Error_Handler();
  }
#endif
}

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
    HAL_NVIC_SetPriority(CAN0_NVIC_RX0_IRQ, 2, 1);
    HAL_NVIC_EnableIRQ(CAN0_NVIC_RX0_IRQ);
    HAL_NVIC_SetPriority(CAN0_NVIC_RX1_IRQ, 2, 1);
    HAL_NVIC_EnableIRQ(CAN0_NVIC_RX1_IRQ);

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
    HAL_NVIC_SetPriority(CAN1_NVIC_RX0_IRQ, 2, 1);
    HAL_NVIC_EnableIRQ(CAN1_NVIC_RX0_IRQ);
    HAL_NVIC_SetPriority(CAN1_NVIC_RX1_IRQ, 2, 1);
    HAL_NVIC_EnableIRQ(CAN1_NVIC_RX1_IRQ);
  }
#endif
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle) {
  if (canHandle->Instance == CAN0_Instance) {
    CAN0_RCC_CLK_DISABLE;

    /**CAN1 GPIO Configuration
    PB8     ------> CAN1_RX
    PB9     ------> CAN1_TX
    */
    HAL_GPIO_DeInit(CAN0_IO_PORT, CAN0_Tx_Pin | CAN0_Rx_Pin);

    /* CAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(CAN0_NVIC_RX0_IRQ);
    HAL_NVIC_DisableIRQ(CAN0_NVIC_RX1_IRQ);
  }
#ifndef ONLY_CAN0
  else if (canHandle->Instance == CAN1_Instance) {
    CAN1_RCC_CLK_DISABLE;
    HAL_GPIO_DeInit(CAN1_IO_PORT, CAN1_Tx_Pin | CAN1_Rx_Pin);

    /* CAN2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(CAN1_NVIC_RX0_IRQ);
    HAL_NVIC_DisableIRQ(CAN1_NVIC_RX1_IRQ);
  }
#endif
}

// can 应用层 初始化
void CAN_Init(CAN_HandleTypeDef* hcan) {
  assert_param(hcan != NULL);
  if (HAL_CAN_Start(hcan) != HAL_OK) Error_Handler();
  if (HAL_CAN_ActivateNotification(hcan, CAN_IT_TX_MAILBOX_EMPTY) != HAL_OK)
    Error_Handler();
  if (HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
    Error_Handler();
  if (HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO1_MSG_PENDING) != HAL_OK)
    Error_Handler();
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

__weak void pCAN0_RxCpltCallback(CAN_HandleTypeDef* hcan,
                                 CAN_RxHeaderTypeDef* temp_rxheader,
                                 const uint8_t* data) {}

__weak void pCAN1_RxCpltCallback(CAN_HandleTypeDef* hcan,
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
#ifndef BACKEND_PROCESS_CANDATA
  CAN_RxHeaderTypeDef temp_rxheader;
  uint8_t temp_rxdata[8] = {0};
  if (hcan == &CAN0_Handle) {
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &temp_rxheader, temp_rxdata) ==
        HAL_ERROR) {
    };
    pCAN0_RxCpltCallback(hcan, &temp_rxheader, temp_rxdata);
  }
#ifndef ONLY_CAN0
  else if (hcan == &CAN1_Handle) {
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &temp_rxheader, temp_rxdata) ==
        HAL_ERROR) {
    };
    pCAN1_RxCpltCallback(hcan, &temp_rxheader, temp_rxdata);
  }
#endif
#else
  if (hcan == &CAN0_Handle) {
    _can0_fifo0_irq = 1;
  }
#ifndef ONLY_CAN0
  else if (hcan == &CAN1_Handle) {
    _can1_fifo0_irq = 1;
  }
#endif

#endif
}

// Rxfifo1 回调
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef* hcan) {
#ifndef BACKEND_PROCESS_CANDATA
  CAN_RxHeaderTypeDef temp_rxheader;
  uint8_t temp_rxdata[8] = {0};

  if (hcan == &CAN0_Handle) {
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO1, &temp_rxheader, temp_rxdata) ==
        HAL_ERROR) {
    };
    pCAN0_RxCpltCallback(hcan, &temp_rxheader, temp_rxdata);
  }
#ifndef ONLY_CAN0
  else if (hcan == &CAN1_Handle) {
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO1, &temp_rxheader, temp_rxdata) ==
        HAL_ERROR) {
    };
    pCAN1_RxCpltCallback(hcan, &temp_rxheader, temp_rxdata);
  }
#endif
#else
  if (hcan == &CAN0_Handle) {
    _can0_fifo1_irq = 1;
  }
#ifndef ONLY_CAN0
  else if (hcan == &CAN1_Handle) {
    _can1_fifo1_irq = 1;
  }
#endif
#endif
}

// 使用方法，配好TxHeader的ID，然后把数据装填到buf里，发送
__weak void CAN_Transmit_ExtId(CAN_HandleTypeDef* hcan,
                               CAN_TxHeaderTypeDef* TxHeader, uint8_t data[],
                               int length) {}

__weak void CAN_Transmit_StdId(CAN_HandleTypeDef* hcan,
                               CAN_TxHeaderTypeDef* TxHeader, uint8_t data[],
                               int length) {}

// 重启can
void CAN_AppRestart(CAN_HandleTypeDef* hcan) {
  HAL_CAN_DeInit(hcan);
  CAN_Init(hcan);
}

#elif defined STM32H7

/* wait for developing ... */

#endif  // STM32H7
