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

int whichsend = 0;  // 0表示CAN0，1表示CAN1

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
  can_queue_instance->tail = nextTail;
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

int32_t tx_success = 0;  // 发送成功计数
// 此函数在定时器中断或者后台任务中调用，实现交替发送can帧
void ProcessCanQueue(int ifCan0) {
  // 构建消息结构体
  CAN_Message msg;
  CAN_QueueInstance *pQueue = (ifCan0 == 1) ? &can0Queue : &can1Queue;
  uint32_t tx_mailbox = 0;
  if (!IsCanQueueEmpty(pQueue) &&
      HAL_CAN_GetTxMailboxesFreeLevel(pQueue->can_instance) > 0) {
    // 如果数据不为空
    if (DequeueCanMessage(pQueue, &msg) == 0) {
      if (HAL_CAN_AddTxMessage(pQueue->can_instance, &msg.header, msg.data,
                               &tx_mailbox) != HAL_OK) {
        // 发送失败，记录错误或采取其它措施
      } else {
        tx_success++;
        if (ifCan0)
          can0_tx_err_cnt = 0;
        else
          can1_tx_err_cnt = 0;
      }
    }
  }
}