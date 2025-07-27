/**
 * @file ws2812led_app.h
 * @author Keten (2863861004@qq.com)
 * @brief rgb灯带驱动
 * @version 0.1
 * @date 2025-07-17
 *
 * @copyright Copyright (c) 2025
 *
 * @attention :
 * @note :
 * @versioninfo :
 */
#ifndef WS2812LED_APP_H
#define WS2812LED_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------include-----------------------------------*/
#include <stdbool.h>
#include <stdint.h>

#include "bsp_tim.h"

/*-----------------------------------macro------------------------------------*/
// R2上分布两条灯带,因此直接进行映射
#define LEFT_LED_NUM 30  // 左段的最后一个是编号20,灯是从1开始编号的
#define RIGHT_LED_START 31
#define RIGHT_LED_NUM 30
#define LED_NUM (LEFT_LED_NUM + RIGHT_LED_NUM)  // WS2812灯个数

#define WS_H 134     // 1 码相对计数值
#define WS_L 67      // 0 码相对计数值
#define WS_REST 40   // 复位信号脉冲数量
#define DATA_LEN 24  // WS2812数据长度，单个需要24个字节
#define WS2812_RST_NUM \
  50  // 官方复位时间为50us（40个周期），保险起见使用50个周期

#define DIVIDE_PARAM 0.5

/*----------------------------------typedef-----------------------------------*/
// 光束效果实例
typedef struct {
  float pos;      // 当前光头位置（可带小数）
  float speed;    // 每帧移动距离（LED 单位）
  int trail_len;  // 尾迹长度（LED 数）
  uint16_t start_led;
  uint16_t end_led;
  uint8_t startRGB[3];
  uint8_t endRGB[3];

} WS2812_BeamEffect_t;

typedef struct {
  uint8_t startRGB[3];
  uint8_t endRGB[3];
  uint8_t currRGB[3];
  uint16_t start_led;
  uint16_t end_led;
  float speed;  // 相位增量 (rad/frame)
  float phase;  // 当前相位 [0,2π)
} WS2812_Breath_t;

enum WS2812_LED_Status {
  STOP_ACT = -2,       // 关机动画
  LAUNCH_BREATH = -1,  // 开机呼吸灯特效
  DAEMON_MODE = 0,     // 守护模式，监测can总线上是否有设备掉线
  SHOOT_MODE = 1,      // 发射模式
};

/*----------------------------------variable----------------------------------*/
extern int if_ws2812_init;
extern int switch_cur_state;  // 指示当前
extern int if_shoot;          // 是否开火标志位

/*----------------------------------function----------------------------------*/
void WS2812_Init(void);
void WS2812_SetSingle(uint16_t num, uint8_t R, uint8_t G, uint8_t B);
void WS2812_BeamEffectInit(WS2812_BeamEffect_t *eff, float speed, int trail_len,
                           uint8_t sR, uint8_t sG, uint8_t sB, uint8_t eR,
                           uint8_t eG, uint8_t eB, int start_led, int end_led);
int WS2812_BeamEffectRun(WS2812_BeamEffect_t *eff);
int WS2812_BeamEffectRunNoShow(WS2812_BeamEffect_t *eff);
int WS2812_BeamEffectKeepRun(WS2812_BeamEffect_t *eff);

int WS2812LED_BreathGenerate(WS2812_Breath_t *breath);

int StopLEDTask(void);
int LaunchLEDTask(void);
int DaemonTask(void);
int ShootTask(void);

#ifdef __cplusplus
}
#endif

#endif /* WS2812LED_APP_H */
