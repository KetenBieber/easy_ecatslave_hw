/**
 * @file bsp_uart.c
 * @author Keten (2863861004@qq.com)
 * @brief 满血串口bsp封装
 * @version 0.1
 * @date 2025-07-15
 *
 * @copyright Copyright (c) 2025
 *
 * @attention :
 * @note : 启用dma时，会自动开启半满终端，需要自己重载
 * HAL_UART_RxHalfCpltCallback函数 不启用dma时，使用中断模式会触发 接收满终端
 * HAL_UART_RxCpltCallback
 * @versioninfo :
 */
#include "bsp_uart.h"

#include <stdbool.h>

#include "bsp_bitband.h"

UART_HandleTypeDef huart6;
DMA_HandleTypeDef hdma_usart6_rx;
DMA_HandleTypeDef hdma_usart6_tx;

// 全局维护一张串口设备表，就可以实现动态注册
static BSP_UART_Instance_t* uart_device_map[MAX_UART_DEVICE_NUM];
// 每路 UART 的“正在发送”状态
static bool tx_busy[MAX_UART_DEVICE_NUM] = {0};

static inline int BSP_UART_Transmit(BSP_UART_Instance_t* uart_instance,
                                    uint16_t len);

static inline void RS485_DE_Ctrl(char p, uint32_t pin, uint8_t cmd);

static inline int get_uart_index(BSP_UART_Instance_t* inst) {
  for (int i = 0; i < MAX_UART_DEVICE_NUM; i++) {
    if (uart_device_map[i] != inst) continue;
    return i;
  }
  // 没有注册
  return -1;
}

void Uart_Configuration(void) {
  // uart 初始化

  // 编码器
  ENCODER_UART.Instance = ENCODER_UART_INSTANCE;
  ENCODER_UART.Init.BaudRate = ENCODER_UART_Baudrate;
  ENCODER_UART.Init.WordLength = UART_WORDLENGTH_8B;
  ENCODER_UART.Init.StopBits = UART_STOPBITS_1;
  ENCODER_UART.Init.Parity = UART_PARITY_NONE;
  ENCODER_UART.Init.Mode = UART_MODE_TX_RX;
  ENCODER_UART.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  ENCODER_UART.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&ENCODER_UART) != HAL_OK) {
    Error_Handler();
  }
}

// DMA需要放置在UART初始化之前
void DMA_Configuration(void) { ENCODER_DMA_RCC_CLK_ENABLE; }

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if (uartHandle->Instance == ENCODER_UART_INSTANCE) {
    // 使能串口时钟
    ENCODER_UART_RCC_CLK_ENABLE;
    // 使能GPIO时钟
    ENCODER_UART_GPIO_CLK_ENABLE;

    ENCODER_DMA_RCC_CLK_ENABLE;

    GPIO_InitStruct.Pin = ENCODER_UART_Tx_Pin | ENCODER_UART_Rx_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_USART6;
    HAL_GPIO_Init(ENCODER_UART_GPIO_Port, &GPIO_InitStruct);

    // init the dma rx
    ENCODER_DMA_Rx_Handle.Instance = ENCODER_DMA_RX_Instance;
    ENCODER_DMA_Rx_Handle.Init.Channel = ENCODER_DMA_Channel;
    ENCODER_DMA_Rx_Handle.Init.Direction = DMA_PERIPH_TO_MEMORY;
    ENCODER_DMA_Rx_Handle.Init.PeriphInc = DMA_PINC_DISABLE;
    ENCODER_DMA_Rx_Handle.Init.MemInc = DMA_MINC_ENABLE;
    ENCODER_DMA_Rx_Handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    ENCODER_DMA_Rx_Handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    ENCODER_DMA_Rx_Handle.Init.Mode = DMA_CIRCULAR;
    ENCODER_DMA_Rx_Handle.Init.Priority = DMA_PRIORITY_LOW;
    ENCODER_DMA_Rx_Handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;  // 禁用FIFO模式
    if (HAL_DMA_Init(&ENCODER_DMA_Rx_Handle) != HAL_OK) {
      Error_Handler();
    }
    __HAL_LINKDMA(uartHandle, hdmarx, ENCODER_DMA_Rx_Handle);

    // init the dma tx
    ENCODER_DMA_Tx_Handle.Instance = ENCODER_DMA_TX_Instance;
    ENCODER_DMA_Tx_Handle.Init.Channel = ENCODER_DMA_Channel;
    ENCODER_DMA_Tx_Handle.Init.Direction = DMA_MEMORY_TO_PERIPH;
    ENCODER_DMA_Tx_Handle.Init.PeriphInc = DMA_PINC_DISABLE;
    ENCODER_DMA_Tx_Handle.Init.MemInc = DMA_MINC_ENABLE;
    ENCODER_DMA_Tx_Handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    ENCODER_DMA_Tx_Handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    ENCODER_DMA_Tx_Handle.Init.Mode = DMA_NORMAL;  // 使用普通模式
    ENCODER_DMA_Tx_Handle.Init.Priority = DMA_PRIORITY_LOW;
    ENCODER_DMA_Tx_Handle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;  // 禁用FIFO模式
    if (HAL_DMA_Init(&ENCODER_DMA_Tx_Handle) != HAL_OK) {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle, hdmatx, ENCODER_DMA_Tx_Handle);

    // interrupt init

    HAL_NVIC_SetPriority(ENCODER_UART_NVIC_IRQ, 1, 0);
    HAL_NVIC_EnableIRQ(ENCODER_UART_NVIC_IRQ);  // 使能串口中断

    // init dma interrupt
    NVIC_SetPriority(ENCODER_DMA_Tx_STREAM_IRQn,
                     NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_EnableIRQ(ENCODER_DMA_Tx_STREAM_IRQn);

    // 打开半满和完成中断
    __HAL_DMA_ENABLE_IT(&ENCODER_DMA_Rx_Handle, DMA_IT_TC);
    __HAL_DMA_ENABLE_IT(&ENCODER_DMA_Rx_Handle, DMA_IT_HT);

    NVIC_SetPriority(ENCODER_DMA_Rx_STREAM_IRQn,
                     NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    NVIC_EnableIRQ(ENCODER_DMA_Rx_STREAM_IRQn);
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle) {
  if (uartHandle->Instance == ENCODER_UART_INSTANCE) {
    // 关闭串口时钟
    ENCODER_UART_RCC_CLK_DISABLE;

    // DeInit GPIO
    HAL_GPIO_DeInit(ENCODER_UART_GPIO_Port,
                    ENCODER_UART_Tx_Pin | ENCODER_UART_Rx_Pin);

    // DeInit DMA
    HAL_DMA_DeInit(uartHandle->hdmarx);
    HAL_DMA_DeInit(uartHandle->hdmatx);

    // DeInit interrupt
    HAL_NVIC_DisableIRQ(ENCODER_UART_NVIC_IRQ);
  }
}

int BSP_Uart_Init(BSP_UART_Instance_t* h) {
  assert_param(h != NULL);
  assert_param(h->dma_buf != NULL);
  assert_param(h->dma_buf_size > 0);
  if (h->mode != OnlyReceive) assert_param(h->tx_dma_buf != NULL);

  for (int i = 0; i < MAX_UART_DEVICE_NUM; i++) {
    if (uart_device_map[i] == NULL) {
      uart_device_map[i] = h;  // 找到空闲位置，注册串口设备
      if (h->mode != OnlyReceive) tx_busy[i] = false;
      break;
    } else if (uart_device_map[i] == h) {
      return -1;  // 已经注册过了
    } else if (i == MAX_UART_DEVICE_NUM - 1) {
      return -1;  // 没有空闲位置了
    }
  }
  // 初始化环形队列
  BSP_UART_RxQueueType_init(h->rx_queue);
  if (h->mode != OnlyReceive) BSP_UART_TxQueueType_init(h->tx_queue);

  h->last_pos = 0;  // 初始化上次位置为0
  h->hdma_rx = h->huart->hdmarx;
  if (h->mode != OnlyReceive) h->hdma_tx = h->huart->hdmatx;
  // 如果是rs485，则注意引脚需要在串口注册前初始化，这里不进行杂糅
  if (h->rs485_enable) {
    // 拉低DE电平
    RS485_DE_Ctrl(h->gpio_port, h->DE_Pin, 0);
  }

  __HAL_UART_ENABLE_IT(h->huart, UART_IT_IDLE);  // 启用IDLE中断

  // 将buf 挂载,buf 会被挂载到huart->pRxBuffPtr
  HAL_UART_Receive_DMA(h->huart, h->dma_buf,
                       h->dma_buf_size);  // 启用DMA接收

  return 0;  // 成功注册
}

static inline void RS485_DE_Ctrl(char p, uint32_t pin, uint8_t cmd) {
  switch (p) {
    case 'A': {
      PAout(pin) = cmd;
      break;
    }
    case 'B': {
      PBout(pin) = cmd;
      break;
    }
    case 'C': {
      PCout(pin) = cmd;
      break;
    }
    case 'D': {
      PDout(pin) = cmd;
      break;
    }
    case 'E': {
      PEout(pin) = cmd;
      break;
    }
    case 'F': {
      PFout(pin) = cmd;
      break;
    }
    case 'G': {
      PGout(pin) = cmd;
      break;
    }
    case 'H': {
      PHout(pin) = cmd;
      break;
    }
  }
}

// 重载串口函数
void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart) {
  UART_RX_ISR_Handler(huart);  // 处理接收完成事件
}
// 重载串口半满函数
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef* huart) {
  UART_RX_ISR_Handler(huart);  // 处理半满事件
}

// 这个函数需要放在每个启用的HAL_UART_IRQHandler函数中调用
void HAL_UART_IDLE_Callback(UART_HandleTypeDef* huart) {
  // 处理IDLE事件
  if (__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE)) {
    __HAL_UART_CLEAR_IDLEFLAG(huart);

    UART_RX_ISR_Handler(huart);
  }
}

void UART_RX_ISR_Handler(UART_HandleTypeDef* huart) {
  for (int i = 0; i < MAX_UART_DEVICE_NUM; i++) {
    if (uart_device_map[i] == NULL) continue;  // 跳过空闲位置
    if (uart_device_map[i]->huart == huart) {
      // 找到对应的串口设备
      BSP_UART_Instance_t* uart_instance = uart_device_map[i];

      // 必要检查
      assert_param(uart_instance != NULL);
      assert_param(uart_instance->dma_buf != NULL);

      uint16_t dma_size = uart_instance->dma_buf_size;

      uint16_t curr_pos =
          dma_size - __HAL_DMA_GET_COUNTER(uart_instance->hdma_rx);
      uint16_t last_pos = uart_instance->last_pos;

      if (curr_pos != last_pos) {
        uint16_t cnt = 0;
        if (curr_pos > last_pos) {
          // 线性接收
          cnt = curr_pos - last_pos;
          for (int i = 0; i < cnt; i++) {
            uint8_t b = uart_instance->dma_buf[last_pos + i];
            BSP_UART_RxQueueType_enqueue(uart_instance->rx_queue, b);
          }

        } else {
          // 回卷接收
          cnt = dma_size - last_pos;
          for (int i = 0; i < cnt; i++) {
            uint8_t b = uart_instance->dma_buf[last_pos + i];
            BSP_UART_RxQueueType_enqueue(uart_instance->rx_queue, b);
          }
          for (int i = 0; i < curr_pos; i++) {
            uint8_t b = uart_instance->dma_buf[i];
            BSP_UART_RxQueueType_enqueue(uart_instance->rx_queue, b);
          }
          cnt += curr_pos;
        }
        uart_instance->last_pos = curr_pos;
        if (uart_instance->IdleCallback != NULL) {
          uart_instance->IdleCallback(uart_instance, cnt);
        }
      }

      return;  // 找到后退出
    }
  }
}

// 重载串口发送完成函数
void HAL_UART_TxCpltCallback(UART_HandleTypeDef* huart) {
  for (int i = 0; i < MAX_UART_DEVICE_NUM; i++) {
    BSP_UART_Instance_t* inst = uart_device_map[i];
    if (inst == NULL || inst->huart != huart) continue;

    // RS-485 发送完成
    if (inst->rs485_enable) {
      // 拉低回到接受模式
      RS485_DE_Ctrl(inst->gpio_port, inst->DE_Pin, 0);
    }

    // 队列还有数据就再发一批
    int avail = BSP_UART_TxQueueType_get_length(inst->tx_queue);
    if (avail > 0) {
      BSP_UART_Transmit(inst, avail);
    } else {
      tx_busy[i] = false;
    }
    return;
  }
}

int BSP_UART_SendData(BSP_UART_Instance_t* inst, const uint8_t* data,
                      uint16_t len) {
  assert_param(inst && data && len > 0);
  for (uint16_t i = 0; i < len; i++) {
    if (BSP_UART_TxQueueType_enqueue(inst->tx_queue, data[i]) != 0)
      break;  // 队列满,后续字节丢弃
  }

  int idx = get_uart_index(inst);
  if (!tx_busy
          [idx]) {  // 如果此事并非正忙状态,则调用Transmit激活串口,并且设置为正忙
    tx_busy[idx] = true;
    uint16_t avail = BSP_UART_TxQueueType_get_length(inst->tx_queue);
    // 激活发送
    return BSP_UART_Transmit(inst, avail);
  }
  // 已经开启发送状态,入队
  return 0;
}

static inline int BSP_UART_Transmit(BSP_UART_Instance_t* uart_instance,
                                    uint16_t len) {
  assert_param(uart_instance != NULL);

  int avail = BSP_UART_TxQueueType_get_length(uart_instance->tx_queue);
  int de_queue = (len < avail) ? len : avail;

  if (avail <= 0) return -1;

  // 使用静态缓冲区
  for (int i = 0; i < de_queue; i++) {
    BSP_UART_TxQueueType_dequeue(uart_instance->tx_queue,
                                 &uart_instance->tx_dma_buf[i]);
  }

  if (uart_instance->rs485_enable) {
    // 拉高准备发送
    RS485_DE_Ctrl(uart_instance->gpio_port, uart_instance->DE_Pin, 1);
  }

  return HAL_UART_Transmit_DMA(uart_instance->huart, uart_instance->tx_dma_buf,
                               de_queue) == HAL_OK
             ? 0
             : -1;
}

// 错误回调处理:当UART 发生错误时,HAL_UART_ErrorCallback将会被调用
void HAL_UART_ErrorCallback(UART_HandleTypeDef* huart) {
  // 立即终止当前串口传输 或 接收操作
  HAL_UART_Abort_IT(huart);
}

// 中止完成回调
void HAL_UART_AbortCpltCallback(UART_HandleTypeDef* huart) {
  for (int i = 0; i < MAX_UART_DEVICE_NUM; i++) {
    BSP_UART_Instance_t* inst = uart_device_map[i];
    if (inst == NULL || inst->huart != huart) continue;
    HAL_UART_Receive_DMA(huart, huart->pRxBuffPtr, inst->dma_buf_size);
    inst->last_pos = 0;
  }
}

void HAL_UART_AbortTransmitCpltCallback(UART_HandleTypeDef* huart) {
  for (int i = 0; i < MAX_UART_DEVICE_NUM; i++) {
    BSP_UART_Instance_t* inst = uart_device_map[i];
    if (inst == NULL || inst->huart != huart) continue;
    // 做一些操作,提醒着一次发送失败了
  }
}

// 接收中止完成回调
void HAL_UART_AbortReceiveCpltCallback(UART_HandleTypeDef* huart) {
  for (int i = 0; i < MAX_UART_DEVICE_NUM; i++) {
    BSP_UART_Instance_t* inst = uart_device_map[i];
    if (inst == NULL || inst->huart != huart) continue;

    // 重新开启接收(由于我们已经完成了挂在缓存区的操作)
    HAL_UART_Receive_DMA(huart, huart->pRxBuffPtr,
                         inst->dma_buf_size);  // 重新开启DMA接收
  }
}