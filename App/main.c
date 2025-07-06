// clang-format off
/**
 * @file main.c
 * @author Keten (2863861004@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-04-13
 *
 * @copyright Copyright (c) 2025
 *
 * @attention :
 * @note :在这里附上soes官网的开发指南：
 *        1.FreeRun模式：
 *          PDI interrupt is not enabled
 *          SyncX interrupt are not enabled
 *          AL Event are polled for events every ESC Read/Write to be handled by
 *          the stack ecat_slv() get called regular to handle stack operations
 *        2.MixedMode模式：
 *          PDI interrupt is enabled
 *          Only SM2 should be masked to generate PDI interrupt.
 *          Sync0 interrupt is enabled if DC Synchronous
 *          AL Event are polled for events every ESC Read/Write to be handled by the stack
 *          ecat_slv_poll() get called regular to handle stack operations
 *          DIG_process(DIG_PROCESS_WD_FLAG) get called regular to kick the software watchdog counter Or the hardware PDI interrupt watchdog should be activated
 *        3.InterruptMode模式：
 *          PDI interrupt is enabled
 *          In addition to SM2 add SMCHANGE, EEP(if EEPROM emulated), ALCONTROL, SM0 and SM1 to the mask to generate PDI interrupt.
 *          Sync0 interrupt is enabled if DC Synchronous
 *          AL Event are not polled for events
 *          ecat_slv_worker() get called when an interrupt occur, preferably from a background task
 *          DIG_process(DIG_PROCESS_WD_FLAG) get called regular to kick the software watchdog counter Or the hardware PDI interrupt watchdog should be activated 
 * @versioninfo :
 */
// clang-format on
#include "main.h"

#include "bsp_init.h"
#include "ecat_app.h"
#include "esc_config.h"

void SystemClock_Config(void);
void MX_FREERTOS_Init(void);

#ifdef DEBUG

float main_current_time = 0, main_last_time = 0;  // 用于记录主循环的时间
float main_dt = 0;

#endif
int main(void) {
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* BSP 层配置 */
  BSP_Init();
  // PCout(13);HAL_GPIO_WritePin()

#if (defined(MixedMode) + defined(InterruptMode) + defined(FreeRun)) != 1
#error "只允许定义MixedMode、InterruptMode或FreeRun中的一个。"
  while (1) {
    // 进入错误循环
    // 后续可以加入指示错误
  }
#endif

  Ecatapp_Init();

  ENABLE_ESC_LOCAL_TIM_INT;
  StartTimer();
  /* we don't use any init function that cubemx generate for us */
  /* we don't use freertos right now! */
  /* Infinite loop */

  uint32_t heartbeat = 0;
  while (1) {
#ifdef DEBUG
    main_current_time = DWT_GetTimeline_ms();
    main_dt = main_current_time - main_last_time;
#endif
    // 主循环只需这一个
    Ecatapp_Loop();
    // 心跳检测
    toggle_flash(&heartbeat, 15, 20000, 1000, 4);

    main_last_time = main_current_time;  // 更新主循环时间戳
  }
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
   */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
    Error_Handler();
  }
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1) {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line) {
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line
     number, ex: printf("Wrong parameters value: file %s on line %d\r\n", file,
     line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
