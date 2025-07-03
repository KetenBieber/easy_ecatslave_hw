/**
 * @file actuator_factor.h
 * @author Keten (2863861004@qq.com)
 * @brief 存放电机类型及其参数
 * @version 0.1
 * @date 2025-06-19
 *
 * @copyright Copyright (c) 2025
 *
 * @attention :
 * @note :
 * @versioninfo :
 */
#ifndef ACTUATOR_FACTOR_H
#define ACTUATOR_FACTOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/*-----------------------------------macro------------------------------------*/

#define DJI_MOTOR_TOTAL_ENCODER 8192              // 大疆电机编码器总值
#define PI 3.141592653f                           // 圆周率
#define DJI_ENCODER_ANGLE_RATIO 2 * PI / 8192.0f  // 大疆电机编码器角度比

/*----------------------------------typedef-----------------------------------*/

typedef union {
  float f;
  int16_t i16[2];
} FloatSplitter;

typedef struct {
  int16_t cur_encoder_;
  int16_t last_encoder_;
  int32_t round_cnt_;
  FloatSplitter angle_splitter_;
} DJI_Motor_Position_t;

/*----------------------------------variable----------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* ACTUATOR_FACTOR_H */
