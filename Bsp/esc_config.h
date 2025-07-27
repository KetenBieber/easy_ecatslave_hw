/**
 * @file esc_config.h
 * @author Keten (2863861004@qq.com)
 * @brief esc 引脚宏定义替换，做一个hal层
 *        其实工作就是：把一些 ESC
 *        所需的引脚号、端口、中断等用一层宏封装起来，将SOES库封装为不用修改的状态
 *        移植库的人只需要在这里做config的修改，将对应的hal引脚对应赋值
 *
 * @version 0.1
 * @date 2025-04-09
 *
 * @copyright Copyright (c) 2025
 *
 * @attention :
 * @note :
 * @versioninfo :
 */
#ifndef ESC_CONFIG_H
#define ESC_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "can.h"
#include "spi.h"
#include "tim.h"

/*-----------------------------------macro------------------------------------*/
/* IRQ relative */
#define ENABLE_ESC_PDI_INT HAL_NVIC_EnableIRQ(PDI_IRQ_EXTI_IRQn);
#define DISABLE_ESC_PDI_INT HAL_NVIC_DisableIRQ(PDI_IRQ_EXTI_IRQn);

#define ENABLE_ESC_SYNC0_INT HAL_NVIC_EnableIRQ(SYNC0_IRQ_EXTI_IRQn);
#define DISABLE_ESC_SYNC0_INT HAL_NVIC_DisableIRQ(SYNC0_IRQ_EXTI_IRQn);

#define ENABLE_ESC_SYNC1_INT HAL_NVIC_EnableIRQ(SYNC1_IRQ_EXTI_IRQn);
#define DISABLE_ESC_SYNC1_INT HAL_NVIC_DisableIRQ(SYNC1_IRQ_EXTI_IRQn);

#define ENABLE_ESC_LOCAL_TIM_INT HAL_NVIC_EnableIRQ(LOCAL_TIM_IRQ);
#define DISABLE_ESC_LOCAL_TIM_INT HAL_NVIC_DisableIRQ(LOCAL_TIM_IRQ);

/* ESC relative */
#define ESC_RESET_Pin GPIO_PIN_8
#define ESC_RESET_GPIO_Port GPIOF
#define ESC_RESET_GPIO_CLK_ENABLE __HAL_RCC_GPIOF_CLK_ENABLE()

#define PDI_IRQ_Pin GPIO_PIN_0
#define PDI_IRQ_GPIO_Port GPIOC
#define PDI_IRQ_EXTI_IRQn EXTI0_IRQn
#define PDI_IRQ_GPIO_CLK_ENABLE __HAL_RCC_GPIOC_CLK_ENABLE()

#define SYNC1_IRQ_Pin GPIO_PIN_1
#define SYNC1_IRQ_GPIO_Port GPIOC
#define SYNC1_IRQ_EXTI_IRQn EXTI1_IRQn
#define SYNC1_IRQ_GPIO_CLK_ENABLE __HAL_RCC_GPIOC_CLK_ENABLE()

#define SYNC0_IRQ_Pin GPIO_PIN_3
#define SYNC0_IRQ_GPIO_Port GPIOC
#define SYNC0_IRQ_EXTI_IRQn EXTI3_IRQn
#define SYNC0_IRQ_GPIO_CLK_ENABLE __HAL_RCC_GPIOC_CLK_ENABLE()

/* spi relative */
#define FLASH_SPI_SCK_Pin GPIO_PIN_5
#define FLASH_SPI_SCK_GPIO_Port GPIOA
#define FLASH_SPI_MISO_Pin GPIO_PIN_6
#define FLASH_SPI_MISO_GPIO_Port GPIOA
#define FLASH_SPI_MOSI_Pin GPIO_PIN_7
#define FLASH_SPI_MOSI_GPIO_Port GPIOA
#define FLASH_SPI_CS_Pin GPIO_PIN_8
#define FLASH_SPI_CS_GPIO_Port GPIOA
#define ESC_SPI_Port GPIOA
#define ESC_SPI hspi1  // 所使用的spi 句柄
#define ESC_SPI_Instance SPI1
#define ESC_SPI_GPIO_CLK_ENABLE __HAL_RCC_GPIOA_CLK_ENABLE()
#define ESC_SPI_CLK_ENABLE __HAL_RCC_SPI1_CLK_ENABLE()

/* local tim relative */
#define LOCAL_TIM TIM2
#define LOCAL_TIM_RCC_CLK_ENABLE __HAL_RCC_TIM2_CLK_ENABLE()
#define LOCAL_TIM_RCC_CLK_DISABLE __HAL_RCC_TIM2_CLK_DISABLE()
#define ESC_LOCAL_TIM htim2
#define LOCAL_TIM_IRQ TIM2_IRQn

/* can0 send relative */
#define CAN0_TASK_TIM TIM3
#define CAN0_TASK_TIM_RCC_CLK_ENABLE __HAL_RCC_TIM3_CLK_ENABLE()
#define CAN0_TASK_TIM_RCC_CLK_DISABLE __HAL_RCC_TIM3_CLK_DISABLE()
#define CAN0_TASK_TIM_INSTANCE htim3
#define CAN0_TASK_TIM_IRQ TIM3_IRQn

/* can1 send relative */
#define CAN1_TASK_TIM TIM4
#define CAN1_TASK_TIM_RCC_CLK_ENABLE __HAL_RCC_TIM4_CLK_ENABLE()
#define CAN1_TASK_TIM_RCC_CLK_DISABLE __HAL_RCC_TIM4_CLK_DISABLE()
#define CAN1_TASK_TIM_INSTANCE htim4
#define CAN1_TASK_TIM_IRQ TIM4_IRQn

/* can0 & can1 relative */
/* In order to be compatible with the upper computer, we use can0 to represent
 * can1 on the STM32 peripheral, and can1 to represent can2 on the STM32
 * peripheral */
#define CAN0_Handle hcan1
#define CAN0_Instance CAN1
#define CAN0_Tx_Pin GPIO_PIN_11
#define CAN0_Rx_Pin GPIO_PIN_12
#define CAN0_IO_PORT GPIOA
#define CAN0_NVIC_RX0_IRQ CAN1_RX0_IRQn
#define CAN0_NVIC_RX1_IRQ CAN1_RX1_IRQn
#define CAN0_NVIC_TX_IRQ CAN1_TX_IRQn
#define CAN0_NVIC_SCE_IRQ CAN1_SCE_IRQn
#define CAN0_RCC_CLK_ENABLE __HAL_RCC_CAN1_CLK_ENABLE()
#define CAN0_RCC_CLK_DISABLE __HAL_RCC_CAN1_CLK_DISABLE()
#define CAN0_IO_RCC_CLK_ENABLE __HAL_RCC_GPIOA_CLK_ENABLE()
#ifndef ONLY_CAN0
#define CAN1_Handle hcan2
#define CAN1_Instance CAN2
#define CAN1_Tx_Pin GPIO_PIN_12
#define CAN1_Rx_Pin GPIO_PIN_13
#define CAN1_IO_PORT GPIOB
#define CAN1_NVIC_RX0_IRQ CAN2_RX0_IRQn
#define CAN1_NVIC_RX1_IRQ CAN2_RX1_IRQn
#define CAN1_NVIC_TX_IRQ CAN2_TX_IRQn
#define CAN1_NVIC_SCE_IRQ CAN2_SCE_IRQn
#define CAN1_RCC_CLK_ENABLE __HAL_RCC_CAN2_CLK_ENABLE()
#define CAN1_RCC_CLK_DISABLE __HAL_RCC_CAN2_CLK_DISABLE()
#define CAN1_IO_RCC_CLK_ENABLE __HAL_RCC_GPIOB_CLK_ENABLE()

#endif

/* led relative - low pin to light up */
#define LED1_Pin GPIO_PIN_13
#define LED1_GPIO_Port GPIOC
#define LED2_Pin GPIO_PIN_14
#define LED2_GPIO_Port GPIOC
#define LED3_Pin GPIO_PIN_15
#define LED3_GPIO_Port GPIOC

#define LED_GPIO_CLK_ENABLE __HAL_RCC_GPIOC_CLK_ENABLE()

/* key relative */
#define KEY0_Pin GPIO_PIN_4
#define KEY0_GPIO_Port GPIOE
#define KEY1_Pin GPIO_PIN_3
#define KEY1_GPIO_Port GPIOE
#define KEY2_Pin GPIO_PIN_2
#define KEY2_GPIO_Port GPIOE
#define KEY_GPIO_Port GPIOE
#define KEY_GPIO_CLK_ENABLE __HAL_RCC_GPIOE_CLK_ENABLE()
#define KEY_GPIO_CLK_DISABLE __HAL_RCC_GPIOE_CLK_DISABLE()

/* beep relative */
// #define BEEP_Pin GPIO_PIN_9
// #define BEEP_GPIO_Port GPIOA
// #define BEEP_GPIO_CLK_ENABLE __HAL_RCC_GPIOA_CLK_ENABLE()
// #define BEEP_GPIO_CLK_DISABLE __HAL_RCC_GPIOA_CLK_DISABLE()

/* photogate relative */
#define PHOTOGATE1_Pin GPIO_PIN_0
#define PHOTOGATE1_GPIO_Port GPIOG
#define PHOTOGATE_1_GPIO_CLK_ENABLE __HAL_RCC_GPIOG_CLK_ENABLE()
#define PHOTOGATE_1_GPIO_CLK_DISABLE __HAL_RCC_GPIOG_CLK_DISABLE()

#define PHOTOGATE2_Pin GPIO_PIN_1
#define PHOTOGATE2_GPIO_Port GPIOG
#define PHOTOGATE_2_GPIO_CLK_ENABLE __HAL_RCC_GPIOG_CLK_ENABLE()
#define PHOTOGATE_2_GPIO_CLK_DISABLE __HAL_RCC_GPIOG_CLK_DISABLE()

#define PHOTOGATE3_Pin GPIO_PIN_2
#define PHOTOGATE3_GPIO_Port GPIOG
#define PHOTOGATE_3_GPIO_CLK_ENABLE __HAL_RCC_GPIOG_CLK_ENABLE()
#define PHOTOGATE_3_GPIO_CLK_DISABLE __HAL_RCC_GPIOG_CLK_DISABLE()

// 参考0电平
#define PHOTOGATE_REFERENCE_Pin GPIO_PIN_4
#define PHOTOGATE_REFERENCE_GPIO_Port GPIOG
#define PHOTOGATE_REFERENCE_GPIO_CLK_ENABLE __HAL_RCC_GPIOG_CLK_ENABLE()
#define PHOTOGATE_REFERENCE_GPIO_CLK_DISABLE __HAL_RCC_GPIOG_CLK_DISABLE()

#ifdef USE_LL_DRIVER

#define LOCAL_TIM_LL LL_APB1_GRP1_PERIPH_TIM2
#define CAN0_TASK_TIM_LL LL_APB1_GRP1_PERIPH_TIM3
#define CAN1_TASK_TIM_LL LL_APB1_GRP1_PERIPH_TIM4

// 使用LL库将不再使用HAL库的中断处理函数
#define LL_Local_TimerISR TIM2_IRQHandler
#define LL_CAN0_Task_TimerISR TIM3_IRQHandler
#define LL_CAN1_Task_TimerISR TIM4_IRQHandler

#endif

// 编码器相关
#define ENCODER_UART huart6  // 使用的串口句柄
#define ENCODER_UART_INSTANCE USART6
#define ENCODER_UART_Baudrate 115200  // 编码器串口波特率
#define ENCODER_UART_Tx_Pin GPIO_PIN_14
#define ENCODER_UART_Rx_Pin GPIO_PIN_9
#define ENCODER_UART_GPIO_Port GPIOG
#define ENCODER_UART_GPIO_CLK_ENABLE __HAL_RCC_GPIOG_CLK_ENABLE()
#define ENCODER_UART_GPIO_CLK_DISABLE __HAL_RCC_GPIOG_CLK_DISABLE()
#define ENCODER_UART_RCC_CLK_ENABLE __HAL_RCC_USART6_CLK_ENABLE()
#define ENCODER_UART_RCC_CLK_DISABLE __HAL_RCC_USART6_CLK_DISABLE()
#define ENCODER_UART_NVIC_IRQ USART6_IRQn     // 使用的中断号
#define ENCODER_DMA_Tx_Handle hdma_usart6_tx  // 使用的DMA句柄
#define ENCODER_DMA_Rx_Handle hdma_usart6_rx  // 使用的DMA句柄
#define ENCODER_DMA_RX_Instance DMA2_Stream1  // 使用的DMA实例
#define ENCODER_DMA_TX_Instance DMA2_Stream6  // 使用的DMA实例
#define ENCODER_DMA_Channel DMA_CHANNEL_5     // 使用的DMA通道
#define ENCODER_DMA_RCC_CLK_ENABLE __HAL_RCC_DMA2_CLK_ENABLE()
#define ENCODER_DMA_RCC_CLK_DISABLE __HAL_RCC_DMA2_CLK_DISABLE()
#define ENCODER_DMA_Tx_STREAM_IRQn DMA2_Stream6_IRQn
#define ENCODER_DMA_Rx_STREAM_IRQn DMA2_Stream1_IRQn
#define ENCODER_RS485_DE_Pin GPIO_PIN_13  // RS485的DE引脚
#define ENCODER_RS485_DE_GPIO_Port GPIOG
#define ENCODER_RS485_DE_GPIO_CLK_ENABLE __HAL_RCC_GPIOG_CLK_ENABLE()
#define ENCODER_RS485_DE_GPIO_CLK_DISABLE __HAL_RCC_GPIOG_CLK_DISABLE()

/* WS2812灯条相关 */
#define WS2812_GPIO_Port GPIOC
#define WS2812_GPIO_Pin GPIO_PIN_7
#define WS2812_GPIO_RCC_CLK_ENABLE __HAL_RCC_GPIOC_CLK_ENABLE()
#define WS2812_GPIO_RCC_CLK_DISABLE __HAL_RCC_GPIOC_CLK_DISABLE()
#define WS2812_TIM htim8
#define WS2812_TIM_INSTANCE TIM8
#define WS2812_TIM_Channel TIM_CHANNEL_2  // 使用的通道
#define WS2812_TIM_RCC_CLK_ENABLE __HAL_RCC_TIM8_CLK_ENABLE();
#define WS2812_TIM_RCC_CLK_DISABLE __HAL_RCC_TIM8_CLK_DISABLE();
#define WS2812_TIM_PWM_DMA hdma_tim8_ch2
#define WS2812_TIM_PWM_DMA_RCC_CLK_ENABLE __HAL_RCC_DMA2_CLK_ENABLE()
#define WS2812_TIM_PWM_DMA_RCC_CLK_DISABLE __HAL_RCC_DMA2_CLK_DISABLE()
#define WS2812_TIM_PWM_DMA_Instance DMA2_Stream3
#define WS2812_TIM_PWM_DMA_Channel DMA_CHANNEL_7
#define WS2812_TIM_DMA_STREAM_IRQn DMA2_Stream3_IRQn
#define WS2812_TIM_DMA_ID TIM_DMA_ID_CC2

/* 发射电磁阀io */
#define SHOOT_GPIO_Port GPIOA
#define SHOOT_GPIO_Pin GPIO_PIN_3
#define SHOOT_GPIO_RCC_CLK_ENABLE __HAL_RCC_GPIOA_CLK_ENABLE()
#define SHOOT_GPIO_RCC_CLK_DISABLE __HAL_RCC_GPIOA_CLK_DISABLE()
#define SHOOT_INIT_STATE GPIO_PIN_SET  // 默认拉高

/*----------------------------------variable----------------------------------*/

/* spi relative:什么信号代表片选 */
#define SCS_LOW 0
#define SCS_HIGH 1

/*----------------------------------config----------------------------------*/

#if defined(LAN9252)
#define SCS_ACTIVE_POLARITY SCS_LOW  // 拉低为片选

#else
#error "No supported ESC specified"
#endif

#ifdef __cplusplus
}
#endif

#endif /* ESC_CONFIG_H */
