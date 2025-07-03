/**
 * @file bsp_spi.h
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
#ifndef BSP_SPI_H
#define BSP_SPI_H

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------include-----------------------------------*/
#include "esc_config.h"
#include "stm32f4xx_hal.h"

/*-----------------------------------macro------------------------------------*/
#define DUMMY_BYTE 0xFF
#define tout 5000
/*----------------------------------variable----------------------------------*/

/*----------------------------------function----------------------------------*/
void spi_setup(void);
void spi_select(int8_t board);
void spi_unselect(int8_t board);

void write(int8_t board, uint8_t *data, uint8_t size);
void read(int8_t board, uint8_t *result, uint8_t size);
void spi_bidirectionally_transfer(int8_t board, uint8_t *result, uint8_t *data,
                                  uint8_t size);

#ifdef __cplusplus
}
#endif

#endif /* BSP_SPI_H */
