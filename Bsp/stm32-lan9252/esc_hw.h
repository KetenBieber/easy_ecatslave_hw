/**
 * @file esc_hw.h
 * @author Keten (2863861004@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-04-10
 *
 * @copyright Copyright (c) 2025
 *
 * @attention :
 * @note :
 * @versioninfo :
 */
#ifndef ESC_HW_H
#define ESC_HW_H

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------include-----------------------------------*/
#include <string.h>

#include "bsp_gpio.h"
#include "bsp_spi.h"
#include "esc.h"

/*----------------------------------macro-----------------------------------*/

#define O_RDWR 1

#define BIT(x) 1 << (x)  // 将1左移x位，用于生成对应的位掩码 2^x
// 掩码怎么用？ 把你要验证的数去 &
// 它，可以验证你的数的某一位是不是1（相&有1才为1）

#define ESC_CMD_SERIAL_WRITE 0x02  //
#define ESC_CMD_SERIAL_READ 0x03   //
#define ESC_CMD_FAST_READ 0x0B     //
#define ESC_CMD_RESET_SQI 0xFF

#define ESC_CMD_FAST_READ_DUMMY 1
#define ESC_CMD_ADDR_INC BIT(6)  // 生成掩码 0100 0000 --- 0x40

#define ESC_PRAM_RD_FIFO_REG 0x000
#define ESC_PRAM_WR_FIFO_REG 0x020
#define ESC_PRAM_RD_ADDR_LEN_REG 0x308
#define ESC_PRAM_RD_CMD_REG 0x30C
#define ESC_PRAM_WR_ADDR_LEN_REG 0x310
#define ESC_PRAM_WR_CMD_REG 0x314  // 向ESC写入PRAM时所需的写命令寄存器地址

#define ESC_PRAM_CMD_BUSY \
  BIT(31)  // 生成掩码 1000 0000 0000 0000 0000 0000 0000 0000 --- 0x8000 0000
#define ESC_PRAM_CMD_ABORT \
  BIT(30)  // 生成掩码 0100 0000 0000 0000 0000 0000 0000 0000 --- 0x4000 0000

#define ESC_PRAM_CMD_CNT(x) ((x >> 8) & 0x1F)
#define ESC_PRAM_CMD_AVAIL BIT(0)

#define ESC_PRAM_SIZE(x) ((x) << 16)
#define ESC_PRAM_ADDR(x) ((x) << 0)

#define ESC_CSR_DATA_REG 0x300
#define ESC_CSR_CMD_REG 0x304  //

#define ESC_CSR_CMD_BUSY BIT(31)
#define ESC_CSR_CMD_READ \
  (BIT(31) | BIT(30))              // 1100 0000 0000 0000 0000 0000 0000 0000
#define ESC_CSR_CMD_WRITE BIT(31)  // 1000 0000 0000 0000 0000 0000 0000 0000
#define ESC_CSR_CMD_SIZE(x) (x << 16)  // 将命令左移到高16位

#define ESC_RESET_CTRL_REG 0x1F8    // 复位控制寄存器
#define ESC_RESET_CTRL_RST BIT(6)   // 生成掩码
#define ESC_DIGITAL_RST 0x00000001  // 写入数据

#define ESC_ID_REV_REG 0x050
#define LAN9252_ID_REV 0x9252

#define ESC_IRQ_CFG_REG 0x054
#define ESC_INT_EN_REG 0x05C
#define ESC_BYTE_TEST_REG 0x064
#define ESC_TEST_VALUE 0x87654321

#define ESC_HW_CFG_REG 0x074
#define ESC_READY BIT(27)  // 生成掩码

/* for DC */
#define ESCREG_ESC_CONFIG 0x0141           // esc寄存器，供读取 检查 DC使能标志
#define DC_SYNC_OUT 0x04                   // 位掩码 0100 也就是对第三位做检查
#define ESCREG_CYCLIC_UNIT_CONTROL 0x0980  // 0x0980:0
#define SYNC_OUT_PDI_CONTROL 0x01
#define ESCREG_SYNC0_CYCLE_TIME \
  0x09A0  // 该寄存器用于存放SYNC0（第一个同步信号）的周期时间，通过读取该地址，可以获得SYNC0信号的间隔时间（通常单位为纳秒）
#define ESCREG_SYNC_START_TIME \
  0x0990  // 这个寄存器用于写入同步起始时间，当同步单元分配给PDI时，需要设置同步的起始时间，从而使同步操作正确启动。
#define SYNC_START_OFFSET 2342840 * 2  // 该宏给出了一个固定的时间偏移量

/* update ESCvar.ALevent by reading anything over PDI  */
#define ESC_updateALevent()                        \
  ESC_read(ESCREG_LOCALTIME, (void *)&ESCvar.Time, \
           sizeof(ESCvar.Time))  // 读取本地事件

/*----------------------------------function----------------------------------*/
void ESC_read(uint16_t address, void *buf, uint16_t len);
void ESC_write(uint16_t address, void *buf, uint16_t len);
void ESC_reset(void);
uint8_t ESC_IsLAN9252();
void ESC_init(const esc_cfg_t *config);
void ESC_interrupt_enable(uint32_t mask);
void ESC_interrupt_disable(uint32_t mask);
int ESC_dc_watchdog_init(void);

/*------------------------------------test------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* ESC_HW_H */
