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
 * @versioninfo :
 */
#include "ecat_app.h"

#include "stm32f4xx_ll_tim.h"

/* global variable */
_Objects Obj;
uint8_t txpdo[MAX_TXPDO_SIZE] __attribute__((aligned(8)));  // 强制8字节对齐
uint8_t can_txData[8] = {0};
uint32_t no_master_msgs_cnt = 0;

/* sem to run the apploop */
static uint8_t pdi_isr_flag_ = 0;
static uint8_t sync0_isr_flag_ = 0;
static uint8_t sync1_isr_flag_ = 0;
uint32_t can0_tx_err_cnt = 0;
uint32_t can1_tx_err_cnt = 0;

extern CAN_QueueInstance can0Queue;
extern CAN_QueueInstance can1Queue;

/* struct for calculate */
DJI_Motor_Position_t dji_motor_pos_[8] = {0};  // 大疆电机位置数据

/* watch dog to watch the pdi and sync0 */
static uint32_t no_pdi_msgs_ = 0;
static uint32_t no_sync0_msgs_ = 0;

#define CAN_TIMEOUT_MS 1000  // 超时1000ms

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

  // Obj.input_io.io2 = PDin(7);
  // Obj.input_io.io3 = PGin(10);
  /* can总线上电机，将直接由can的接收中断进行打包 */
}

#ifdef DEBUG
float current_time = 0.0f, last_time = 0.0f;  // 用于记录当前时间，单位为秒
float dt;
float setOutput_run_time = 0.0f;
#endif
/**
 * @brief 处理RxPDO
 *
 */
void cb_set_outputs(void) {
  current_time = DWT_GetTimeline_ms();
  dt = current_time - last_time;
  last_time = current_time;
  /* 有8个可控io */
  //
  CAN0_TxHeader.StdId = 0x200;
  can_txData[0] = (Obj.can0_motor_commands[0] >> 8u);
  can_txData[1] = Obj.can0_motor_commands[0];
  can_txData[2] = (Obj.can0_motor_commands[1] >> 8u);
  can_txData[3] = Obj.can0_motor_commands[1];

  if (EnqueueCanMessage(&can0Queue, &CAN0_TxHeader, can_txData) != 0) {
    // 队列满时增加错误计数
    can0_tx_err_cnt++;
  }

#ifdef DEBUG
  setOutput_run_time = DWT_GetTimeline_ms() - current_time;
#endif
}

void _clear_statusword(void) {}

void setZeroOutputs(void) {
  // 发送空数据
  CAN0_TxHeader.StdId = 0x200;
  uint32_t tx_mailbox = 0;
  memset(can_txData, 0, sizeof(can_txData));
  if (HAL_CAN_AddTxMessage(&CAN0_Handle, &CAN0_TxHeader, can_txData,
                           &tx_mailbox) != HAL_OK)
    ;
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
void _safeoutput_callback(void) {}

/* core cfg */
static esc_cfg_t config = {
    .user_arg = "lan9252",
#if defined(MixedMode) || defined(InterruptMode)
    .use_interrupt = 1,
#else
    .use_interrupt = 1,
#endif
    .watchdog_cnt = 200,  // if TIM happened to be 1ms, this means 200ms
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
                            const uint8_t *data) {}

static uint32_t debug_pdi = 0;
static uint32_t debug_sync0 = 0;
/*
  使用LED4指示 sync0 是否正常工作
  使用LED5指示 pdi 是否正常工作
*/

#ifdef DEBUG
float func_1_runtime = 0, func_2_runtime = 0;
float loop_start = 0;
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
    if (debug_sync0++ > 200) {
      // HAL_GPIO_TogglePin(LED4_GPIO_Port, LED4_Pin);
      PCToggle(14);
      debug_sync0 = 0;
    }
  }
  // 只有Pdi中断触发，会处理DIG_PROCESS_OUTPUTS_FLAG
  if (pdi_isr_flag_) {
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
      if (debug_pdi++ > 200) {
        // HAL_GPIO_TogglePin(LED5_GPIO_Port, LED5_Pin);
        PCToggle(15);
        debug_pdi = 0;
      }
    }
    pdi_isr_flag_ = 0;
  } else {
    // ecat_slv_poll();
    ecat_slv();
    DIG_process(DIG_PROCESS_WD_FLAG);
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

#ifdef DEBUG
  loop_start = DWT_GetTimeline_ms();
#endif

  ecat_slv_poll();

#ifdef DEBUG
  func_1_runtime =
      DWT_GetTimeline_ms() - loop_start;  // 计算ecat_slv_poll运行时间
  loop_start = DWT_GetTimeline_ms();
#endif

  // 可以看到下面处理了输入输出以及看门狗，这就是所说的不依赖外部中断的方式（FreeRun模式）
  DIG_process(DIG_PROCESS_WD_FLAG | DIG_PROCESS_OUTPUTS_FLAG |
              DIG_PROCESS_INPUTS_FLAG);
#ifdef DEBUG
  func_2_runtime =
      DWT_GetTimeline_ms() - loop_start;  // 计算DIG_process运行时间
#endif
#endif
}

void pCAN0_RxCpltCallback(CAN_HandleTypeDef *hcan,
                          CAN_RxHeaderTypeDef *temp_rxheader,
                          const uint8_t *data) {
  _can0_receive_callback(hcan, temp_rxheader, data);
}

#ifndef ONLY_CAN0
void pCAN1_RxCpltCallback(CAN_HandleTypeDef *hcan,
                          CAN_RxHeaderTypeDef *temp_rxheader,
                          const uint8_t *data) {
  _can1_receive_callback(hcan, temp_rxheader, data);
}
#endif

int64_t debug_ = 0;
/**
 * @brief 外部中断回调
 *
 * @param GPIO_Pin
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
#if defined(MixedMode) && defined(DOIT_INLOOP)
  /* 检测是否有PDI中断 */
  if (GPIO_Pin == PDI_IRQ_Pin) {
    // ESC_updateALevent();
    // if (ESCvar.ALevent & ESCREG_ALEVENT_SM2) {
    pdi_isr_flag_ = 1;
    no_pdi_msgs_ = 0;  // 重置看门狗计数器
    // }
  } else if (GPIO_Pin == SYNC0_IRQ_Pin) {
    sync0_isr_flag_ = 1;
    no_sync0_msgs_ = 0;
  } else if (GPIO_Pin == SYNC1_IRQ_Pin) {
    sync1_isr_flag_ = 1;
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
#ifdef DEBUG
float tim_current = 0, tim_last = 0;
float tim_dt = 0;
float tim_start = 0;
float tim_run_dt = 0;
#endif

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (htim == &ESC_LOCAL_TIM) {
#ifdef DEBUG
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

#ifdef DEBUG
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
void LL_Local_TimerISR(void) {
  // 本地时钟看门狗
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
 * @brief （已弃用！）屏蔽其他事件使其只有SM2事件能够触发PDI
 *        不应该在这里直接屏蔽，应该在中断进入之后去检查
 *
 */
void ESC_MaskePDI_OnlySM2(void) {
  ESC_ALeventmaskwrite(MIXEDMODE_PDI_INT_MASK);
}

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