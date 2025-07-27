/**
 * @file bsp_gpio.h
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
#ifndef BSP_GPIO_H
#define BSP_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------include-----------------------------------*/
#include "esc_config.h"
#include "stm32f4xx_hal.h"

/*----------------------------------function----------------------------------*/
void rst_setup(void);
void rst_low(void);
void rst_high(void);
void rst_check_start(void);
uint8_t is_esc_reset(void);

void PDI_IRQ_Configuration(void);

void SYNC0_Configuration(void);

void SYNC1_Configuration(void);

void LED_Configuration(void);

void Key_Configuration(void);

void EncoderRs485IO_DE_Configuration(void);

void ShooterIO_Configuration(void);

#ifdef __cplusplus
}
#endif

#endif /* BSP_GPIO_H */
