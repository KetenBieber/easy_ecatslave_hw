/**
 * @file bsp_spi.c
 * @author Keten (2863861004@qq.com)
 * @brief 板级spi包，封装esc上的spi操作，具体引脚可以在esc_config.h中修改
 * @version 0.1
 * @date 2025-04-09
 *
 * @copyright Copyright (c) 2025
 *
 * @attention :
 * @note :
 * @versioninfo :
 */
#include "bsp_spi.h"

void spi_setup(void) {
  /* SPI Instance relative */
  ESC_SPI.Instance = ESC_SPI_Instance;
  ESC_SPI.Init.Mode = SPI_MODE_MASTER;
  ESC_SPI.Init.Direction = SPI_DIRECTION_2LINES;
  ESC_SPI.Init.DataSize = SPI_DATASIZE_8BIT;
  ESC_SPI.Init.CLKPolarity = SPI_POLARITY_LOW;
  ESC_SPI.Init.CLKPhase = SPI_PHASE_1EDGE;
  ESC_SPI.Init.NSS = SPI_NSS_SOFT;
  ESC_SPI.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  ESC_SPI.Init.FirstBit = SPI_FIRSTBIT_MSB;
  ESC_SPI.Init.TIMode = SPI_TIMODE_DISABLE;
  ESC_SPI.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  ESC_SPI.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&ESC_SPI) !=
      HAL_OK) {  // 在这里面会去调用 spi.c 中的HAL_SPI_MspInit
    Error_Handler();
  }
}

void HAL_SPI_MspInit(SPI_HandleTypeDef *spiHandle) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if (spiHandle->Instance == ESC_SPI_Instance) {
    /* USER CODE BEGIN SPI1_MspInit 0 */

    /* USER CODE END SPI1_MspInit 0 */
    /* SPI1 clock enable */
    ESC_SPI_CLK_ENABLE;

    ESC_SPI_GPIO_CLK_ENABLE;
    /**SPI1 GPIO Configuration
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI
    PA8     ------> SPI3_CS
    */
    GPIO_InitStruct.Pin = FLASH_SPI_SCK_Pin | FLASH_SPI_MOSI_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = FLASH_SPI_MISO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(FLASH_SPI_MISO_GPIO_Port, &GPIO_InitStruct);

    HAL_GPIO_WritePin(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_Pin, GPIO_PIN_SET);
    GPIO_InitStruct.Pin = FLASH_SPI_CS_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(FLASH_SPI_CS_GPIO_Port, &GPIO_InitStruct);
  }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef *spiHandle) {
  if (spiHandle->Instance == ESC_SPI_Instance) {
    /* USER CODE BEGIN SPI1_MspDeInit 0 */

    /* USER CODE END SPI1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI1_CLK_DISABLE();

    /**SPI1 GPIO Configuration
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI
    */
    HAL_GPIO_DeInit(ESC_SPI_Port, FLASH_SPI_SCK_Pin | FLASH_SPI_MISO_Pin |
                                      FLASH_SPI_MOSI_Pin);

    /* USER CODE BEGIN SPI1_MspDeInit 1 */

    /* USER CODE END SPI1_MspDeInit 1 */
  }
}

// 开启片选
void spi_select(int8_t board) {
// Soft CSM
#if SCS_ACTIVE_POLARITY == SCS_LOW
  HAL_GPIO_WritePin(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_Pin, GPIO_PIN_RESET);
#else
  HAL_GPIO_WritePin(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_Pin, GPIO_PIN_SET);
#endif
}

// 关闭片选
void spi_unselect(int8_t board) {
#if SCS_ACTIVE_POLARITY == SCS_LOW
  HAL_GPIO_WritePin(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_Pin, GPIO_PIN_SET);
#else
  HAL_GPIO_WritePin(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_Pin, GPIO_PIN_RESET);
#endif
}

inline static uint8_t spi_transfer(uint8_t byte) {
  uint8_t rx_data;
  HAL_SPI_TransmitReceive(&ESC_SPI, &byte, &rx_data, 1, 2000);
  return rx_data;
}

void write(int8_t board, uint8_t *data, uint8_t size) {
  for (int i = 0; i < size; ++i) {
    spi_transfer(data[i]);
  }
}

void read(int8_t board, uint8_t *result, uint8_t size) {
  for (int i = 0; i < size; ++i) {
    result[i] = spi_transfer(DUMMY_BYTE);
  }
}

void spi_bidirectionally_transfer(int8_t board, uint8_t *result, uint8_t *data,
                                  uint8_t size) {
  for (int i = 0; i < size; ++i) {
    result[i] = spi_transfer(data[i]);
  }
}
