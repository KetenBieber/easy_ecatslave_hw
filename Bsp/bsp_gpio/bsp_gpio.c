/**
 * @file bsp_gpio.c
 * @author Keten (2863861004@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-04-09
 *
 * @copyright Copyright (c) 2025
 *
 * @attention : 2025-07-03 新板子设计，需要对gpio方面做修改
 * @note :
 * @versioninfo :
 */
#include "bsp_gpio.h"

/* rst relative */
void rst_setup(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* GPIO Ports Clock Enable */
  ESC_RESET_GPIO_CLK_ENABLE;

  HAL_GPIO_WritePin(ESC_RESET_GPIO_Port, ESC_RESET_Pin, GPIO_PIN_RESET);
  GPIO_InitStruct.Pin = ESC_RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(ESC_RESET_GPIO_Port, &GPIO_InitStruct);

  rst_high();
}

void rst_low(void) {
  /* Set RSTN line low */
  HAL_GPIO_WritePin(ESC_RESET_GPIO_Port, ESC_RESET_Pin, GPIO_PIN_RESET);
}

void rst_high(void) {
  HAL_GPIO_WritePin(ESC_RESET_GPIO_Port, ESC_RESET_Pin, GPIO_PIN_SET);
}

void rst_check_start(void) {
  /* Setup NRST as GPIO input and pull it high */
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  ESC_RESET_GPIO_CLK_ENABLE;
  GPIO_InitStruct.Pin = ESC_RESET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(ESC_RESET_GPIO_Port, &GPIO_InitStruct);

  rst_high();
}

uint8_t is_esc_reset(void) {
  return (HAL_GPIO_ReadPin(ESC_RESET_GPIO_Port, ESC_RESET_Pin) == GPIO_PIN_SET);
}

/* ESC IO relative */
void PDI_IRQ_Configuration(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* GPIO Ports Clock Enable */
  PDI_IRQ_GPIO_CLK_ENABLE;
  GPIO_InitStruct.Pin = PDI_IRQ_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(PDI_IRQ_GPIO_Port, &GPIO_InitStruct);

  HAL_NVIC_SetPriority(PDI_IRQ_EXTI_IRQn, 0, 0);
  // HAL_NVIC_EnableIRQ(PDI_IRQ_EXTI_IRQn);  // 不允许在此处使能
}

void PDI_IRQ_Disable(void) { HAL_NVIC_DisableIRQ(PDI_IRQ_EXTI_IRQn); }

void SYNC0_Configuration(void) {
  GPIO_InitTypeDef GPIO_InitStruct;
  /* GPIO Ports Clock Enable */
  SYNC0_IRQ_GPIO_CLK_ENABLE;
  GPIO_InitStruct.Pin = SYNC0_IRQ_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(SYNC0_IRQ_GPIO_Port, &GPIO_InitStruct);

  HAL_NVIC_SetPriority(SYNC0_IRQ_EXTI_IRQn, 0, 0);
  // HAL_NVIC_EnableIRQ(SYNC0_IRQ_EXTI_IRQn);  // 不允许在此处使能
}

void SYNC0_IRQ_Disable(void) { HAL_NVIC_DisableIRQ(SYNC0_IRQ_EXTI_IRQn); }

void SYNC1_Configuration(void) {
  GPIO_InitTypeDef GPIO_InitStruct;
  /* GPIO Ports Clock Enable */
  SYNC1_IRQ_GPIO_CLK_ENABLE;
  GPIO_InitStruct.Pin = SYNC1_IRQ_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(SYNC1_IRQ_GPIO_Port, &GPIO_InitStruct);

  HAL_NVIC_SetPriority(SYNC1_IRQ_EXTI_IRQn, 0, 0);
  // HAL_NVIC_EnableIRQ(SYNC1_IRQ_EXTI_IRQn);  // 不允许在此处使能
}

void SYNC1_IRQ_Disable(void) { HAL_NVIC_DisableIRQ(SYNC1_IRQ_EXTI_IRQn); }

// 配置板上的 led
void LED_Configuration(void) {
  GPIO_InitTypeDef GPIO_InitStruct;
  /* GPIO Ports Clock Enable */
  LED_GPIO_CLK_ENABLE;
  HAL_GPIO_WritePin(GPIOC, LED1_Pin | LED2_Pin | LED3_Pin, GPIO_PIN_SET);
  GPIO_InitStruct.Pin = LED1_Pin | LED2_Pin | LED3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

// 配置板上的 按键
void Key_Configuration(void) {
  GPIO_InitTypeDef GPIO_InitStruct;
  /* GPIO Ports Clock Enable */
  KEY_GPIO_CLK_ENABLE;

  GPIO_InitStruct.Pin = KEY0_Pin | KEY1_Pin | KEY2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

// 配置光电门所用io --- 输入上拉
// 1 2 3 分别对应 D6 D7 G10，对应舵向front、left_rear、 right_rear的光电门
void Photogate_Configuration(void) {
  GPIO_InitTypeDef GPIO_InitStruct;
  /* GPIO Ports Clock Enable */
  PHOTOGATE_1_GPIO_CLK_ENABLE;
  PHOTOGATE_2_GPIO_CLK_ENABLE;
  PHOTOGATE_3_GPIO_CLK_ENABLE;
  PHOTOGATE_REFERENCE_GPIO_CLK_ENABLE;
  HAL_GPIO_WritePin(PHOTOGATE_REFERENCE_GPIO_Port, PHOTOGATE_REFERENCE_Pin,
                    GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = PHOTOGATE1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(PHOTOGATE1_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = PHOTOGATE2_Pin;
  HAL_GPIO_Init(PHOTOGATE2_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = PHOTOGATE3_Pin;
  HAL_GPIO_Init(PHOTOGATE3_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = PHOTOGATE_REFERENCE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;    // 设置参考光电门为输出模式
  GPIO_InitStruct.Pull = GPIO_PULLUP;            // 上拉
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  // 设置速度为高速
  HAL_GPIO_Init(PHOTOGATE_REFERENCE_GPIO_Port, &GPIO_InitStruct);
  HAL_GPIO_WritePin(PHOTOGATE_REFERENCE_GPIO_Port, PHOTOGATE_REFERENCE_Pin,
                    GPIO_PIN_RESET);  // 设置参考光电门为高电平
}

// 配置RS485的DE引脚
void EncoderRs485IO_DE_Configuration(void) {
  GPIO_InitTypeDef GPIO_InitTypeDef = {0};
  // 使能GPIO时钟
  HAL_GPIO_WritePin(ENCODER_RS485_DE_GPIO_Port, ENCODER_RS485_DE_Pin,
                    GPIO_PIN_RESET);
  GPIO_InitTypeDef.Pin = ENCODER_RS485_DE_Pin;
  GPIO_InitTypeDef.Mode = GPIO_MODE_OUTPUT_PP;    // 设置为推挽输出模式
  GPIO_InitTypeDef.Pull = GPIO_PULLUP;            // 上拉
  GPIO_InitTypeDef.Speed = GPIO_SPEED_FREQ_HIGH;  // 设置速度为高速
  HAL_GPIO_Init(ENCODER_RS485_DE_GPIO_Port, &GPIO_InitTypeDef);
  HAL_GPIO_WritePin(ENCODER_RS485_DE_GPIO_Port, ENCODER_RS485_DE_Pin,
                    GPIO_PIN_RESET);  // 默认拉低DE引脚
}

void ShooterIO_Configuration(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  // 拉低IO
  HAL_GPIO_WritePin(SHOOT_GPIO_Port, SHOOT_GPIO_Pin, SHOOT_INIT_STATE);

  GPIO_InitStruct.Pin = SHOOT_GPIO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;    // 设置为推挽输出模式
  GPIO_InitStruct.Pull = GPIO_PULLUP;            // 上拉
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;  // 设置速度为高速
  HAL_GPIO_Init(SHOOT_GPIO_Port, &GPIO_InitStruct);
  HAL_GPIO_WritePin(SHOOT_GPIO_Port, SHOOT_GPIO_Pin, SHOOT_INIT_STATE);
}