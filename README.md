# easy_ecatslave_hw

> Clean & practical EtherCAT slave firmware | STM32F4 + LAN9252 | Robocon actuator nodes

[中文文档](./README_CN.md)

## Overview

`easy_ecatslave_hw` is an EtherCAT slave firmware based on **STM32F407** + **LAN9252** ESC chip, purpose-built for Robocon competition robots. It provides a ready-to-use slave solution — connect CAN bus motors (DJI / VESC) and the node immediately joins the EtherCAT bus, dramatically reducing wiring complexity and debugging effort.

## Hardware Architecture

```
┌────────────────────────────────────┐
│          Host (Industrial PC)       │
│         EtherCAT Master             │
└──────────────┬─────────────────────┘
               │ EtherCAT (RJ45)
┌──────────────┴─────────────────────┐
│         LAN9252 ESC Chip            │
│         (EtherCAT Slave Controller) │
├────────────────────────────────────┤
│         SPI Bus                     │
├────────────────────────────────────┤
│         STM32F407                   │
│  ├── CAN1 ── Steering motor (M2006)│
│  ├── CAN2 ── Drive motor (VESC)    │
│  ├── GPIO ── 3-ch digital IO       │
│  └── SPI  ── WS2812 LED            │
└────────────────────────────────────┘
```

## Branches

| Branch | Description |
|--------|-------------|
| **main** | Main development, latest features |
| `onlyDJI_f407zgt6` | DJI-motor-only simplified variant |
| `swerve_HAL_f407zgt6` | HAL-based swerve chassis slave (CAN1 steering M2006 / CAN2 drive VESC + 3IO) |
| `swerve_LL_f407zgt6` | LL library swerve slave — lower latency |
| `sync_ecat_test` | DC sync clock test (multi-slave synchronization) |

## Key Features

- **Full EtherCAT State Machine**: INIT → PREOP → SAFEOP → OP transitions
- **DC Distributed Clock**: Multi-slave sync with < 1 μs precision
- **CAN Robustness**: Extensive error handling — bus-off recovery, timeout retry, error counter monitoring
- **Watchdog Protection**: Local slave watchdog monitors master connection; auto fallback to SAFEOP+ERROR
- **Atomic Operations**: `LDREX`/`STREX` critical sections for bare-metal ISR safety
- **DWT Performance Timing**: Cycle Counter for microsecond-level code profiling
- **WS2812 LED**: SPI-driven RGB LED effects

## Debugging EtherCAT

```c
// Key status registers
ESCvar.ALevent   // Master event indicator
ESCvar.ALerror   // Error code — primary debugging entry point
ESCvar.ALstatus  // Slave state (INIT/BOOT/PREOP/SAFEOP/OP)
```

- **INIT**: No PDO/SDO | **PREOP**: SDO only | **SAFEOP**: TxPDO only | **OP**: Full PDO
- Watchdog timeout forces slave into SAFEOP+ERROR — ensure `watchdog_cnt` matches SM2 interrupt period

## Quick Start

```shell
# Configure & build
cmake --preset stm32f4
cmake --build --preset stm32f4

# Flash
JFlash -openprj project.jflash -open firmware.hex -auto -exit
```

## Related Repos

- [rc_ecat_controls](https://github.com/KetenBieber/rc_ecat_controls) — ROS EtherCAT master controllers
- [gdut_rc_embedded_fw](https://github.com/KetenBieber/gdut_rc_embedded_fw) — Main control firmware (formerly General_Framework)
- [rc25_path](https://github.com/KetenBieber/rc25_path) — Path planning & simulation
