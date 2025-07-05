/**
 * @file bsp_spi.c
 * @author Keten (2863861004@qq.com)
 * @brief 板级spi包，封装esc上的spi操作，具体引脚可以在esc_config.h中修改
 * @version 0.1
 * @date 2025-04-09
 *
 *       2025-06-24 updata LL library spi
 *
 *       2025-07-04 update spi dma
 *
 *
 * @copyright Copyright (c) 2025
 *
 * @attention :
 * @note :
 * @versioninfo :
 */
#include "bsp_spi.h"

void spi_setup(void) {
  spi_dmaisr_init();
  /* SPI Instance relative */
  LL_SPI_InitTypeDef SPI_InitStruct = {0};
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);

  /* SPI1 GPIO Configuration */
  GPIO_InitStruct.Pin = FLASH_SPI_SCK_Pin | FLASH_SPI_MOSI_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_5;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = FLASH_SPI_MISO_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_5;
  LL_GPIO_Init(FLASH_SPI_MISO_GPIO_Port, &GPIO_InitStruct);

  LL_GPIO_SetOutputPin(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_Pin);
  GPIO_InitStruct.Pin = FLASH_SPI_CS_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  LL_GPIO_Init(FLASH_SPI_CS_GPIO_Port, &GPIO_InitStruct);

  spi_dma_configuration();
  /* for spi  */
  SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
  SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
  SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
  SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
  SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV2;
  SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
  SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
  SPI_InitStruct.CRCPoly = 10;
  LL_SPI_Init(ESC_SPI_Instance, &SPI_InitStruct);
  LL_SPI_SetStandard(ESC_SPI_Instance, LL_SPI_PROTOCOL_MOTOROLA);

  LL_DMA_SetPeriphAddress(DMA2, LL_DMA_STREAM_3,
                          LL_SPI_DMA_GetRegAddr(ESC_SPI_Instance));
  // LL_DMA_ClearFlag_TC3(DMA2);
  // LL_DMA_EnableIT_TC(DMA2, LL_DMA_STREAM_3);
  LL_SPI_EnableDMAReq_TX(SPI1);
  LL_SPI_Enable(ESC_SPI_Instance);
}

void spi_dmaisr_init(void) {
  // 使能DMA时钟
  __HAL_RCC_DMA2_CLK_ENABLE();

  // NVIC_SetPriority(DMA2_Stream3_IRQn,
  //                  NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
  // NVIC_EnableIRQ(DMA2_Stream3_IRQn);
}

void spi_dma_configuration(void) {
  /* SPI1_TX Init */
  LL_DMA_SetChannelSelection(DMA2, LL_DMA_STREAM_3, LL_DMA_CHANNEL_3);

  LL_DMA_SetDataTransferDirection(DMA2, LL_DMA_STREAM_3,
                                  LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

  LL_DMA_SetStreamPriorityLevel(DMA2, LL_DMA_STREAM_3, LL_DMA_PRIORITY_LOW);

  LL_DMA_SetMode(DMA2, LL_DMA_STREAM_3, LL_DMA_MODE_NORMAL);

  LL_DMA_SetPeriphIncMode(DMA2, LL_DMA_STREAM_3, LL_DMA_PERIPH_NOINCREMENT);

  LL_DMA_SetMemoryIncMode(DMA2, LL_DMA_STREAM_3, LL_DMA_MEMORY_INCREMENT);

  LL_DMA_SetPeriphSize(DMA2, LL_DMA_STREAM_3, LL_DMA_PDATAALIGN_BYTE);

  LL_DMA_SetMemorySize(DMA2, LL_DMA_STREAM_3, LL_DMA_MDATAALIGN_BYTE);

  LL_DMA_DisableFifoMode(DMA2, LL_DMA_STREAM_3);
}

// 开启片选
void spi_select(int8_t board) {
// Soft CSM
#if SCS_ACTIVE_POLARITY == SCS_LOW
  LL_GPIO_ResetOutputPin(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_Pin);
#else
  LL_GPIO_SetOutputPin(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_Pin);
#endif
}

// 关闭片选
void spi_unselect(int8_t board) {
#if SCS_ACTIVE_POLARITY == SCS_LOW
  LL_GPIO_SetOutputPin(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_Pin);
#else
  LL_GPIO_ResetOutputPin(FLASH_SPI_CS_GPIO_Port, FLASH_SPI_CS_Pin);
#endif
}

inline static uint8_t spi_transfer(uint8_t byte) {
  uint32_t timeout = 10000U;  // 设置超时时间

  // 等待发送缓存空，有超时保护
  while (!LL_SPI_IsActiveFlag_TXE(ESC_SPI_Instance)) {
    if (--timeout == 0) {
      // 超时处理，可以返回一个错误码或结束函数
      return 0xFF;  // 返回错误码（根据具体需求修改）
    }
  }
  LL_SPI_TransmitData8(ESC_SPI_Instance, byte);

  timeout = 10000U;
  // 等待接收缓存非空，有超时保护
  while (!LL_SPI_IsActiveFlag_RXNE(ESC_SPI_Instance)) {
    if (--timeout == 0) {
      return 0xFF;  // 返回错误码
    }
  }

  return LL_SPI_ReceiveData8(ESC_SPI_Instance);
}

/**
 * @brief 仅写入
 *
 * @param board
 * @param data
 * @param size
 */
void write(int8_t board, uint8_t *data, uint8_t size) {
  // for (int i = 0; i < size; ++i) {
  //   spi_transfer(data[i]);
  // }
#ifndef USE_DMA_SPI
  __disable_irq();

  for (uint32_t i = 0; i < size; i++) {
    uint32_t timeout = 10000U;
    // 写入一个字节，开始发送
    LL_SPI_TransmitData8(ESC_SPI_Instance, data[i]);
    // 等待TXE，保证DR可写
    while (!LL_SPI_IsActiveFlag_TXE(ESC_SPI_Instance)) {
      if (--timeout == 0) {
        // 超时处理
        return;
      }
    }

    // 如果有上次残留的 RXNE，立刻读出清标志
    (void)LL_SPI_ReceiveData8(ESC_SPI_Instance);
  }

  // 最后统一等 BSY/RXNE
  while ((LL_SPI_IsActiveFlag_BSY(ESC_SPI_Instance))) {
    (void)LL_SPI_ReceiveData8(ESC_SPI_Instance);
  }

  __enable_irq();

#else
  // 2. 切半双工 TX-only
  LL_SPI_Disable(ESC_SPI_Instance);
  LL_SPI_SetTransferDirection(ESC_SPI_Instance, LL_SPI_HALF_DUPLEX_TX);
  LL_SPI_Enable(ESC_SPI_Instance);

  // 3. 等待上次 DMA 事务结束
  while (LL_DMA_IsEnabledStream(DMA2, LL_DMA_STREAM_3));

  // 4. 配置地址 & 长度
  LL_DMA_ClearFlag_TC3(DMA2);
  LL_DMA_SetMemoryAddress(DMA2, LL_DMA_STREAM_3, (uint32_t)data);
  LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_3, size);

  // 5. 启动 TX-DMA
  LL_SPI_EnableDMAReq_TX(ESC_SPI_Instance);
  LL_DMA_EnableStream(DMA2, LL_DMA_STREAM_3);

  // 6. 等待 DMA 完成
  // 8. 等待 TCIF3 置位（本次传输完成）
  while (!LL_DMA_IsActiveFlag_TC3(DMA2)) {
    __NOP();
  }
  // 9. 清标志
  LL_DMA_ClearFlag_TC3(DMA2);

  // 7. 关闭 DMA 请求
  LL_SPI_DisableDMAReq_TX(ESC_SPI_Instance);
  // LL_DMA_ClearFlag_TC3(DMA2);

  // 8. 恢复全双工
  LL_SPI_Disable(ESC_SPI_Instance);
  LL_SPI_SetTransferDirection(ESC_SPI_Instance, LL_SPI_FULL_DUPLEX);
  LL_SPI_Enable(ESC_SPI_Instance);
#endif
}

/**
 * @brief 仅读取
 *
 * @param board
 * @param result
 * @param size
 */
void read(int8_t board, uint8_t *result, uint8_t size) {
  __disable_irq();
  for (uint32_t i = 0; i < size; i++) {
    // 1) 发一个哑字节，产生时钟
    LL_SPI_TransmitData8(ESC_SPI_Instance, DUMMY_BYTE);
    // 2) 等待 RXNE，就绪即可读
    while (!LL_SPI_IsActiveFlag_RXNE(ESC_SPI_Instance));
    // 3) 读 DR，既拿到数据又清除 RXNE
    result[i] = LL_SPI_ReceiveData8(ESC_SPI_Instance);
  }
  // 4) 全部读完后，再等 BSY 清零
  while (LL_SPI_IsActiveFlag_BSY(ESC_SPI_Instance));
  __enable_irq();
}
/**
 * @brief 全双工传输，在发送数据的同时，将每个传输操作返回的字节存入
 *
 * @param board
 * @param result
 * @param data
 * @param size
 */
void spi_bidirectionally_transfer(int8_t board, uint8_t *result, uint8_t *data,
                                  uint8_t size) {
  for (int i = 0; i < size; ++i) {
    result[i] = spi_transfer(data[i]);
  }
}
