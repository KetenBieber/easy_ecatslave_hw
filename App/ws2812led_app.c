/**
 * @file ws2812led_app.c
 * @author Keten (2863861004@qq.com)
 * @brief rgb灯带驱动
 * @version 0.1
 * @date 2025-07-17
 *
 * @copyright Copyright (c) 2025
 *
 * @attention :
 * @note : 预计使用定时器，传入时间戳，完全实现非阻塞式控制
 *
 *          左右侧灯带必定是反向的关系,所以需要做映射,将某一侧的灯带映射成反向,从而使得start
 * 不会大于end
 *
 *
 *  @note : ethercat 主站启动成功通讯之后,会开启开机动画
 *
 * @versioninfo :
 */
#include "ws2812led_app.h"

/* global varibals */

// 显存数组，长度为 灯的数量*24+复位周期
uint16_t WS2812_RGB_Buff[LED_NUM * DATA_LEN + WS2812_RST_NUM] = {0};
int if_ws2812_init = 0;     // WS2812是否初始化标志位
int switch_cur_state = -2;  // 指示当前

extern bool can0_no_msg;
extern bool can1_no_msg;

int if_shoot = 0;  // 是否开火标志位

static inline uint16_t WS2812_MapLED(uint16_t idx) {
  if (idx < LEFT_LED_NUM) {
    return idx;
  } else if (idx >= LEFT_LED_NUM) {
    return RIGHT_LED_START + idx - LEFT_LED_NUM;
  } else if (idx > LED_NUM) {
    // 临界保护
    return 0;
  }
}

// WS2812初始化函数
void WS2812_Init() {
  // 初始化DMA部分
  WS2812_DMA_Configuration();

  // 初始化定时器部分
  WS2812_TIM_Configuration();

  // 设置关闭所有灯
  for (int i = 0; i < LED_NUM; i++) {
    WS2812_SetSingle(i, 0, 0, 0);
  }
  // 作用：调用DMA将显存中的内容实时搬运至定时器的比较寄存器
  HAL_TIM_PWM_Start_DMA(&WS2812_TIM, WS2812_TIM_Channel,
                        (uint32_t *)WS2812_RGB_Buff,
                        LED_NUM * DATA_LEN + WS2812_RST_NUM);
  if_ws2812_init = 1;
}

// 打包完调用此函数，开始显示数据
void WS2812_Show(void) {
  // 普通模式 DMA：传输完成后会触发 HAL_TIM_PWM_PulseFinishedCallback
  HAL_TIM_PWM_Start_DMA(&WS2812_TIM, WS2812_TIM_Channel,
                        (uint32_t *)WS2812_RGB_Buff,
                        LED_NUM * DATA_LEN + WS2812_RST_NUM);
}

/* 只写 buffer，不发帧 */
void WS2812_SetPixel(uint16_t idx, uint8_t R, uint8_t G, uint8_t B) {
  uint32_t base = idx * DATA_LEN;
  for (uint8_t i = 0; i < 8; i++) {
    WS2812_RGB_Buff[base + i] = (G << i & 0x80) ? WS_H : WS_L;
    WS2812_RGB_Buff[base + 8 + i] = (R << i & 0x80) ? WS_H : WS_L;
    WS2812_RGB_Buff[base + 16 + i] = (B << i & 0x80) ? WS_H : WS_L;
  }
}

/**
 * 函数：WS2812单灯设置函数，设置完立即发送
 * 参数：num:灯的位置，R、G、B分别为三个颜色通道的亮度，最大值为255
 * 作用：单独设置每一个WS2812的颜色
 ***/
void WS2812_SetSingle(uint16_t num, uint8_t R, uint8_t G, uint8_t B) {
  uint32_t indexx = num * 24;
  for (uint8_t i = 0; i < 8; i++) {
    // 填充数组
    WS2812_RGB_Buff[indexx + i] = (G << i) & (0x80) ? WS_H : WS_L;
    WS2812_RGB_Buff[indexx + i + 8] = (R << i) & (0x80) ? WS_H : WS_L;
    WS2812_RGB_Buff[indexx + i + 16] = (B << i) & (0x80) ? WS_H : WS_L;
  }
  // 作用：调用DMA将显存中的内容实时搬运至定时器的比较寄存器
  WS2812_Show();
}

// 一次设置多个led等的颜色
void WS2812_SetMultiple(uint16_t start, uint16_t end, uint8_t R, uint8_t G,
                        uint8_t B) {
  // 设置多个灯的颜色
  int lo = start < end ? start : end;
  int hi = start < end ? end : start;
  for (int i = lo - 1; i < hi; i++) {
    WS2812_SetSingle(i, R, G, B);
  }
  // 发送
  WS2812_Show();
}

// 清空所有灯的颜色
void WS2812_ClearAll(void) {
  // 清空所有灯的颜色
  for (int i = 0; i <= LED_NUM; i++) {
    WS2812_SetPixel(i, 0, 0, 0);
  }
  // 发送
  WS2812_Show();
}

void WS2812_ClearMultiple(uint16_t start, uint16_t end) {
  int lo = start < end ? start : end;
  int hi = start < end ? end : start;
  for (int i = lo; i <= hi; i++) {
    WS2812_SetPixel(i, 0, 0, 0);
  }
  WS2812_Show();
}

// 初始化 光束效果
void WS2812_BeamEffectInit(WS2812_BeamEffect_t *eff, float speed, int trail_len,
                           uint8_t sR, uint8_t sG, uint8_t sB, uint8_t eR,
                           uint8_t eG, uint8_t eB, int start_led, int end_led) {
  // 初始化光束效果实例
  // eff->pos = ((end_led > start_led) ? (-trail_len)
  //                                   : (start_led - end_led + trail_len));
  eff->pos = 0;
  eff->speed = speed;
  eff->trail_len = trail_len;
  eff->startRGB[0] = sR;
  eff->startRGB[1] = sG;
  eff->startRGB[2] = sB;
  eff->endRGB[0] = eR;
  eff->endRGB[1] = eG;
  eff->endRGB[2] = eB;
  eff->start_led = start_led;  // 起始LED编号
  eff->end_led = end_led;      // 结束LED编号
}

// 光束效果运行函数，direction >= 0 为正向，< 0 为反向，需被定期调用
// 仅适用于单端灯条控制
int WS2812_BeamEffectRun(WS2812_BeamEffect_t *eff) {
  if (!if_ws2812_init) return 0;

  // 根据 start_led 与 end_led 定义控制区域
  int lo = (eff->start_led < eff->end_led) ? eff->start_led : eff->end_led;
  int hi = (eff->start_led < eff->end_led) ? eff->end_led : eff->start_led;
  int len = hi - lo + 1;
  int dir = (eff->end_led > eff->start_led) ? 1 : -1;  // 正向：1，反向：-1

  // 仅清空控制区域内的 LED状态
  for (int i = lo; i <= hi; i++) {
    WS2812_SetPixel(i, 0, 0, 0);
  }
  WS2812_Show();

  // 根据方向计算当前光束"头"在区域内的 LED 索引
  int head = (dir > 0) ? (lo + (int)eff->pos) : (hi - (int)eff->pos);

  // 绘制从 head 开始的尾迹（只在区域内绘制）
  for (int t = 0; t < eff->trail_len; t++) {
    int led;
    if (dir > 0) {
      if (head - t < lo) continue;  // 超出区域下界
      led = head - t;
    } else {
      if (head + t > hi) continue;  // 超出区域上界
      led = head + t;
    }
    // 如果计算出的索引不在控制区域内，跳过
    if (led < lo || led > hi) continue;

    float ratio = (eff->trail_len > 1) ? ((float)t / (eff->trail_len - 1)) : 0;
    uint8_t r =
        (uint8_t)(eff->startRGB[0] * (1 - ratio) + eff->endRGB[0] * ratio);
    uint8_t g =
        (uint8_t)(eff->startRGB[1] * (1 - ratio) + eff->endRGB[1] * ratio);
    uint8_t b =
        (uint8_t)(eff->startRGB[2] * (1 - ratio) + eff->endRGB[2] * ratio);
    WS2812_SetPixel(led, r, g, b);
  }
  WS2812_Show();

  // 累加 pos，当 pos 超出区域（加上尾迹）时回绕重置
  eff->pos += eff->speed;
  if (eff->pos >= (len + eff->trail_len)) {
    eff->pos = 0.0f;
    return 1;  // 完成一次循环
  }
  return 0;
}

// 新增一个不调用 WS2812_Show() 的光束更新函数，用于同时写入多个区域
int WS2812_BeamEffectRunNoShow(WS2812_BeamEffect_t *eff) {
  if (!if_ws2812_init) return 0;

  // 根据 start_led 与 end_led 定义控制区域
  int lo = (eff->start_led < eff->end_led) ? eff->start_led : eff->end_led;
  int hi = (eff->start_led < eff->end_led) ? eff->end_led : eff->start_led;
  int len = hi - lo + 1;
  int dir = (eff->end_led > eff->start_led) ? 1 : -1;  // 正向：1，反向：-1

  // 清空区域内 LED（仅更新缓冲区，不触发显示）
  for (int i = lo; i <= hi; i++) {
    WS2812_SetPixel(i, 0, 0, 0);
  }

  // 根据方向计算当前光束 "头" 的 LED 编号
  int head = (dir > 0) ? (lo + (int)eff->pos) : (hi - (int)eff->pos);

  // 绘制从 head 开始的尾迹（只更新缓冲区）
  for (int t = 0; t < eff->trail_len; t++) {
    int led;
    if (dir > 0) {
      if (head - t < lo) continue;  // 超出区域下界
      led = head - t;
    } else {
      if (head + t > hi) continue;  // 超出区域上界
      led = head + t;
    }
    // 如果计算出的区域索引不在 [lo, hi] 内，跳过
    if (led < lo || led > hi) continue;

    float ratio = (eff->trail_len > 1) ? ((float)t / (eff->trail_len - 1)) : 0;
    uint8_t r =
        (uint8_t)(eff->startRGB[0] * (1 - ratio) + eff->endRGB[0] * ratio);
    uint8_t g =
        (uint8_t)(eff->startRGB[1] * (1 - ratio) + eff->endRGB[1] * ratio);
    uint8_t b =
        (uint8_t)(eff->startRGB[2] * (1 - ratio) + eff->endRGB[2] * ratio);
    WS2812_SetPixel(led, r, g, b);
  }
  // 更新 pos（不调用 WS2812_Show()）
  eff->pos += eff->speed;
  if (eff->pos >= (len + eff->trail_len)) {
    eff->pos = 0.0f;
    return 1;  // 完成一次循环
  }
  return 0;
}

int WS2812_BeamEffectKeepRun(WS2812_BeamEffect_t *eff) {
  if (!if_ws2812_init) return 0;

  // 使用 start_led 为起始参考，不再取 min/max
  int start = eff->start_led;
  int end = eff->end_led;
  // 计算区域长度：包括两端
  int len = (end >= start) ? (end - start + 1) : (start - end + 1);
  // 方向：正向=1（从 start 到 end），反向=-1（从 start 向下走）
  int dir = (end >= start) ? 1 : -1;

  // 计算当前光束“头”在区域中的实际 LED 编号
  int head = (dir > 0) ? (start + (int)eff->pos) : (start - (int)eff->pos);
  // 范围检测：确保 head 落在 start 到 end 之间（无论顺序如何）
  if (dir > 0) {
    if (head > end) head = end;
  } else {
    if (head < end) head = end;
  }
  // 点亮当前 LED，不清除已有显示，由外部统一 WS2812_Show() 发帧
  WS2812_SetPixel(head, eff->startRGB[0], eff->startRGB[1], eff->startRGB[2]);

  // 更新光束位置，方向由 dir 决定
  eff->pos += eff->speed * (dir > 0 ? 1.0f : 1.0f);
  // 当光束走出区域（加上尾迹长度）后回绕重置
  if (dir > 0) {
    if (eff->pos >= (len + eff->trail_len)) {
      eff->pos = 0.0f;
      return 1;  // 完成一次循环
    }
  } else {
    if (eff->pos >=
        (len + eff->trail_len)) {  // 这里累加同样累加正数，但实际是反向计数
      eff->pos = 0.0f;
      return 1;
    }
  }
  return 0;
}

/*
  呼吸灯需要保留什么东西？当前亮度，什么颜色的渐变？初始颜色和结尾态颜色
*/

// 生成呼吸灯效果
int WS2812LED_BreathGenerate(WS2812_Breath_t *breath) {
  // 1. 用 sinf 生成 [0,1] 间 t
  float t = (sinf(breath->phase) + 1.0f) * 0.5f;
  // 2. 线性插值 startRGB→endRGB
  for (int i = 0; i < 3; i++) {
    breath->currRGB[i] =
        breath->startRGB[i] +
        (uint8_t)((breath->endRGB[i] - breath->startRGB[i]) * t);
  }
  // 3. 填 buffer（不马上发帧）
  WS2812_SetMultiple(breath->start_led, breath->end_led, breath->currRGB[0],
                     breath->currRGB[1], breath->currRGB[2]);
  // 4. 更新相位，并循环 [0,2π)
  float prev_phase = breath->phase;
  breath->phase += breath->speed;
  if (breath->phase >= 2.0f * 3.1415926f) {
    breath->phase -= 2.0f * 3.1415926f;
  }

  // 5. 检测跨越 π/2(全亮) 和 3π/2(全暗) 时机
  float half_pi = 0.5f * 3.1415926f;
  float three_half_pi = 1.5f * 3.1415926f;
  if (prev_phase < half_pi && breath->phase >= half_pi) {
    return 2;  // 达到全亮
  }
  if (prev_phase < three_half_pi && breath->phase >= three_half_pi) {
    return 1;  // 达到全暗
  }
  return 0;
}

// PWM DMA 完成回调函数 停止
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
  if (htim == &htim8) {
    HAL_TIM_PWM_Stop_DMA(&htim8, TIM_CHANNEL_2);
  }
}

// app层 主站关闭通信接口任务
int StopLEDTask(void) {
  static int stop_task = -1;
  static WS2812_BeamEffect_t stop_beam1, stop_beam2;
  static WS2812_Breath_t stop_led = {
      .startRGB = {0, 0, 0},
      .endRGB = {255 * DIVIDE_PARAM, 0, 0},  // 红色
      .currRGB = {0, 0, 0},
      .start_led = 1,
      .end_led = LED_NUM,
      .speed = 0.0018,
      .phase = 0,
  };
  switch (stop_task) {
    case -1: {
      WS2812_BeamEffectInit(&stop_beam1, 0.03, 20, 255 * DIVIDE_PARAM, 0, 0, 0,
                            0, 0, 0, 30);
      WS2812_BeamEffectInit(&stop_beam2, 0.03, 20, 255 * DIVIDE_PARAM, 0, 0, 0,
                            0, 0, 60, 31);
      stop_task = 0;
      break;
    }
    case 0: {
      static int cnt_0 = 0;
      int r1 = WS2812_BeamEffectKeepRun(&stop_beam1);
      int r2 = WS2812_BeamEffectKeepRun(&stop_beam2);
      WS2812_Show();
      if (r1 || r2) {
        if (cnt_0++ >= 5) {
          stop_task = 1;
          cnt_0 = 0;
        }
      }
      break;
    }
    case 1: {
      if (WS2812LED_BreathGenerate(&stop_led)) {
      }
      break;
    }
  }

  // 如果状态即将要改变,注意初始化状态机动画,因为下次还要再重头播放一次
  if (switch_cur_state != STOP_ACT) {
    stop_task = -1;
  }
  return switch_cur_state;
}

// app层 ,启动函数
int LaunchLEDTask(void) {
  static int launch_task = -1;
  static WS2812_Breath_t test_led = {
      .startRGB = {0, 0, 0},
      .endRGB = {87 * DIVIDE_PARAM, 0, 154 * DIVIDE_PARAM},  // 紫色
      .currRGB = {0, 0, 0},
      .start_led = 1,
      .end_led = LED_NUM,
      .speed = 0.0018,
      .phase = 0,
  };
  static WS2812_BeamEffect_t beam1;
  static WS2812_BeamEffect_t beam2;
  static int cnt_3 = 0;
  static int cnt = 0;
  switch (launch_task) {
    case -1: {
      WS2812_BeamEffectInit(&beam1, 0.03f, 10, 68 * DIVIDE_PARAM, 0,
                            141 * DIVIDE_PARAM, 0, 0, 0, 1, 30);
      WS2812_BeamEffectInit(&beam2, 0.03f, 10, 68 * DIVIDE_PARAM, 0,
                            141 * DIVIDE_PARAM, 0, 0, 0, 60, 31);
      launch_task = 0;
      break;
    }
    case 0: {
      // 上刷
      int r1 = WS2812_BeamEffectRunNoShow(&beam1);
      int r2 = WS2812_BeamEffectRunNoShow(&beam2);
      // 统一调用一次显示，确保两侧同时更新
      WS2812_Show();
      if (r1 || r2) {
        launch_task = 1;  // 切换到下一个状态
      }
      break;
    }
    case 1: {
      // 设置反向
      WS2812_BeamEffectInit(&beam1, 0.03f, 10, 68 * DIVIDE_PARAM, 0,
                            141 * DIVIDE_PARAM, 0, 0, 0, 30, 0);
      WS2812_BeamEffectInit(&beam2, 0.03f, 10, 68 * DIVIDE_PARAM, 0,
                            141 * DIVIDE_PARAM, 0, 0, 0, 31, 60);
      launch_task = 2;
      break;
    }
    case 2: {
      // 下刷
      int r1 = WS2812_BeamEffectRunNoShow(&beam1);
      int r2 = WS2812_BeamEffectRunNoShow(&beam2);
      WS2812_Show();
      if (r1 || r2) {
        launch_task = 3;  // 切换到下一个状态
      }
      break;
    }
    case 3: {
      // 上刷蓄能
      int f1 = WS2812_BeamEffectKeepRun(&beam1);
      int f2 = WS2812_BeamEffectKeepRun(&beam2);
      WS2812_Show();
      if (f1 || f2) {
        if (cnt_3++ >= 5) {
          launch_task = 4;
          cnt_3 = 0;
        }
      }
      break;
    }
    case 4: {
      if (WS2812LED_BreathGenerate(&test_led) == 1) {
        if (cnt++ >= 1) {
          launch_task = -1;
          cnt = 0;
          switch_cur_state = DAEMON_MODE;  // 切换到守护模式
        }
      }
      break;
    }
  }

  if (switch_cur_state != LAUNCH_BREATH) {
    cnt_3 = 0;
    cnt = 0;
    launch_task = -1;
  }

  return switch_cur_state;
}

// app层,守护线程函数
int DaemonTask(void) {
  static WS2812_Breath_t alarm_led = {
      .startRGB = {0, 0, 0},
      .endRGB = {87 * DIVIDE_PARAM, 0, 154 * DIVIDE_PARAM},  // 紫色
      .currRGB = {0, 0, 0},
      .start_led = 1,
      .end_led = LED_NUM,
      .speed = 0.0025,
      .phase = 0,
  };

  // 根据 can0_no_msg 和 can1_no_msg 判断不同状态
  if (can0_no_msg && !can1_no_msg) {
    // 当只有can0无消息时，采用一种动画效果
    alarm_led.endRGB[0] = 255 * DIVIDE_PARAM;
    alarm_led.endRGB[1] = 100 * DIVIDE_PARAM;
    alarm_led.endRGB[2] = 0;
    alarm_led.speed = 0.0050;
    WS2812LED_BreathGenerate(&alarm_led);
  } else if (!can0_no_msg && can1_no_msg) {
    // 当只有can1无消息时，采用另一种动画效果（这里可以更换颜色或速度）
    // 例如修改 test_led 的颜色参数，然后生成动画
    alarm_led.endRGB[0] = 255 * DIVIDE_PARAM;
    alarm_led.endRGB[1] = 100 * DIVIDE_PARAM;
    alarm_led.endRGB[2] = 0;
    alarm_led.speed = 0.01;
    WS2812LED_BreathGenerate(&alarm_led);
  } else if (can0_no_msg && can1_no_msg) {
    // 两路均无消息时，采用默认动画，或者同时调用两种效果的混合
    alarm_led.endRGB[0] = 255 * DIVIDE_PARAM;
    alarm_led.endRGB[1] = 0;
    alarm_led.endRGB[2] = 0;
    alarm_led.speed = 0.01;
    WS2812LED_BreathGenerate(&alarm_led);
  } else {
    // 正常情况下，或消息恢复时，紫色呼吸灯
    alarm_led.endRGB[0] = 87 * DIVIDE_PARAM;
    alarm_led.endRGB[1] = 0;
    alarm_led.endRGB[2] = 154 * DIVIDE_PARAM;
    alarm_led.speed = 0.0025;  // 恢复正常呼吸灯速度
    WS2812LED_BreathGenerate(&alarm_led);
  }

  if (if_shoot == 1) {              // 设置为1 ,开始进入shoot 任务进行蓄能
    switch_cur_state = SHOOT_MODE;  // 如果开火标志位为真，切换到发射模式
  }
  return switch_cur_state;
}

//  app层,发射线程LED函数
int ShootTask(void) {
  static int shoot_task = -1;
  static WS2812_BeamEffect_t shoot_beam1;
  static WS2812_BeamEffect_t shoot_beam2;
  static int cnt_3 = 0;
  switch (shoot_task) {
    case -1: {
      WS2812_BeamEffectInit(&shoot_beam1, 0.05f, 10, 68 * DIVIDE_PARAM, 0,
                            141 * DIVIDE_PARAM, 0, 0, 0, 1, 30);
      WS2812_BeamEffectInit(&shoot_beam2, 0.05f, 10, 68 * DIVIDE_PARAM, 0,
                            141 * DIVIDE_PARAM, 0, 0, 0, 60, 31);
      shoot_task = 0;
      break;
    }
    case 0: {
      // 下刷保持蓄能效果
      int r1 = WS2812_BeamEffectKeepRun(&shoot_beam1);
      int r2 = WS2812_BeamEffectKeepRun(&shoot_beam2);
      WS2812_Show();
      if (r1 || r2) {
        if (if_shoot == 2) {
          shoot_task = 1;
        }
      }
      break;
    }
    case 1: {
      WS2812_BeamEffectInit(&shoot_beam1, 0.15f, 10, 0, 0, 0, 68 * DIVIDE_PARAM,
                            0, 141 * DIVIDE_PARAM, 30, 1);
      WS2812_BeamEffectInit(&shoot_beam2, 0.15f, 10, 0, 0, 0, 68 * DIVIDE_PARAM,
                            0, 141 * DIVIDE_PARAM, 31, 60);
      shoot_task = 2;
      break;
    }
    case 2: {
      // 上刷发射效果
      int f1 = WS2812_BeamEffectKeepRun(&shoot_beam1);
      int f2 = WS2812_BeamEffectKeepRun(&shoot_beam2);
      WS2812_Show();
      if (f1 || f2) {
        if (cnt_3++ >= 5) {
          shoot_task = -1;
          if_shoot = 0;  // 回到未发射状态
          cnt_3 = 0;
        }
      }
      break;
    }
  }

  if (if_shoot == 0) {  // 从发射状态回到守护线程状态
    switch_cur_state = DAEMON_MODE;
  }
  return switch_cur_state;
}

// case 4: {
//   WS2812_BeamEffectInit(&beam1, 0.03f, 10, 0, 0, 0, 68, 0, 141, 20, 3);
//   WS2812_BeamEffectInit(&beam2, 0.03f, 10, 0, 0, 0, 68, 0, 141, 21, 40);
//   launch_task = 5;
//   break;
// }
// case 5: {
//   static int cnt_5 = 0;
//   int f1 = WS2812_BeamEffectKeepRun(&beam1);
//   int f2 = WS2812_BeamEffectKeepRun(&beam2);
//   WS2812_Show();
//   if (f1 || f2) {
//     if (cnt_5++ >= 5) {
//       launch_task = 6;
//       cnt_5 = 0;
//     }
//   }
//   break;
// }