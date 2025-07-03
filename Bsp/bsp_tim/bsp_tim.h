/**
 * @file bsp_tim.h
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
#ifndef BSP_TIM_H
#define BSP_TIM_H

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------include-----------------------------------*/
#include "esc_config.h"
#include "stm32f4xx_hal.h"

/*-----------------------------------macro------------------------------------*/

/*----------------------------------typedef-----------------------------------*/

/*----------------------------------variable----------------------------------*/

/*-------------------------------------os-------------------------------------*/

/*----------------------------------function----------------------------------*/
void TIM_Configuration(void);
void CAN0TASK_TIM_Configuration(void);
void CAN1TASK_TIM_Configuration(void);

void StartTimIT(void);

/*------------------------------------test------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* BSP_TIM_H */
