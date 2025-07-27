/**
 * @file ecat_app.c
 * @author Keten (2863861004@qq.com)
 * @brief ecat 应用层代码编写
 * @version 0.1
 * @date 2025-04-09
 *
 * @copyright Copyright (c) 2025
 *
 * @attention :
 * @note : 直接使用内存共享的方式来实现数据的传输
 *
 *         需要注意，如果自己配置hal库，配置中断一定要在it.c中做好配置
 *
 *         can0 --- DJI Motor
 *         can1 --- VESC Motor
 *
 *          PA3 --- 1号舵轮io
 *          PD2 --- 2号舵轮io
 *          PC11 --- 3号舵轮io
 *
 *
 *
 * @versioninfo :
 */
#include "ecat_app.h"

#include "encoder_app.h"
#include "stm32f4xx_ll_tim.h"
#include "ws2812led_app.h"

/* global variable */
_Objects Obj;
uint8_t txpdo[MAX_TXPDO_SIZE] __attribute__((aligned(8)));  // 强制8字节对齐
uint8_t can_txData[8] = {0};

/* sem to run the apploop */
static uint8_t pdi_isr_flag_ = 0;
static uint8_t sync0_isr_flag_ = 0;
static uint8_t sync1_isr_flag_ = 0;
uint32_t can0_tx_err_cnt = 0;
uint32_t can1_tx_err_cnt = 0;

// can0和can1的错误计数,可用于重启
uint32_t can0_queue_err_cnt = 0;  // can0队列错误计数
uint32_t can1_queue_err_cnt = 0;  // can1队列错误计数
uint32_t can0_queue_cnt = 0;
uint32_t can1_queue_cnt = 0;  // can0和can1队列计数

int start_led = 0;
int set_encoder_zero = 0;  // 设置编码器归零标志位
uint8_t last_io3_state = 0;

extern bool can0_no_msg;
extern bool can1_no_msg;

// 编码器值
extern EncoderData encoder_data;

#ifdef TEST_TIME
float can0_queue_arbitration_lost = 0.0f;    // can0队列仲裁时间
float can1_queue_arbitration_lost = 0.0f;    // can0队列仲裁时间
float can_all_send_arbitration_lost = 0.0f;  // 所有can发送仲裁时间
int32_t real_tx_success = 0;
int32_t add_to_mailbox_success = 0;
#endif
extern CAN_QueueInstance can0Queue;
extern CAN_QueueInstance can1Queue;

/* struct for calculate */
DJI_Motor_Position_t dji_motor_pos_[8] = {0};  // 大疆电机位置数据

/* watch dog to watch the pdi and sync0 */
static uint32_t no_pdi_msgs_ = 0;
static uint32_t no_sync0_msgs_ = 0;

#define CAN_TIMEOUT_MS 1000  // 超时1000ms

void toggle_flash(uint32_t *counter, uint8_t led_pin, uint32_t start,
                  uint32_t interval, uint8_t count) {
  (*counter)++;
  uint32_t end = start + interval * (count - 1);
  if (*counter >= start && *counter <= end) {
    if (((*counter - start) % interval) == 0) {
      PCToggle(led_pin);
    }
  }
  if (*counter > end) {
    *counter = 0;
  }
}

uint16_t _dc_checker(void) {
  int dog_ = 0;
  dog_ = ESC_dc_watchdog_init();
  if (dog_ == 0) {
    // 说明DC配置失败
    return 0;
  } else
    APP_setwatchdog(dog_);
  return 0;
}

void esc_pdi_debug() {
  // Read few core CSR registers to verify PDI is working
  // 读取几个核心CSR寄存器以验证PDI是否正常工作
  uint8_t value = 0;
  ESC_read(0x0000, (void *)&value, sizeof(uint8_t));
  ESC_read(0x0001, (void *)&value, sizeof(uint8_t));
  ESC_read(0x0004, (void *)&value, sizeof(uint8_t));
  ESC_read(0x0005, (void *)&value, sizeof(uint8_t));
  ESC_read(0x0006, (void *)&value, sizeof(uint8_t));
}

/**
 * @brief 更新从站当前的输入数据，以便后续在PDO中发给主站
 *
 */
void cb_get_inputs(void) {
  /* 一共8个可设置input io */

  /* 获取光电门io状态，进行打包 */
  Obj.input_io.io1 = PGin(0);
  Obj.input_io.io2 = PGin(1);
  Obj.input_io.io3 = PGin(2);

  handle_encoder_pos(&encoder_data);
  Obj.can0_motor_positions[5] = encoder_data.angle_splitter.i16[0];
  Obj.can0_motor_currents[5] = encoder_data.angle_splitter.i16[1];
  // Obj.input_io.io2 = PDin(7);
  // Obj.input_io.io3 = PGin(10);
  /* can总线上电机，将直接由can的接收中断进行打包 */
}

#ifdef TEST_TIME
float current_time = 0.0f, last_time = 0.0f;  // 用于记录当前时间，单位为秒
float dt;
float setOutput_run_time = 0.0f;
#endif
/**
 * @brief 处理RxPDO
 *
 */
void cb_set_outputs(void) {
  if (start_led == 0) {
    switch_cur_state = -1;  // 开机动画
    start_led = 1;
  }
#ifdef TEST_TIME
  current_time = DWT_GetTimeline_ms();
  dt = current_time - last_time;
  last_time = current_time;
#endif
  /* 有8个可控io */
  // 电磁阀 io 控制
  PAout(3) = Obj.output_io.io1;  // 1号io

  if (Obj.output_io.io1 == 1) {
    if_shoot = 2;  // 设置为2，表示开火 这个标志位只在状态机中被取消
  }
  if (Obj.output_io.io2 == 1 && Obj.output_io.io1 == 0) {
    if_shoot = 1;  // 设置为1 ,表示蓄能
  }

  uint8_t cur_io3 = Obj.output_io.io3;
  if (cur_io3 && !last_io3_state) {
    set_encoder_zero = 1;
  }
  last_io3_state = cur_io3;

  // can 命令下发
  CAN0_TxHeader.StdId = 0x200;
  can_txData[0] = (Obj.can0_motor_commands[0] >> 8u);
  can_txData[1] = Obj.can0_motor_commands[0];
  can_txData[2] = (Obj.can0_motor_commands[1] >> 8u);
  can_txData[3] = Obj.can0_motor_commands[1];
  can_txData[4] = (Obj.can0_motor_commands[2] >> 8u);
  can_txData[5] = Obj.can0_motor_commands[2];
  can_txData[6] = (Obj.can0_motor_commands[3] >> 8u);
  can_txData[7] = Obj.can0_motor_commands[3];

#ifndef IF_USE_SENDQUEUE

  if (HAL_CAN_GetTxMailboxesFreeLevel(&CAN0_Handle) > 0) {
    if (HAL_CAN_IsTxMessagePending(&CAN0_Handle, CAN_TX_MAILBOX0) == HAL_OK) {
      HAL_CAN_AddTxMessage(&CAN0_Handle, &CAN0_TxHeader, can_txData,
                           (uint32_t *)CAN_TX_MAILBOX0);
    } else if (HAL_CAN_IsTxMessagePending(&CAN0_Handle, CAN_TX_MAILBOX1) ==
               HAL_OK) {
      HAL_CAN_AddTxMessage(&CAN0_Handle, &CAN0_TxHeader, can_txData,
                           (uint32_t *)CAN_TX_MAILBOX1);
    } else if (HAL_CAN_IsTxMessagePending(&CAN0_Handle, CAN_TX_MAILBOX2) ==
               HAL_OK) {
      HAL_CAN_AddTxMessage(&CAN0_Handle, &CAN0_TxHeader, can_txData,
                           (uint32_t *)CAN_TX_MAILBOX2);
    }
  } else {
  }
#else
  if (EnqueueCanMessage(&can0Queue, &CAN0_TxHeader, can_txData) == -1) {
    // 队列满时增加错误计数
    can0_queue_err_cnt++;
  }
  can0_queue_cnt++;
#endif

  CAN0_TxHeader.StdId = 0x1FF;
  can_txData[0] = (Obj.can0_motor_commands[4] >> 8u);
  can_txData[1] = Obj.can0_motor_commands[4];
  can_txData[2] = (Obj.can0_motor_commands[5] >> 8u);
  can_txData[3] = Obj.can0_motor_commands[5];
  can_txData[4] = (Obj.can0_motor_commands[6] >> 8u);
  can_txData[5] = Obj.can0_motor_commands[6];
  can_txData[6] = (Obj.can0_motor_commands[7] >> 8u);
  can_txData[7] = Obj.can0_motor_commands[7];
#ifndef IF_USE_SENDQUEUE

  if (HAL_CAN_GetTxMailboxesFreeLevel(&CAN0_Handle) > 0) {
    if (HAL_CAN_IsTxMessagePending(&CAN0_Handle, CAN_TX_MAILBOX0) == HAL_OK) {
      HAL_CAN_AddTxMessage(&CAN0_Handle, &CAN0_TxHeader, can_txData,
                           (uint32_t *)CAN_TX_MAILBOX0);
    } else if (HAL_CAN_IsTxMessagePending(&CAN0_Handle, CAN_TX_MAILBOX1) ==
               HAL_OK) {
      HAL_CAN_AddTxMessage(&CAN0_Handle, &CAN0_TxHeader, can_txData,
                           (uint32_t *)CAN_TX_MAILBOX1);
    } else if (HAL_CAN_IsTxMessagePending(&CAN0_Handle, CAN_TX_MAILBOX2) ==
               HAL_OK) {
      HAL_CAN_AddTxMessage(&CAN0_Handle, &CAN0_TxHeader, can_txData,
                           (uint32_t *)CAN_TX_MAILBOX2);
    }
  } else {
  }
#else
  if (EnqueueCanMessage(&can0Queue, &CAN0_TxHeader, can_txData) == -1) {
    // 队列满时增加错误计数
    can0_queue_err_cnt++;
  }
  can0_queue_cnt++;
#endif

#ifdef TEST_TIME
  // for (int i = 0; i < 5; i++) {
  //   CAN0_TxHeader.StdId = 0x201 + i;
  //   can_txData[0] = (Obj.can0_motor_commands[0] >> 8u);
  //   can_txData[1] = Obj.can0_motor_commands[0];
  //   can_txData[2] = (Obj.can0_motor_commands[1] >> 8u);
  //   can_txData[3] = Obj.can0_motor_commands[1];
  //   can_txData[4] = (Obj.can0_motor_commands[2] >> 8u);
  //   can_txData[5] = Obj.can0_motor_commands[2];
  //   can_txData[6] = (Obj.can0_motor_commands[3] >> 8u);
  //   can_txData[7] = Obj.can0_motor_commands[3];
  //   if (EnqueueCanMessage(&can0Queue, &CAN0_TxHeader, can_txData) == -1) {
  //     // 队列满时增加错误计数
  //     can0_queue_err_cnt++;
  //   }
  //   can0_queue_cnt++;
  // }
#endif

#ifndef ONLY_CAN0
  /* 4个电机 */
  for (int i = 1; i <= 4; i++) {
    CAN1_TxHeader.ExtId = (CAN_CMD_SET_ERPM << 8 | i);
    CAN1_TxHeader.IDE = CAN_ID_EXT;
    if (Obj.vesc_can1_commands[i - 1] == 0) {
      // 如果目标转速为0，发送刹车指令
      can_txData[0] = ((int32_t)BRAKE_CURRENT >> 24) & 0xFF;
      can_txData[1] = ((int32_t)BRAKE_CURRENT >> 16) & 0xFF;
      can_txData[2] = ((int32_t)BRAKE_CURRENT >> 8) & 0xFF;
      can_txData[3] = (int32_t)BRAKE_CURRENT & 0xFF;
      CAN1_TxHeader.ExtId = (CAN_CMD_SET_BRAKE << 8) | i;
    } else {
      can_txData[0] = (int32_t)Obj.vesc_can1_commands[i - 1] >> 24 & 0xFF;
      can_txData[1] = (int32_t)Obj.vesc_can1_commands[i - 1] >> 16 & 0xFF;
      can_txData[2] = (int32_t)Obj.vesc_can1_commands[i - 1] >> 8 & 0xFF;
      can_txData[3] = (int32_t)Obj.vesc_can1_commands[i - 1] & 0xFF;
    }
#ifndef IF_USE_SENDQUEUE
    // 往can上发送电机指令
    if (HAL_CAN_IsTxMessagePending(&CAN1_Handle, CAN_TX_MAILBOX0) == HAL_OK) {
      HAL_CAN_AddTxMessage(&CAN1_Handle, &CAN1_TxHeader, can_txData,
                           (uint32_t *)CAN_TX_MAILBOX0);
    } else if (HAL_CAN_IsTxMessagePending(&CAN1_Handle, CAN_TX_MAILBOX1) ==
               HAL_OK) {
      HAL_CAN_AddTxMessage(&CAN1_Handle, &CAN1_TxHeader, can_txData,
                           (uint32_t *)CAN_TX_MAILBOX1);
    } else if (HAL_CAN_IsTxMessagePending(&CAN1_Handle, CAN_TX_MAILBOX2) ==
               HAL_OK) {
      HAL_CAN_AddTxMessage(&CAN1_Handle, &CAN1_TxHeader, can_txData,
                           (uint32_t *)CAN_TX_MAILBOX2);
    } else {
    }
#else
    if (EnqueueCanMessage(&can1Queue, &CAN1_TxHeader, can_txData) == -1) {
      // 队列满时增加错误计数
      can1_queue_err_cnt++;
    }
    can1_queue_cnt++;
#endif
  }

#ifdef TEST_TIME
  // for (int i = 0; i < 3; i++) {
  //   CAN1_TxHeader.ExtId = (CAN_CMD_SET_CURRENT << 8 | i);
  //   CAN1_TxHeader.IDE = CAN_ID_EXT;
  //   // 如果目标转速为0，发送刹车指令
  //   can_txData[0] = ((int32_t)BRAKE_CURRENT >> 24) & 0xFF;
  //   can_txData[1] = ((int32_t)BRAKE_CURRENT >> 16) & 0xFF;
  //   can_txData[2] = ((int32_t)BRAKE_CURRENT >> 8) & 0xFF;
  //   can_txData[3] = (int32_t)BRAKE_CURRENT & 0xFF;
  //   if (EnqueueCanMessage(&can1Queue, &CAN1_TxHeader, can_txData) == -1) {
  //     // 队列满时增加错误计数
  //     can1_queue_err_cnt++;
  //   }
  //   can1_queue_cnt++;
  // }
#endif

#endif
#ifdef TEST_TIME
  setOutput_run_time = DWT_GetTimeline_ms() - current_time;
  can0_queue_arbitration_lost = (float)can0_tx_err_cnt / (float)can0_queue_cnt;
  can1_queue_arbitration_lost = (float)can1_tx_err_cnt / (float)can1_queue_cnt;
  can_all_send_arbitration_lost =
      (float)real_tx_success /
      (float)add_to_mailbox_success;  // 所有can发送仲裁时间
#endif
}

void _clear_statusword(void) {}

void setZeroOutputs(void) {
  // 往can0发送空数据
  CAN0_TxHeader.StdId = 0x200;
  uint32_t tx_mailbox = 0;
  memset(can_txData, 0, sizeof(can_txData));
  if (EnqueueCanMessage(&can0Queue, &CAN0_TxHeader, can_txData) == -1)
    ;

  CAN0_TxHeader.StdId = 0x1FF;
  if (EnqueueCanMessage(&can0Queue, &CAN0_TxHeader, can_txData) == -1)
    ;

#ifndef ONLY_CAN0
  // 往can1上发送空数据
  for (int i = 1; i <= 4; i++) {
    CAN1_TxHeader.ExtId = (CAN_CMD_SET_ERPM << 8 | i);
    CAN1_TxHeader.IDE = CAN_ID_EXT;
    // 如果目标转速为0，发送刹车指令
    can_txData[0] = ((int32_t)BRAKE_CURRENT >> 24) & 0xFF;
    can_txData[1] = ((int32_t)BRAKE_CURRENT >> 16) & 0xFF;
    can_txData[2] = ((int32_t)BRAKE_CURRENT >> 8) & 0xFF;
    can_txData[3] = (int32_t)BRAKE_CURRENT & 0xFF;
    CAN1_TxHeader.ExtId = (CAN_CMD_SET_BRAKE << 8) | i;
    if (EnqueueCanMessage(&can1Queue, &CAN1_TxHeader, can_txData) == -1)
      ;
  }
#endif
}

/**
 * @brief 重载，其实就是多了个清除状态位的函数
 *        前面的代码是esc库中原有的默认的实现
 *
 */
void _txpdo_override(void) {
  if (MAX_MAPPINGS_SM3 > 0) {
    COE_pdoPack(txpdo, ESCvar.sm3mappings, SMmap3);
  }
  ESC_write(ESC_SM3_sma, txpdo, ESCvar.ESC_SM3_sml);
  _clear_statusword();
}
/**
 * @brief SDO 写入 回调函数
 *
 * @param index
 * @param subindex
 * @param flags
 */
void _post_sdo_callback(uint16_t index, uint8_t subindex, uint16_t flags) {
  // HAL_GPIO_WritePin(LED5_GPIO_Port, LED5_Pin, GPIO_PIN_SET);
}

/**
 * @brief 当esc从站状态变为safe-operational时选用的output函数
 *
 */
void _safeoutput_callback(void) {
  // 关闭LED任务
  switch_cur_state = -2;
  start_led = 0;
  // 往can0发送空数据
  CAN0_TxHeader.StdId = 0x200;
  uint32_t tx_mailbox = 0;
  memset(can_txData, 0, sizeof(can_txData));
  if (EnqueueCanMessage(&can0Queue, &CAN0_TxHeader, can_txData) == -1)
    ;

  CAN0_TxHeader.StdId = 0x1FF;
  if (EnqueueCanMessage(&can0Queue, &CAN0_TxHeader, can_txData) == -1)
    ;

#ifndef ONLY_CAN0
  // 往can1上发送空数据
  for (int i = 1; i <= 4; i++) {
    CAN1_TxHeader.ExtId = (CAN_CMD_SET_ERPM << 8 | i);
    CAN1_TxHeader.IDE = CAN_ID_EXT;
    // 如果目标转速为0，发送刹车指令
    can_txData[0] = ((int32_t)BRAKE_CURRENT >> 24) & 0xFF;
    can_txData[1] = ((int32_t)BRAKE_CURRENT >> 16) & 0xFF;
    can_txData[2] = ((int32_t)BRAKE_CURRENT >> 8) & 0xFF;
    can_txData[3] = (int32_t)BRAKE_CURRENT & 0xFF;
    CAN1_TxHeader.ExtId = (CAN_CMD_SET_BRAKE << 8) | i;

    if (EnqueueCanMessage(&can1Queue, &CAN1_TxHeader, can_txData) == -1)
      ;
  }
#endif
}

/* core cfg */
static esc_cfg_t config = {
    .user_arg = "lan9252",
#if defined(MixedMode) || defined(InterruptMode)
    .use_interrupt = 1,
#else
    .use_interrupt = 1,
#endif
    .watchdog_cnt = 2000,  //
    .set_defaults_hook = NULL,
    .pre_state_change_hook = NULL,
    .post_state_change_hook = NULL,
    .application_hook = NULL,
    .safeoutput_override = _safeoutput_callback,
    .pre_object_download_hook = NULL,
    .post_object_download_hook = _post_sdo_callback,
    .rxpdo_override = NULL,
    .txpdo_override = _txpdo_override,
    .esc_hw_interrupt_enable = ESC_interrupt_enable,
    .esc_hw_interrupt_disable = ESC_interrupt_disable,
    .esc_hw_eep_handler = NULL,
    .esc_check_dc_handler =
        _dc_checker,  // 用于检查DC同步设置，也就是当使用DC同步时，要调用的回调函数；
                      // 在状态转换（例如从PREOP切换到SAFEOP时）对DC同步参数进行检查和验证
};

void Ecatapp_Init(void) {
  ecat_slv_init(&config);
  PDI_IRQ_Configuration();  // 配置PDI中断
  SYNC0_Configuration();    // 配置SYNC0中断
  SYNC1_Configuration();    // 配置SYNC1中断
  ENABLE_ESC_PDI_INT;       // 使能PDI中断
  ENABLE_ESC_SYNC0_INT;     // 使能SYNC0中断
  ENABLE_ESC_SYNC1_INT;     // 使能SYNC1中断
}

/**
 * @brief can1 接收回调，处理大疆电机数据
 *
 *        2025.6.19 写点大粪，将浮点位置拆分后放在position 和 currents
 *
 *
 * @param hcan
 * @param rxheader
 * @param data
 */
void _can0_receive_callback(CAN_HandleTypeDef *hcan,
                            CAN_RxHeaderTypeDef *rxheader,
                            const uint8_t *data) {
  uint8_t index = rxheader->StdId - 0x201;
  if (hcan == &CAN0_Handle) {
#ifdef USE_ENCODER
#endif
    handle_dji_motor_pos(&dji_motor_pos_[index], data);
    // 位置位存放低8位,电流位存放高8位
    Obj.can0_motor_positions[index] =
        dji_motor_pos_[index].angle_splitter_.i16[0];
    Obj.can0_motor_currents[index] =
        dji_motor_pos_[index].angle_splitter_.i16[1];
    Obj.can0_motor_velocities[index] = (int16_t)((data[2] << 8u) | data[3]);
    Obj.can0_motor_temperatures[index] = (uint8_t)(data[6]);
  }
}

void _can1_receive_callback(CAN_HandleTypeDef *hcan,
                            CAN_RxHeaderTypeDef *rxheader,
                            const uint8_t *data) {
  uint8_t temp_vesc_id = rxheader->ExtId & 0xFF;   // 获取VESC ID
  uint16_t temp_vesc_flag = rxheader->ExtId >> 8;  // 解析电调命令标识符
  if (temp_vesc_flag == CAN_PACKET_STATUS) {
    switch (temp_vesc_id) {
      case 1: {
        Obj.vesc_can1_motor_velocities[0] =
            (int32_t)(((uint32_t)data[0]) << 24 | ((uint32_t)data[1]) << 16 |
                      ((uint32_t)data[2]) << 8 | ((uint32_t)data[3]));
        Obj.vesc_can1_motor_currents[0] =
            (int16_t)((uint32_t)data[4]) << 8 | ((uint32_t)data[5]);
        break;
      }
      case 2: {
        Obj.vesc_can1_motor_velocities[1] =
            (int32_t)(((uint32_t)data[0]) << 24 | ((uint32_t)data[1]) << 16 |
                      ((uint32_t)data[2]) << 8 | ((uint32_t)data[3]));
        Obj.vesc_can1_motor_currents[1] =
            (int16_t)((uint32_t)data[4]) << 8 | ((uint32_t)data[5]);

        break;
      }
      case 3: {
        Obj.vesc_can1_motor_velocities[2] =
            (int32_t)(((uint32_t)data[0]) << 24 | ((uint32_t)data[1]) << 16 |
                      ((uint32_t)data[2]) << 8 | ((uint32_t)data[3]));
        Obj.vesc_can1_motor_currents[2] =
            (int16_t)((uint32_t)data[4]) << 8 | ((uint32_t)data[5]);

        break;
      }
      case 4: {
        Obj.vesc_can1_motor_velocities[3] =
            (int32_t)(((uint32_t)data[0]) << 24 | ((uint32_t)data[1]) << 16 |
                      ((uint32_t)data[2]) << 8 | ((uint32_t)data[3]));
        Obj.vesc_can1_motor_currents[3] =
            (int16_t)((uint32_t)data[4]) << 8 | ((uint32_t)data[5]);

        break;
      }
      case 5: {
        Obj.vesc_can1_motor_velocities[4] =
            (int32_t)(((uint32_t)data[0]) << 24 | ((uint32_t)data[1]) << 16 |
                      ((uint32_t)data[2]) << 8 | ((uint32_t)data[3]));
        Obj.vesc_can1_motor_currents[5] =
            (int16_t)((uint32_t)data[4]) << 8 | ((uint32_t)data[5]);

        break;
      }
      case 6: {
        Obj.vesc_can1_motor_velocities[5] =
            (int32_t)(((uint32_t)data[0]) << 24 | ((uint32_t)data[1]) << 16 |
                      ((uint32_t)data[2]) << 8 | ((uint32_t)data[3]));
        Obj.vesc_can1_motor_currents[5] =
            (int16_t)((uint32_t)data[4]) << 8 | ((uint32_t)data[5]);

        break;
      }
    }
  }
}

static uint32_t debug_pdi = 0;
static uint32_t debug_sync0 = 0;
/*
  使用LED4指示 sync0 是否正常工作
  使用LED5指示 pdi 是否正常工作
*/

#ifdef TEST_TIME
float func_1_runtime = 0, func_2_runtime = 0;
float loop_start = 0;

uint64_t start_copy_output_data = 0;
uint64_t copy_ouput_data_runtime = 0;

float sm2_cycle_time = 0.0f, sm2_start_time = 0.0f,
      sm2_last_time = 0.0f;  // 用于记录SM2的周期时间
#endif
// MainLoop 部分
void Ecatapp_Loop(void) {
  /*
    然后在这里面做不同模式的区分
  */
#if defined(MixedMode) && defined(DOIT_INLOOP)
  /* Mixed模式，则在中断中置标志位，然后在loop中轮询标志位进行检查 */
  if (sync0_isr_flag_) {
    ESC_updateALevent();
    DIG_process(DIG_PROCESS_APP_HOOK_FLAG | DIG_PROCESS_INPUTS_FLAG);
    sync0_isr_flag_ = 0;
    // 闪烁指示
    toggle_flash(&debug_sync0, 13, 1000, 100, 4);
    // 只有Pdi中断触发，会处理DIG_PROCESS_OUTPUTS_FLAG
  }
  if (pdi_isr_flag_) {
    ESC_updateALevent();
    if (ESCvar.ALevent & ESCREG_ALEVENT_SM2) {
#ifdef TEST_TIME
      sm2_start_time = DWT_GetTimeline_ms();
      sm2_cycle_time = sm2_start_time - sm2_last_time;  // 计算SM2中断周期时间
      sm2_last_time = sm2_start_time;                   // 更新SM2中断结束时间
#endif
      if (ESCvar.dcsync == 0) {
        // If DC sync is not active, run the application, all except for
        // the Watchdog 如果DC同步未激活，运行应用程序，但监视器除外
#ifdef TEST_TIME
        start_copy_output_data = DWT_GetTimeline_us();
#endif
        DIG_process(DIG_PROCESS_OUTPUTS_FLAG | DIG_PROCESS_APP_HOOK_FLAG |
                    DIG_PROCESS_INPUTS_FLAG | DIG_PROCESS_WD_FLAG);
#ifdef TEST_TIME
        copy_ouput_data_runtime = DWT_GetTimeline_us() - start_copy_output_data;
#endif
      } else {
        DIG_process(DIG_PROCESS_OUTPUTS_FLAG |
                    DIG_PROCESS_WD_FLAG);  // If DC sync is active, call
      }  // output handler only
      // 闪烁指示
      toggle_flash(&debug_pdi, 14, 1000, 100, 4);
    }
    pdi_isr_flag_ = 0;
  } else {
    ecat_slv_poll();
    DIG_process(DIG_PROCESS_WD_FLAG | DIG_PROCESS_OUTPUTS_FLAG);
  }
#endif

#if defined(MixedMode) && !defined(DOIT_INLOOP)
  /* 直接在中断中处理程序，主程序跑一个看门狗一样的东西 */
  if (debug_sync0 > 100) {
    // HAL_GPIO_TogglePin(LED4_GPIO_Port, LED4_Pin);
    PCToggle(14);
    debug_sync0 = 0;
  }
  if (debug_pdi > 100) {
    // HAL_GPIO_TogglePin(LED5_GPIO_Port, LED5_Pin);
    PCToggle(15);
    debug_pdi = 0;
  }
  ecat_slv_poll();
  DIG_process(DIG_PROCESS_WD_FLAG);
#endif

#ifdef InterruptMode
  ecat_slv_worker(ESCREG_ALEVENT_CONTROL | ESCREG_ALEVENT_SMCHANGE |
                  ESCREG_ALEVENT_SM0 | ESCREG_ALEVENT_SM1);
  DIG_process(DIG_PROCESS_WD_FLAG);
#endif

#ifdef FreeRun

#ifdef TEST_TIME
  loop_start = DWT_GetTimeline_ms();
#endif

  ecat_slv_poll();

#ifdef TEST_TIME
  func_1_runtime =
      DWT_GetTimeline_ms() - loop_start;  // 计算ecat_slv_poll运行时间
  loop_start = DWT_GetTimeline_ms();
#endif

  // 可以看到下面处理了输入输出以及看门狗，这就是所说的不依赖外部中断的方式（FreeRun模式）
  DIG_process(DIG_PROCESS_WD_FLAG | DIG_PROCESS_OUTPUTS_FLAG |
              DIG_PROCESS_INPUTS_FLAG);
#ifdef TEST_TIME
  func_2_runtime =
      DWT_GetTimeline_ms() - loop_start;  // 计算DIG_process运行时间
#endif
#endif
}

void CAN0_RxCpltCallback(CAN_HandleTypeDef *hcan,
                         CAN_RxHeaderTypeDef *temp_rxheader,
                         const uint8_t *data) {
  _can0_receive_callback(hcan, temp_rxheader, data);
}

#ifndef ONLY_CAN0
void CAN1_RxCpltCallback(CAN_HandleTypeDef *hcan,
                         CAN_RxHeaderTypeDef *temp_rxheader,
                         const uint8_t *data) {
  _can1_receive_callback(hcan, temp_rxheader, data);
}
#endif

#ifdef TEST_TIME
int64_t debug_ = 0;
float sync0_cycle_time = 0.0f, sync0_start_time = 0.0f,
      sync0_last_time = 0.0f;  // 用于记录sync0的周期时间
float sync1_cycle_time = 0.0f, sync1_start_time = 0.0f,
      sync1_last_time = 0.0f;  // 用于记录sync1的周期时间
float pdi_cycle_time = 0.0f, pdi_start_time = 0.0f,
      pdi_last_time = 0.0f;  // 用于记录pdi的周期时间
#endif
/**
 * @brief 外部中断回调重载
 *
 * @param GPIO_Pin
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
#if defined(MixedMode) && defined(DOIT_INLOOP)
  /* 检测是否有PDI中断 */
  if (GPIO_Pin == PDI_IRQ_Pin) {
    pdi_isr_flag_ = 1;
#ifdef TEST_TIME
    pdi_start_time = DWT_GetTimeline_ms();
    pdi_cycle_time = pdi_start_time - pdi_last_time;  // 计算PDI中断周期时间
    pdi_last_time = pdi_start_time;                   // 更新PDI中断结束时间
#endif
  } else if (GPIO_Pin == SYNC0_IRQ_Pin) {
    sync0_isr_flag_ = 1;
#ifdef TEST_TIME
    sync0_start_time = DWT_GetTimeline_ms();
    sync0_cycle_time =
        sync0_start_time - sync0_last_time;  // 计算SYNC0中断周期时间
    sync0_last_time = sync0_start_time;      // 更新SYNC0中断结束时间
#endif
  } else if (GPIO_Pin == SYNC1_IRQ_Pin) {
    sync1_isr_flag_ = 1;
#ifdef TEST_TIME
    sync1_start_time = DWT_GetTimeline_ms();
    sync1_cycle_time =
        sync1_start_time - sync1_last_time;  // 计算SYNC1中断周期时间
    sync1_last_time = sync1_start_time;      // 更新SYNC1中断结束时间
#endif
  }
#endif

#if defined(MixedMode) && !defined(DOIT_INLOOP)
  if (GPIO_Pin == SYNC0_IRQ_Pin) {
    ESC_updateALevent();
    DIG_process(DIG_PROCESS_APP_HOOK_FLAG | DIG_PROCESS_INPUTS_FLAG);
    debug_sync0++;
  }
  // 只有Pdi中断触发，会处理DIG_PROCESS_OUTPUTS_FLAG
  if (GPIO_Pin == PDI_IRQ_Pin) {
    ESC_updateALevent();
    if (ESCvar.ALevent & ESCREG_ALEVENT_SM2) {
      if (ESCvar.dcsync == 0) {
        // If DC sync is not active, run the application, all except for
        // the Watchdog 如果DC同步未激活，运行应用程序，但监视器除外
        DIG_process(DIG_PROCESS_OUTPUTS_FLAG | DIG_PROCESS_APP_HOOK_FLAG |
                    DIG_PROCESS_INPUTS_FLAG);
      } else {
        DIG_process(DIG_PROCESS_OUTPUTS_FLAG);  // If DC sync is active, call
      }  // output handler only
    }
    debug_pdi++;
  }
#endif

#ifdef InterruptMode
  if (GPIO_Pin == PDI_IRQ_Pin) {
    pdi_isr_flag_ = 1;
  } else if (GPIO_Pin == SYNC0_IRQ_Pin) {
    sync0_isr_flag_ = 1;
  } else if (GPIO_Pin == SYNC1_IRQ_Pin) {
    sync1_isr_flag_ = 1;
  }
#endif

#ifdef FreeRun
  /* 指示一下FreeRun模式下是否PDI中断正常触发 */
  if (GPIO_Pin == PDI_IRQ_Pin) {
    if (debug_++ > 100) {
      // HAL_GPIO_TogglePin(LED5_GPIO_Port, LED5_Pin);
      PCToggle(15);
      debug_ = 0;
    }
  }
#endif
}

/**
 * @brief 定时中断回调
 *        ecat_slv_poll() get called regular to handle stack operations
 *        DIG_process(DIG_PROCESS_WD_FLAG) get called regular to kick the
 *        software watchdog counter Or the hardware PDI interrupt watchdog
 *        should be activated
 *
 *        2025-6-25 添加LL分支，LL分支将使用3个独立的函数地址映射
 *
 *
 * @param htim
 */
#ifndef USE_LL_DRIVER
#ifdef TEST_TIME
float tim_current = 0, tim_last = 0;
float tim_dt = 0;
float tim_start = 0;
float tim_run_dt = 0;
#endif

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (htim == &ESC_LOCAL_TIM) {
#ifdef TEST_TIME
    tim_start = DWT_GetTimeline_ms();
    tim_current = DWT_GetTimeline_ms();
    tim_dt = tim_current - tim_last;
    tim_last = tim_current;
#endif

#ifdef MixedMode
    // DIG_process(DIG_PROCESS_WD_FLAG);
    if ((ESCvar.ALstatus & (ESCsafeop | ESCerror)) == (ESCsafeop | ESCerror)) {
      // 直接硬件复位
      HAL_NVIC_SystemReset();
    }

    /* watch dog waiting */
    // no_pdi_msgs_++;
    no_sync0_msgs_++;
    if (no_sync0_msgs_ > 10000) {
      // 硬件复位，重头再来
      HAL_NVIC_SystemReset();
    }
#endif

#ifdef TEST_TIME
    tim_run_dt = DWT_GetTimeline_ms() - tim_start;  // 计算本次定时器运行的时间
#endif
  }
#ifdef IF_USE_SENDQUEUE
  else if (htim == &CAN0_TASK_TIM_INSTANCE) {
    /* 处理can发送任务，发送can0和can1的队列数据 */
    ProcessCanQueue(1);
  } else if (htim == &CAN1_TASK_TIM_INSTANCE) {
    ProcessCanQueue(0);
  }
#endif
}

#else
extern int if_ws2812_init;  // ws2812灯条初始化标志位

enum WS2812_LED_Status main_led_task = LAUNCH_BREATH;

void LL_Local_TimerISR(void) {
  // 本地时钟看门狗
#ifdef MixedMode

#endif

  // 顺便做can总线监测
  CAN_Watchdog_Task();

  // 在这里做RGB灯条状态更新

  static WS2812_BeamEffect_t beam;
  if (!if_ws2812_init) {
  } else {
    switch (main_led_task) {
      case STOP_ACT: {
        main_led_task = StopLEDTask();
        break;
      }
      case LAUNCH_BREATH: {
        main_led_task = LaunchLEDTask();
        break;
      }
      case DAEMON_MODE: {
        main_led_task = DaemonTask();
        break;
      }
      case SHOOT_MODE: {
        main_led_task = ShootTask();
        break;
      }
      default:
        break;
    }
  }

  if (set_encoder_zero) {
    set_encoder_zero = 0;
    Encoder_SetZero();
  }
  LL_TIM_ClearFlag_UPDATE(LOCAL_TIM);
}

void LL_CAN0_Task_TimerISR(void) {
  // can0 发送任务
#ifdef IF_USE_SENDQUEUE
  ProcessCanQueue(1);  // 处理can0发送队列
#endif
  LL_TIM_ClearFlag_UPDATE(CAN0_TASK_TIM);
}

void LL_CAN1_Task_TimerISR(void) {
  // can1 发送任务
#ifdef IF_USE_SENDQUEUE
  ProcessCanQueue(0);  // 处理can1发送队列
#endif
  LL_TIM_ClearFlag_UPDATE(CAN1_TASK_TIM);
}

#endif

/**
 * @brief 这个函数用于FreeRun模式下对看门狗的自动重装载
 *        防止看门狗触发
 *
 */
void ESC_dc_watchdog_feed(void) { APP_setwatchdog(ESCvar.watchdogcnt); }

/**
 * @brief 解析出float 类型的转子位置
 *
 * @param pos_structor_
 * @param can_rx_data
 */
void handle_dji_motor_pos(DJI_Motor_Position_t *pos_structor_,
                          uint8_t can_rx_data[]) {
  pos_structor_->cur_encoder_ =
      (int16_t)((can_rx_data[0] << 8u) | can_rx_data[1]);
  int16_t delta_encoder =
      pos_structor_->cur_encoder_ - pos_structor_->last_encoder_;
  if (delta_encoder < -DJI_MOTOR_TOTAL_ENCODER / 2) {
    pos_structor_->round_cnt_++;
  } else if (delta_encoder > DJI_MOTOR_TOTAL_ENCODER / 2) {
    pos_structor_->round_cnt_--;
  }
  int32_t total_encoder = pos_structor_->round_cnt_ * DJI_MOTOR_TOTAL_ENCODER +
                          pos_structor_->cur_encoder_;
  pos_structor_->last_encoder_ = pos_structor_->cur_encoder_;
  pos_structor_->angle_splitter_.f =
      (float)(total_encoder * DJI_ENCODER_ANGLE_RATIO);
}

/**
 * @brief 写一些看门狗操作
 *
 */
void CAN0_watchdogCallback(void) { can0_no_msg = true; }

/**
 * @brief 写一些看门狗操作
 *
 */
void CAN1_watchdogCallback(void) { can1_no_msg = true; }

void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan) {
#ifdef TEST_TIME
  // 发送完成回调
  CAN_QueueInstance *pQueue = (hcan == &CAN0_Handle) ? &can0Queue : &can1Queue;
  pQueue->send_cycle_time =
      DWT_GetTimeline_ms() - pQueue->send_cycle_last_time;  // 计算发送周期时间
  pQueue->send_cycle_last_time = DWT_GetTimeline_ms();      // 更新发送周期时间
  real_tx_success++;
#endif

#ifdef USE_CAN_TX_IDLE_INTERRUPT
  if (hcan == &CAN0_Handle) {
    ProcessCanQueue(1);
  } else if (hcan == &CAN1_Handle) {
    ProcessCanQueue(0);
  }
#endif
}

void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan) {
#ifdef TEST_TIME
  CAN_QueueInstance *pQueue = (hcan == &CAN0_Handle) ? &can0Queue : &can1Queue;
  pQueue->send_cycle_time =
      DWT_GetTimeline_ms() - pQueue->send_cycle_last_time;  // 计算发送周期时间
  pQueue->send_cycle_last_time = DWT_GetTimeline_ms();      // 更新发送周期时间
  real_tx_success++;
#endif

#ifdef USE_CAN_TX_IDLE_INTERRUPT
  if (hcan == &CAN0_Handle) {
    ProcessCanQueue(1);
  } else if (hcan == &CAN1_Handle) {
    ProcessCanQueue(0);
  }
#endif
}

void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan) {
#ifdef TEST_TIME
  // 发送完成回调
  CAN_QueueInstance *pQueue = (hcan == &CAN0_Handle) ? &can0Queue : &can1Queue;
  pQueue->send_cycle_time =
      DWT_GetTimeline_ms() - pQueue->send_cycle_last_time;  // 计算发送周期时间
  pQueue->send_cycle_last_time = DWT_GetTimeline_ms();      // 更新发送周期时间
  real_tx_success++;
#endif

#ifdef USE_CAN_TX_IDLE_INTERRUPT
  if (hcan == &CAN0_Handle) {
    ProcessCanQueue(1);
  } else if (hcan == &CAN1_Handle) {
    ProcessCanQueue(0);
  }
#endif
}