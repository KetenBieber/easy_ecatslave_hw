/**
 * @file bsp_can.h
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
#ifndef BSP_CAN_H
#define BSP_CAN_H

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------include-----------------------------------*/
#include <stdbool.h>

#include "esc_config.h"
#include "stm32f4xx_hal.h"

/*-----------------------------------macro------------------------------------*/
#define CanFilter_0 (0 << 3)
#define CanFilter_1 (1 << 3)
#define CanFilter_2 (2 << 3)
#define CanFilter_3 (3 << 3)
#define CanFilter_4 (4 << 3)
#define CanFilter_5 (5 << 3)
#define CanFilter_6 (6 << 3)
#define CanFilter_7 (7 << 3)
#define CanFilter_8 (8 << 3)
#define CanFilter_9 (9 << 3)
#define CanFilter_10 (10 << 3)
#define CanFilter_11 (11 << 3)
#define CanFilter_12 (12 << 3)
#define CanFilter_13 (13 << 3)
#define CanFilter_14 (14 << 3)
#define CanFilter_15 (15 << 3)
#define CanFilter_16 (16 << 3)
#define CanFilter_17 (17 << 3)
#define CanFilter_18 (18 << 3)
#define CanFilter_19 (19 << 3)
#define CanFilter_20 (20 << 3)
#define CanFilter_21 (21 << 3)
#define CanFilter_22 (22 << 3)
#define CanFilter_23 (23 << 3)
#define CanFilter_24 (24 << 3)
#define CanFilter_25 (25 << 3)
#define CanFilter_26 (26 << 3)
#define CanFilter_27 (27 << 3)

/* FIFO选择 占据字段中第2位的位置 可配置双FIFO */
#define CanFifo_0 (0 << 2)
#define CanFifo_1 (1 << 2)

/* std&ext 选择 占据字段中第1位的位置 可配置扩展帧or标准帧 区别是扩展29位
 * 标准11位 */
#define Can_STDID (0 << 1)
#define Can_EXTID (1 << 1)

/* data&remote 选择 占据字段中第0位的位置 可配置数据帧or遥控帧 */
#define Can_DataType (0 << 0)
#define Can_RemoteType (1 << 0)

#define CAN_LINE_BUSY 0
#define CAN_SUCCESS 1
#define CAN_FIFO_SIZE 1024
/*----------------------------------variable----------------------------------*/
/* global */
extern uint8_t CAN0Fifo0_RxDataBuf[8];
extern uint8_t CAN0Fifo1_RxDataBuf[8];
extern uint8_t CAN1Fifo0_RxDataBuf[8];
extern uint8_t CAN1Fifo1_RxDataBuf[8];
/* 发送句柄 */
extern CAN_TxHeaderTypeDef CAN0_TxHeader;
extern CAN_TxHeaderTypeDef CAN1_TxHeader;

extern bool start_can0_watchdog;  // can0看门狗是否启动
extern bool start_can1_watchdog;  // can1看门狗是否启动

/*----------------------------------function----------------------------------*/

void CAN_Configuration(CAN_HandleTypeDef* hcan);

void CAN_Init(CAN_HandleTypeDef* hcan);

/**
 * @brief 需用用户自己提供实现，初始化can帧头部信息，减少赋值开销
 *
 * @param hcan
 */
void CAN_TxHeader_Init(void);

/**
 * @brief 配置看门狗参数
 *        需要初始化完DWT之后才可使用
 *
 * @param timeout
 * @param ifCan1 如果是can1，则为true，否则为can0
 */
void configWatchdog(float timeout, bool ifCan1);

/**
 * @brief can0接收看门狗处理
 *
 */
void CAN0_watchdogCallback(void);

/**
 * @brief can1接收看门狗处理
 *
 */
void CAN1_watchdogCallback(void);

/**
 * @brief can0回调
 *
 * @param hcan
 * @param temp_rxheader
 * @param data
 */
void CAN0_RxCpltCallback(CAN_HandleTypeDef* hcan,
                         CAN_RxHeaderTypeDef* temp_rxheader,
                         const uint8_t* data);

/**
 * @brief can1回调
 *
 * @param hcan
 * @param temp_rxheader
 * @param data
 * @return * void
 */
void CAN1_RxCpltCallback(CAN_HandleTypeDef* hcan,
                         CAN_RxHeaderTypeDef* temp_rxheader,
                         const uint8_t* data);

/**
 * @brief CAN过滤器初始化 ，具体配置参考头文件
 *
 * @param hcan
 * @param object_para
 * @param Id
 * @param MaskId
 */
void CAN_Filter_Init(CAN_HandleTypeDef* hcan, uint8_t object_para, uint32_t Id,
                     uint32_t MaskId);

/**
 * @brief 重启can控制器函数
 *
 * @param hcan
 */
void CAN_AppRestart(CAN_HandleTypeDef* hcan);

#ifdef __cplusplus
}
#endif

#endif /* BSP_CAN_H */
