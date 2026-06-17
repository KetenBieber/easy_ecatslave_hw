# easy_ecatslave_hw

[English](./README.md)

> 简洁实用的 EtherCAT 从站固件 | STM32F4 + LAN9252 | 面向 Robocon 机器人执行器节点

## 项目简介

`easy_ecatslave_hw` 是基于 **STM32F407** + **LAN9252** ESC 芯片的 EtherCAT 从站固件，专为 Robocon 竞赛机器人设计。它提供了一套"开箱即用"的从站方案——连接 CAN 总线电机（DJI 电机 / VESC）后即可作为 EtherCAT 从站接入主站，大幅降低机器人电控系统的布线复杂度与调试成本。

## 硬件架构

```
┌────────────────────────────────────┐
│           主站 (工控机)              │
│         EtherCAT Master             │
└──────────────┬─────────────────────┘
               │ EtherCAT (RJ45)
┌──────────────┴─────────────────────┐
│         LAN9252 ESC 芯片            │
│         (EtherCAT 从站控制器)        │
├────────────────────────────────────┤
│         SPI 通信                    │
├────────────────────────────────────┤
│         STM32F407                   │
│  ├── CAN1 ── 舵向电机 (M2006)       │
│  ├── CAN2 ── 轮向电机 (VESC)        │
│  ├── GPIO ── 3 路 IO               │
│  └── SPI  ── WS2812 LED            │
└────────────────────────────────────┘
```

## 分支说明

| 分支 | 描述 |
|------|------|
| **main** | 主开发分支，集成最新特性 |
| `onlyDJI_f407zgt6` | 仅适配 DJI 电机的简化版本 |
| `swerve_HAL_f407zgt6` | HAL 库版舵轮底盘从站（CAN1 舵向 M2006 / CAN2 轮向 VESC + 3IO） |
| `swerve_LL_f407zgt6` | LL 库版舵轮底盘从站，更低延迟 |
| `sync_ecat_test` | DC 同步时钟测试分支（多从站同步） |

## 关键特性

- **EtherCAT 标准状态机**: 完整实现 INIT → PREOP → SAFEOP → OP 状态转换
- **DC 分布式时钟**: 支持多从站同步，同步精度 < 1μs
- **CAN 总线健壮性**: 大量 CAN 通信异常处理，避免单点故障导致从站离线
- **看门狗保护**: 从站本地看门狗监测主站连接，失联自动回退到 SAFEOP+ERROR
- **原子操作**: 裸机环境下使用 `LDREX`/`STREX` 保护 ISR-主循环临界区
- **DWT 性能测量**: 内置 Cycle Counter 微秒级代码计时
- **WS2812 LED**: SPI 驱动的灯效控制

## CAN 防范措施

针对 CAN 控制器在实际使用中的不稳定性（发送超时、错误帧、总线离线等），项目采取了以下措施：

- CAN 错误自动恢复（自动重初始化）
- 发送超时重试机制
- 错误计数器监控
- 极限吞吐量测试验证

## EtherCAT 调试要点

```c
// 关键状态变量
ESCvar.ALevent   // 主站事件指示
ESCvar.ALerror   // 错误码 — 定位问题的首要入口
ESCvar.ALstatus  // 从站状态 (INIT/BOOT/PREOP/SAFEOP/OP)
```

- **INIT**: 不可 PDO/SDO 通信 | **PREOP**: 仅 SDO | **SAFEOP**: 仅 TxPDO | **OP**: 全双工 PDO
- 看门狗超时会导致从站掉入 SAFEOP+ERROR，注意 `watchdog_cnt` 与 SM2 中断周期的匹配

## 快速开始

```shell
# 配置 & 编译
cmake --preset stm32f4
cmake --build --preset stm32f4

# 烧录
JFlash -openprj project.jflash -open firmware.hex -auto -exit
```

## 相关仓库

- [rc_ecat_controls](https://github.com/KetenBieber/rc_ecat_controls) — 上位机 ROS EtherCAT 主站控制器
- [gdut_rc_embedded_fw](https://github.com/KetenBieber/gdut_rc_embedded_fw) — 机器人主控固件（原 General_Framework）
- [rc25_path](https://github.com/KetenBieber/rc25_path) — 路径规划与仿真
