/**
 * @file can_queue.h
 * @author Keten (2863861004@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-05-29
 *
 * @copyright Copyright (c) 2025
 *
 * @attention :
 * @note :
 * @versioninfo :
 */
#ifndef CAN_QUEUE_H
#define CAN_QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------include-----------------------------------*/
#include <stdio.h>

#include "esc_config.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_can.h"

/*-----------------------------------macro------------------------------------*/
#define CAN_QUEUE_SIZE 200
/*----------------------------------typedef-----------------------------------*/
typedef struct {
  CAN_TxHeaderTypeDef header;
  uint8_t data[8];
  float t_enqueue;  // 入队时间戳（cycle）
} CAN_Message;

typedef struct {
  CAN_HandleTypeDef *can_instance;  // 保存一个CAN句柄
  CAN_Message messages[CAN_QUEUE_SIZE];
  volatile uint32_t head;  // 队列头
  volatile uint32_t tail;  // 队列尾

#ifdef TEST_TIME
  // 统计指标
  uint32_t max_queue_depth;   // 最大队列深度
  float total_enqueue_delay;  // 总入队延迟
  uint64_t enqueue_count;     // 入队计数
  float avg_delay;            // 平均入队延迟
  float send_cycle_time,
      send_cycle_last_time;  // 发送周期时间,用于测量发送间隔
#endif

} CAN_QueueInstance;

/*----------------------------------variable----------------------------------*/

/*----------------------------------function----------------------------------*/

/**
 * @brief
 *
 * @param can_queue_instance
 * @param header
 * @param data
 * @return int
 */
int EnqueueCanMessage(CAN_QueueInstance *can_queue_instance,
                      const CAN_TxHeaderTypeDef *header, const uint8_t data[8]);

/**
 * @brief
 *
 * @param can_queue_instance
 * @param msg
 * @return * int
 */
int DequeueCanMessage(CAN_QueueInstance *can_queue_instance, CAN_Message *msg);

/**
 * @brief
 *
 * @param can_queue_instance
 * @return int
 */
int IsCanQueueEmpty(CAN_QueueInstance *can_queue_instance);

/**
 * @brief
 *
 */
void ProcessCanQueue(int ifCan0);
#ifdef __cplusplus
}
#endif

#endif /* CAN_QUEUE_H */
