/**
 * @file bsp_init.h
 * @author Keten (2863861004@qq.com)
 * @brief 提供一个初始化bsp层的init函数
 * @version 0.1
 * @date 2025-04-13
 *
 * @copyright Copyright (c) 2025
 *
 * @attention :
 * @note :
 * @versioninfo :
 */
#ifndef BSP_INIT_H
#define BSP_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------include-----------------------------------*/

#include "bsp_can.h"
#include "bsp_dwt.h"
#include "bsp_gpio.h"
#include "bsp_spi.h"
#include "bsp_tim.h"

/*----------------------------------function----------------------------------*/
void BSP_Init(void) {
  /* 初始化esc 复位io */
  rst_setup();

  /* dwt 初始化，DEBUG调试版本使用，RELEASE版本取消 */
  DWT_Init(168);  // A板为180MHz, B板为168MHz, 这里使用A板的频率

  /* led外设io初始化 */
  /* esc本地时钟外设初始化 */
  TIM_Configuration();
  /* can发送任务定时器配置初始化 */
  CAN0TASK_TIM_Configuration();
  CAN1TASK_TIM_Configuration();
  /* 从站应用层外设初始化 */

  /* can总线硬件初始化 */
  CAN_Configuration(&CAN0_Handle);
  CAN_Configuration(&CAN1_Handle);

  /* 板上外设初始化 */
  LED_Configuration();
  // Key_Configuration();

  /* 光电门io初始化 */
  Photogate_Configuration();
  /* can 总线应用层初始化 */
  CAN_Init(&CAN0_Handle);
  CAN_Filter_Init(&CAN0_Handle,
                  CanFilter_0 | CanFifo_0 | Can_STDID | Can_DataType, 0, 0);
  CAN_Filter_Init(&CAN0_Handle,
                  CanFilter_1 | CanFifo_1 | Can_STDID | Can_DataType, 0, 0);

#ifndef ONLY_CAN0
  // 使用扩展帧
  CAN_Init(&CAN1_Handle);
  CAN_Filter_Init(&CAN1_Handle,
                  CanFilter_14 | CanFifo_0 | Can_EXTID | Can_DataType, 0, 0);
  CAN_Filter_Init(&CAN1_Handle,
                  CanFilter_15 | CanFifo_1 | Can_EXTID | Can_DataType, 0, 0);
#endif
}

#ifdef __cplusplus
}
#endif

#endif /* BSP_INIT_H */
