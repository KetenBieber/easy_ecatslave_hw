/**
 * @file can_queue.c
 * @author Keten (2863861004@qq.com)
 * @brief can发送任务队列实现
 * @version 0.1
 * @date 2025-05-29
 *
 * @copyright Copyright (c) 2025
 *
 * @attention :
 * @note : 理想下，入队tail前进一步，出队后head前进一步
 *         生产者和消费者的速度差距体现在tail 和 head的差值
 *         消费者应该跟随生产者，设置需求应该满足于生产者
 *         最好就是head紧随tail
 * @versioninfo :
 */
#include "can_queue.h"

#include "bsp_dwt.h"

CAN_QueueInstance can0Queue = {
    .can_instance = &CAN0_Handle,
    .messages = {0},
    .head = 0,
    .tail = 0,
};

CAN_QueueInstance can1Queue = {
    .can_instance = &CAN1_Handle,
    .messages = {0},
    .head = 0,
    .tail = 0,
};

extern uint32_t can0_tx_err_cnt;
extern uint32_t can1_tx_err_cnt;
extern int32_t add_to_mailbox_success;

// 入队：成功返回0，队列满返回-1
int EnqueueCanMessage(CAN_QueueInstance *can_queue_instance,
                      const CAN_TxHeaderTypeDef *header,
                      const uint8_t data[8]) {
  uint32_t nextTail = (can_queue_instance->tail + 1) % CAN_QUEUE_SIZE;
  if (nextTail == can_queue_instance->head) {
    return -1;  // 队列已满
  }
  can_queue_instance->messages[can_queue_instance->tail].header = *header;
  memcpy(can_queue_instance->messages[can_queue_instance->tail].data, data,
         sizeof(uint8_t) * 8);

#ifdef TEST_TIME
  can_queue_instance->messages[can_queue_instance->tail].t_enqueue =
      DWT_GetTimeline_ms();
#endif

  can_queue_instance->tail = nextTail;

#ifdef TEST_TIME
  uint32_t depth =
      (can_queue_instance->tail + CAN_QUEUE_SIZE - can_queue_instance->head) %
      CAN_QUEUE_SIZE;
  if (depth > can_queue_instance->max_queue_depth)
    can_queue_instance->max_queue_depth = depth;
#endif

  return 0;
}

// 出队：成功返回0，队列空返回-1
int DequeueCanMessage(CAN_QueueInstance *can_queue_instance, CAN_Message *msg) {
  if (can_queue_instance->head == can_queue_instance->tail) {
    // 队列为空
    return -1;
  }
  *msg = can_queue_instance->messages[can_queue_instance->head];
  can_queue_instance->head = (can_queue_instance->head + 1) % CAN_QUEUE_SIZE;
  return 0;
}

// 队列是否为空
int IsCanQueueEmpty(CAN_QueueInstance *can_queue_instance) {
  return (can_queue_instance->head == can_queue_instance->tail);
}

#ifdef TEST_TIME
int32_t can0_tx_success = 0;  // 发送成功计数
int32_t can1_tx_success = 0;
float can0_arbitration_lost = 0.f;
float can1_arbitration_lost = 0.f;
#endif
// 此函数在定时器中断或者后台任务中调用，实现交替发送can帧
void ProcessCanQueue(int ifCan0) {
  // 构建消息结构体
  CAN_Message msg;
  CAN_QueueInstance *pQueue = (ifCan0 == 1) ? &can0Queue : &can1Queue;
  uint32_t tx_mailbox = 0;
  if (!IsCanQueueEmpty(pQueue)
#ifndef USE_CAN_TX_IDLE_INTERRUPT
      && HAL_CAN_GetTxMailboxesFreeLevel(pQueue->can_instance) > 0
#endif
  ) {
    // 如果数据不为空
    if (DequeueCanMessage(pQueue, &msg) == 0) {
#ifdef TEST_TIME
      float t_now = DWT_GetTimeline_ms();
      pQueue->total_enqueue_delay += (t_now - msg.t_enqueue);
      pQueue->enqueue_count++;
#endif

      if (HAL_CAN_AddTxMessage(pQueue->can_instance, &msg.header, msg.data,
                               &tx_mailbox) != HAL_OK) {
        // 发送失败，记录错误或采取其它措施
        if (ifCan0) {
          can0_tx_err_cnt++;
        } else {
          can1_tx_err_cnt++;
        }
      } else {
#ifdef TEST_TIME
        if (ifCan0)
          can0_tx_success++;
        else
          can1_tx_success++;
#endif
      }
#ifdef TEST_TIME
      pQueue->avg_delay = pQueue->total_enqueue_delay /
                          (pQueue->enqueue_count * 1.0f);  // 平均排队延时
      // 计算丢包率
      if (ifCan0)
        can0_arbitration_lost =
            (can0_tx_err_cnt * 1.0f) / pQueue->enqueue_count;
      else
        can1_arbitration_lost =
            (can1_tx_err_cnt * 1.0f) / pQueue->enqueue_count;
      add_to_mailbox_success =
          can0_tx_success + can1_tx_success;  // 成功添加到邮箱的计数
#endif
    }
  }
}
