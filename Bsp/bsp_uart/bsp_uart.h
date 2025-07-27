/**
 * @file bsp_uart.h
 * @author Keten (2863861004@qq.com)
 * @brief 满血串口bsp 封装 + rx485
 * @version 0.1
 * @date 2025-07-15
 *
 * @copyright Copyright (c) 2025
 *
 * @attention :
 * @note :
 * @versioninfo :
 */
#ifndef BSP_UART_H
#define BSP_UART_H

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------include-----------------------------------*/

#include <stdint.h>

#include "circular_queue.h"  // for circular queue
#include "esc_config.h"      // for UART_HandleTypeDef huart6
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart.h"


/*-----------------------------------macro------------------------------------*/

#define MAX_UART_DEVICE_NUM 6

#define BSP_UART_RX_QUEUE_SIZE 256
#define BSP_UART_TX_QUEUE_SIZE 256

DECLARE_CIRCULAR_QUEUE(BSP_UART_RxQueueType, uint8_t, BSP_UART_RX_QUEUE_SIZE)
DECLARE_CIRCULAR_QUEUE(BSP_UART_TxQueueType, uint8_t, BSP_UART_TX_QUEUE_SIZE)

/*----------------------------------typedef-----------------------------------*/
enum UartMode {
  OnlyReceive = 0,
  OnlySend = 1,
  BothReceiveAndSend = 2,
};

typedef struct BSP_UART_Instance_t {
  UART_HandleTypeDef *huart;
  uint8_t mode;                    //
  DMA_HandleTypeDef *hdma_rx;      // DMA接收句柄
  DMA_HandleTypeDef *hdma_tx;      // DMA发送句柄
  BSP_UART_RxQueueType *rx_queue;  // 接收队列
  BSP_UART_TxQueueType *tx_queue;  // 发送队列

  uint8_t rs485_enable;  // 0=普通 UART，1=RS-485
  char gpio_port;
  uint32_t DE_Pin;  // 驱动使能引脚号

  uint8_t *dma_buf;  // 专门给 DMA 的循环 buf
  uint16_t dma_buf_size;
  uint16_t last_pos;  // 上次搬数据到 app_queue 的位置

  uint8_t *tx_dma_buf;

  void (*IdleCallback)(struct BSP_UART_Instance_t *h,
                       uint16_t len);  // 用户自己装填
} BSP_UART_Instance_t;

/*----------------------------------variable----------------------------------*/
extern UART_HandleTypeDef huart6;

/*-------------------------------------os-------------------------------------*/

/*----------------------------------function----------------------------------*/
void HAL_UART_IDLE_Callback(UART_HandleTypeDef *huart);

int BSP_UART_SendData(BSP_UART_Instance_t *inst, const uint8_t *data,
                      uint16_t len);
/*------------------------------------test------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* BSP_UART_H */
