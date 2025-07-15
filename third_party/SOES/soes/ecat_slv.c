/*
 * Licensed under the GNU General Public License version 2 with exceptions. See
 * LICENSE file in the project root for full license information
 */
#include "ecat_slv.h"

#include <stddef.h>

#include "esc.h"
#include "esc_coe.h"
#include "esc_eoe.h"
#include "esc_foe.h"

#define IS_RXPDO(index) ((index) >= 0x1600 && (index) < 0x1800)
#define IS_TXPDO(index) ((index) >= 0x1A00 && (index) < 0x1C00)

/* Global variables used by the stack */
uint8_t MBX[MBXBUFFERS * MAX(MBXSIZE, MBXSIZEBOOT)];
_MBXcontrol MBXcontrol[MBXBUFFERS];
_SMmap SMmap2[MAX_MAPPINGS_SM2];
_SMmap SMmap3[MAX_MAPPINGS_SM3];
_ESCvar ESCvar;

/* Private variables */
static volatile int watchdog;

#if MAX_MAPPINGS_SM2 > 0
static uint8_t rxpdo[MAX_RXPDO_SIZE] __attribute__((aligned(8)));
#else
extern uint8_t *rxpdo;
#endif

#if MAX_MAPPINGS_SM3 > 0
static uint8_t txpdo[MAX_TXPDO_SIZE] __attribute__((aligned(8)));
#else
extern uint8_t *txpdo;
#endif

/** Function to pre-qualify the incoming SDO download.
 *
 * @param[in] index      = index of SDO download request to check
 * @param[in] sub-index  = sub-index of SDO download request to check
 * @return SDO abort code, or 0 on success
 */
uint32_t ESC_download_pre_objecthandler(uint16_t index, uint8_t subindex,
                                        void *data, size_t size,
                                        uint16_t flags) {
  if (IS_RXPDO(index) || IS_TXPDO(index) || index == RX_PDO_OBJIDX ||
      index == TX_PDO_OBJIDX) {
    uint8_t minSub = ((flags & COMPLETE_ACCESS_FLAG) == 0) ? 0 : 1;
    if (subindex > minSub && COE_maxSub(index) != 0) {
      return ABORT_SUBINDEX0_NOT_ZERO;
    }
  }

  if (ESCvar.pre_object_download_hook) {
    return (ESCvar.pre_object_download_hook)(index, subindex, data, size,
                                             flags);
  }

  return 0;
}

/** Hook called from the slave stack SDO Download handler to act on
 * user specified Index and Sub-index.
 *
 * @param[in] index      = index of SDO download request to handle
 * @param[in] sub-index  = sub-index of SDO download request to handle
 * @return SDO abort code, or 0 on success
 */
uint32_t ESC_download_post_objecthandler(uint16_t index, uint8_t subindex,
                                         uint16_t flags) {
  if (ESCvar.post_object_download_hook != NULL) {
    return (ESCvar.post_object_download_hook)(index, subindex, flags);
  }

  return 0;
}

/** Function to pre-qualify the incoming SDO upload.
 *
 * @param[in] index      = index of SDO upload request to handle
 * @param[in] sub-index  = sub-index of SDO upload request to handle
 * @return SDO abort code, or 0 on success
 */
uint32_t ESC_upload_pre_objecthandler(uint16_t index, uint8_t subindex,
                                      void *data, size_t size, uint16_t flags) {
  if (ESCvar.pre_object_upload_hook != NULL) {
    return (ESCvar.pre_object_upload_hook)(index, subindex, data, size, flags);
  }

  return 0;
}

/** Hook called from the slave stack SDO Upload handler to act on
 * user specified Index and Sub-index.
 *
 * @param[in] index      = index of SDO upload request to handle
 * @param[in] sub-index  = sub-index of SDO upload request to handle
 * @return SDO abort code, or 0 on success
 */
uint32_t ESC_upload_post_objecthandler(uint16_t index, uint8_t subindex,
                                       uint16_t flags) {
  if (ESCvar.post_object_upload_hook != NULL) {
    return (ESCvar.post_object_upload_hook)(index, subindex, flags);
  }

  return 0;
}

/** Hook called from the slave stack ESC_stopoutputs to act on state changes
 * forcing us to stop outputs. Here we can set them to a safe state.
 */
void APP_safeoutput(void) {
  DPRINT("APP_safeoutput\n");

  if (ESCvar.safeoutput_override != NULL) {
    (ESCvar.safeoutput_override)();
  }
}

/** Write local process data to Sync Manager 3, Master Inputs.
 */
void TXPDO_update(void) {
  if (ESCvar.txpdo_override != NULL) {
    (ESCvar.txpdo_override)();
  } else {
    if (MAX_MAPPINGS_SM3 > 0) {
      COE_pdoPack(txpdo, ESCvar.sm3mappings, SMmap3);
    }
    ESC_write(ESC_SM3_sma, txpdo, ESCvar.ESC_SM3_sml);
  }
}

/** Read Sync Manager 2 to local process data, Master Outputs.
 */
void RXPDO_update(void) {
  if (ESCvar.rxpdo_override != NULL) {
    (ESCvar.rxpdo_override)();
  } else {
    ESC_read(ESC_SM2_sma, rxpdo, ESCvar.ESC_SM2_sml);
    if (MAX_MAPPINGS_SM2 > 0) {
      COE_pdoUnpack(rxpdo, ESCvar.sm2mappings, SMmap2);
    }
  }
}

/* Set the watchdog count value, don't have any affect when using
 * HW watchdog 0x4xx
 *
 * @param[in] watchdogcnt  = new watchdog count value
 */
void APP_setwatchdog(int watchdogcnt) {
  CC_ATOMIC_SET(ESCvar.watchdogcnt, watchdogcnt);
}

/* Function to update local I/O, call read ethercat outputs, call
 * write ethercat inputs. Implement watch-dog counter to count-out if we have
 * made state change affecting the App.state.
 *
 * 看门狗计数是由最大值递减，所以超时即看门狗计数<=0
 */
void DIG_process(uint8_t flags) {
  /* Handle watchdog 如果传入的是看门狗标志位 */
  if ((flags & DIG_PROCESS_WD_FLAG) > 0) {
    if (CC_ATOMIC_GET(watchdog) > 0) {  // 检查看门狗是否超时
      CC_ATOMIC_SUB(watchdog, 1);       // 看门狗计数-1
    }

    if ((CC_ATOMIC_GET(watchdog) <= 0) &&  // 如果超时
        ((CC_ATOMIC_GET(ESCvar.App.state) & APPSTATE_OUTPUT) > 0)) {
      DPRINT("DIG_process watchdog expired\n");
      ESC_ALstatusgotoerror(
          (ESCsafeop | ESCerror),
          ALERR_WATCHDOG);  // 将从站状态切换到安全状态，并上报看门狗错误
    } else if (((CC_ATOMIC_GET(ESCvar.App.state) & APPSTATE_OUTPUT) == 0)) {
      CC_ATOMIC_SET(watchdog, ESCvar.watchdogcnt);
    }
  }

  /* Handle Outputs */
  if ((flags & DIG_PROCESS_OUTPUTS_FLAG) > 0) {
    if (((CC_ATOMIC_GET(ESCvar.App.state) & APPSTATE_OUTPUT) > 0) &&
        (ESCvar.ALevent & ESCREG_ALEVENT_SM2)) {
      RXPDO_update();
      CC_ATOMIC_SET(watchdog, ESCvar.watchdogcnt);  // 在这里重装载看门狗
      /* Set outputs */
      cb_set_outputs();
    } else if (ESCvar.ALevent & ESCREG_ALEVENT_SM2) {
      RXPDO_update();
    }
  }

  /* Call application */
  if ((flags & DIG_PROCESS_APP_HOOK_FLAG) > 0) {
    /* Call application callback if set */
    if (ESCvar.application_hook != NULL) {
      (ESCvar.application_hook)();
    }
  }

  /* Handle Inputs */
  if ((flags & DIG_PROCESS_INPUTS_FLAG) > 0) {
    if (CC_ATOMIC_GET(ESCvar.App.state) > 0) {
      /* Update inputs */
      cb_get_inputs();
      TXPDO_update();
    }
  }
}

/*
 * Handler for SM change, SM0/1, AL CONTROL and EEPROM events, the application
 * control what interrupts that should be served and re-activated with
 * event mask argument
 * 处理与EtherCAT从站控制器（ESC）相关的非同步事件，这些事件包括同步管理器（SM）状态变化、邮箱（Mailbox）处理、EEPROM操作等
 *
 * 在中断模式下，当PDI中断触发时，可以调用ecat_slv_worker处理非同步事件
 */
void ecat_slv_worker(uint32_t event_mask) {
  do {
    /* Check the state machine检查状态机 */
    ESC_state();
    /* Check the SM activation event检查同步管理器（SM）的激活事件 */
    ESC_sm_act_event();

    /* Check mailboxes 处理邮箱事件，返回直接的就是邮箱的数据个数 */
    while ((ESC_mbxprocess() > 0) || (ESCvar.txcue > 0)) {
      /* 有数据，那么就来到这里调用对应的协议处理函数 */
      ESC_coeprocess();
#if USE_FOE
      ESC_foeprocess();
#endif
#if USE_EOE
      ESC_eoeprocess();
#endif
      ESC_xoeprocess();
    }
#if USE_EOE
    ESC_eoeprocess_tx();
#endif
    /* Call emulated eeprom handler if set */
    if (ESCvar.esc_hw_eep_handler != NULL) {
      (ESCvar.esc_hw_eep_handler)();
    }

    /* 更新AL Event状态 */
    CC_ATOMIC_SET(ESCvar.ALevent, ESC_ALeventread());

  } while (ESCvar.ALevent &
           event_mask);  // 如果AL Event中有需要处理的事件，那么就继续处理

  /* 重新激活事件掩码 */
  ESC_ALeventmaskwrite(ESC_ALeventmaskread() | event_mask);
}

/*
 * Polling function. It should be called periodically for an application
 * when only SM2/DC interrupt is active.
 * Read and handle events for the EtherCAT state, status, mailbox and eeprom.
 */
void ecat_slv_poll(void) {
  /* Read local time from ESC*/
  ESC_read(ESCREG_LOCALTIME, (void *)&ESCvar.Time, sizeof(ESCvar.Time));
  ESCvar.Time = etohl(ESCvar.Time);

  /* Check the state machine */
  ESC_state();
  /* Check the SM activation event 检查同步管理器事件（SM激活事件） */
  ESC_sm_act_event();

  /* 前面的操作就是为了执行下面这一步时，是最新的状态 */
  /* Check mailboxes */
  if (ESC_mbxprocess()) {
    ESC_coeprocess();
#if USE_FOE
    ESC_foeprocess();
#endif
#if USE_EOE
    ESC_eoeprocess();
#endif
    ESC_xoeprocess();
  }
#if USE_EOE
  ESC_eoeprocess_tx();
#endif

  /* Call emulated eeprom handler if set */
  if (ESCvar.esc_hw_eep_handler != NULL) {
    (ESCvar.esc_hw_eep_handler)();
  }
}

/*
 * Poll all events in a free-run application
 */
void ecat_slv(void) {
  ecat_slv_poll();
  // 可以看到下面处理了输入输出以及看门狗，这就是所说的不依赖外部中断的方式（FreeRun模式）
  DIG_process(DIG_PROCESS_WD_FLAG | DIG_PROCESS_OUTPUTS_FLAG |
              DIG_PROCESS_APP_HOOK_FLAG | DIG_PROCESS_INPUTS_FLAG);
}

/*
 * Initialize the slave stack.
 */
void ecat_slv_init(esc_cfg_t *config) {
  DPRINT("Slave stack init started\n");

  /* Init watchdog */
  watchdog = config->watchdog_cnt;

  /* Call stack configuration */
  ESC_config(config);
  /* Call HW init */
  ESC_init(config);

  /*  wait until ESC is started up */
  while ((ESCvar.DLstatus & 0x0001) == 0) {
    ESC_read(ESCREG_DLSTATUS, (void *)&ESCvar.DLstatus,
             sizeof(ESCvar.DLstatus));
    ESCvar.DLstatus = etohs(ESCvar.DLstatus);
  }

#if USE_FOE
  /* Init FoE */
  FOE_init();
#endif

#if USE_EOE
  /* Init EoE */
  EOE_init();
#endif

  /* reset ESC to init state */
  ESC_ALstatus(ESCinit);
  ESC_ALerror(ALERR_NONE);
  ESC_stopmbx();
  ESC_stopinput();
  ESC_stopoutput();
  /* Init Object Dictionary default values */
  COE_initDefaultValues();
}
