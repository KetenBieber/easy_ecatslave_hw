/**
 * @file encoder_app.h
 * @author Keten (2863861004@qq.com)
 * @brief 读取编码器
 * @version 0.1
 * @date 2025-07-15
 *
 * @copyright Copyright (c) 2025
 *
 * @attention :
 * @note :
 * @versioninfo :
 */
#ifndef ENCODER_APP_H
#define ENCODER_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------include-----------------------------------*/
#include <stdbool.h>

#include "actuator_factor.h"  // for FloatSpliter
#include "bsp_uart.h"

/*-----------------------------------macro------------------------------------*/

// 根据编码器实际参数设置：
// 如12M0064 ,即：12 是分辨率：2^12 = 4096
// 64表示多圈计数值，2^12 * 64 = 262144 = 2^18
// 因此可以算出编码器的最大值和半个周期
#define ENCODER_RESOLUTION 12                             // 分辨率
#define ENCODER_MULTITURN 64                              // 多圈计数值
#define ENCODER_CYCLE (1 << ENCODER_RESOLUTION)           // 周期值
#define ENCODER_MAX ENCODER_CYCLE *ENCODER_MULTITURN - 1  //
#define ENCODER_HALF (ENCODER_MAX + 1) / 2
#define ENCODER_FACTOR 0.01f
/*----------------------------------typedef-----------------------------------*/
typedef struct {
  uint32_t Encoder_last;
  uint32_t Encoder_curt;
  int32_t delta;
  float distance;
  FloatSplitter angle_splitter;  // 用于存储角度的分量
} EncoderData;
/*----------------------------------variable----------------------------------*/

/*-------------------------------------os-------------------------------------*/

/*----------------------------------function----------------------------------*/
void Encoder_Device_Init(void);

void Encoder_Uart_Idle_Callback(struct BSP_UART_Instance_t *h, uint16_t len);
int Encoder_GetData(uint8_t *buf, uint8_t len);
void test_RS485Send(void);
void handle_encoder_pos(EncoderData *encoder_data);
void Encoder_SetZero(void);

#ifdef __cplusplus
}
#endif

#endif /* ENCODER_APP_H */
