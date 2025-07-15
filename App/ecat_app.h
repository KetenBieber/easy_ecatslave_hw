/**
 * @file ecat_app.h
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
#ifndef ECAT_APP_H
#define ECAT_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------include-----------------------------------*/
/* esc relative */
#include "esc_hw.h"

/* soes relative */
#include "actuator_factor.h"
#include "bsp_bitband.h"
#include "bsp_can.h"
#include "bsp_dwt.h"
#include "can_queue.h"
#include "ecat_slv.h"
#include "utypes.h"

/*----------------------------------macro----------------------------------*/
// Only SM2 should be masked to generate PDI interrupt
#define MIXEDMODE_PDI_INT_MASK ESCREG_ALEVENT_SM2

#define CAN_TX_ERR_CNT_MAX 5000  // 最大的发送错误次数

/* vesc relative */
#define CAN_PACKET_STATUS 9  // 电流&转速 接收数据包标识符
// 控制命令枚举
#define CAN_CMD_SET_CURRENT 0x01
#define CAN_CMD_SET_ERPM 0x03
#define CAN_CMD_SET_BRAKE 0x02

#define BRAKE_CURRENT 6000  // 刹车电流

/*----------------------------------variable----------------------------------*/

extern CAN_TxHeaderTypeDef CAN0_TxHeader;
extern CAN_TxHeaderTypeDef CAN1_TxHeader;

/*----------------------------------function----------------------------------*/
void toggle_flash(uint32_t *counter, uint8_t led_pin, uint32_t start,
                  uint32_t interval, uint8_t count);

void esc_pdi_debug();

void Ecatapp_Init(void);
void Ecatapp_Loop(void);

void ESC_dc_watchdog_feed(void);

#ifdef __cplusplus
}
#endif

#endif /* ECAT_APP_H */
