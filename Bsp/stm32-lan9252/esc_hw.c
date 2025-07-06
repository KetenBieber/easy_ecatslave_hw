/**
 * @file esc_hw.c
 * @author Keten (2863861004@qq.com) ---Adapted and modified from
 *         https://github.com/kubabuda/ecat_servo
 * @brief
 * @version 0.1
 * @date 2025-04-11
 *
 * @copyright Copyright (c) 2025
 *
 * @attention :
 * @note :
 * @versioninfo :
 */
#include "esc_hw.h"
/*
 * Licensed under the GNU General Public License version 2 with exceptions. See
 * LICENSE file in the project root for full license information
 */

static int lan9252 = -1;

/* lan9252 singel write */
static void lan9252_write_32(uint16_t address, uint32_t val) {
  uint8_t data[7];

  data[0] = ESC_CMD_SERIAL_WRITE;
  data[1] = ((address >> 8) & 0xFF);
  data[2] = (address & 0xFF);
  data[3] = (val & 0xFF);
  data[4] = ((val >> 8) & 0xFF);
  data[5] = ((val >> 16) & 0xFF);
  data[6] = ((val >> 24) & 0xFF);

  /* Select device. */
  spi_select(lan9252);
  /* Write data */
  write(lan9252, data, sizeof(data));
  /* Un-select device. */
  spi_unselect(lan9252);
}

/* lan9252 single read */
static uint32_t lan9252_read_32(uint32_t address) {
  uint8_t data[4];    // 4字节 --- 32位
  uint8_t result[4];  // 4字节 --- 32位

  data[0] = ESC_CMD_FAST_READ;        // 命令头
  data[1] = ((address >> 8) & 0xFF);  // 存地址高8位
  data[2] = (address & 0xFF);         // 存地址低8位
  data[3] = ESC_CMD_FAST_READ_DUMMY;  // 装载DUMMY，作为数据尾

  /* Select device. */
  spi_select(lan9252);
  /* Read data */
  write(lan9252, data, sizeof(data));
  read(lan9252, result, sizeof(result));
  /* Un-select device. */
  spi_unselect(lan9252);

  return ((result[3] << 24) | (result[2] << 16) | (result[1] << 8) | result[0]);
}

/* ESC read CSR function */
static void ESC_read_csr(uint16_t address, void *buf, uint16_t len) {
  uint32_t value;

  value = (ESC_CSR_CMD_READ | ESC_CSR_CMD_SIZE(len) | address);  //
  lan9252_write_32(ESC_CSR_CMD_REG, value);

  do {
    value = lan9252_read_32(ESC_CSR_CMD_REG);
  } while (value & ESC_CSR_CMD_BUSY);

  value = lan9252_read_32(ESC_CSR_DATA_REG);
  memcpy(buf, (uint8_t *)&value, len);
}

/* ESC write CSR function */
static void ESC_write_csr(uint16_t address, void *buf, uint16_t len) {
  uint32_t value;  // 32位数据

  memcpy((uint8_t *)&value, buf, len);  //
  lan9252_write_32(ESC_CSR_DATA_REG, value);
  value = (ESC_CSR_CMD_WRITE | ESC_CSR_CMD_SIZE(len) | address);
  lan9252_write_32(ESC_CSR_CMD_REG, value);

  do {
    value = lan9252_read_32(ESC_CSR_CMD_REG);
  } while (value & ESC_CSR_CMD_BUSY);
}

/* ESC read process data ram function */
/*static*/ void ESC_read_pram(uint16_t address, void *buf, uint16_t len) {
  uint32_t value;
  uint8_t *temp_buf = buf;
  uint16_t byte_offset = 0;
  uint8_t fifo_cnt, first_byte_position, temp_len, data[4];

  value = ESC_PRAM_CMD_ABORT;
  lan9252_write_32(ESC_PRAM_RD_CMD_REG, value);

  do {
    value = lan9252_read_32(ESC_PRAM_RD_CMD_REG);
  } while (value & ESC_PRAM_CMD_BUSY);

  value = ESC_PRAM_SIZE(len) | ESC_PRAM_ADDR(address);
  lan9252_write_32(ESC_PRAM_RD_ADDR_LEN_REG, value);

  value = ESC_PRAM_CMD_BUSY;
  lan9252_write_32(ESC_PRAM_RD_CMD_REG, value);

  do {
    value = lan9252_read_32(ESC_PRAM_RD_CMD_REG);
  } while ((value & ESC_PRAM_CMD_AVAIL) == 0);

  /* Fifo count */
  fifo_cnt = ESC_PRAM_CMD_CNT(value);

  /* Read first value from FIFO */
  value = lan9252_read_32(ESC_PRAM_RD_FIFO_REG);
  fifo_cnt--;

  /* Find out first byte position and adjust the copy from that
   * according to LAN9252 datasheet and MicroChip SDK code
   */
  first_byte_position = (address & 0x03);
  temp_len =
      ((4 - first_byte_position) > len) ? len : (4 - first_byte_position);

  memcpy(temp_buf, ((uint8_t *)&value + first_byte_position), temp_len);
  len -= temp_len;
  byte_offset += temp_len;

  /* Select device. */
  spi_select(lan9252);
  /* Send command and address for fifo read */
  data[0] = ESC_CMD_FAST_READ;
  data[1] = ((ESC_PRAM_RD_FIFO_REG >> 8) & 0xFF);
  data[2] = (ESC_PRAM_RD_FIFO_REG & 0xFF);
  data[3] = ESC_CMD_FAST_READ_DUMMY;
  write(lan9252, data, sizeof(data));

  /* Continue reading until we have read len */
  while (len > 0) {
    temp_len = (len > 4) ? 4 : len;
    /* Always read 4 byte */
    read(lan9252, (temp_buf + byte_offset), sizeof(uint32_t));

    fifo_cnt--;
    len -= temp_len;
    byte_offset += temp_len;
  }
  /* Un-select device. */
  spi_unselect(lan9252);
}

/* ESC write process data ram function */
/* static */ void ESC_write_pram(uint16_t address, void *buf, uint16_t len) {
  uint32_t value;
  uint8_t *temp_buf = buf;
  uint16_t byte_offset = 0;
  uint8_t fifo_cnt, first_byte_position, temp_len, data[3];

  value = ESC_PRAM_CMD_ABORT;
  lan9252_write_32(ESC_PRAM_WR_CMD_REG, value);

  do {
    value = lan9252_read_32(ESC_PRAM_WR_CMD_REG);
  } while (value & ESC_PRAM_CMD_BUSY);

  value = ESC_PRAM_SIZE(len) | ESC_PRAM_ADDR(address);
  lan9252_write_32(ESC_PRAM_WR_ADDR_LEN_REG, value);

  value = ESC_PRAM_CMD_BUSY;
  lan9252_write_32(ESC_PRAM_WR_CMD_REG, value);

  do {
    value = lan9252_read_32(ESC_PRAM_WR_CMD_REG);
  } while ((value & ESC_PRAM_CMD_AVAIL) == 0);

  /* Fifo count */
  fifo_cnt = ESC_PRAM_CMD_CNT(value);

  /* Find out first byte position and adjust the copy from that
   * according to LAN9252 datasheet
   */
  first_byte_position = (address & 0x03);
  temp_len =
      ((4 - first_byte_position) > len) ? len : (4 - first_byte_position);

  memcpy(((uint8_t *)&value + first_byte_position), temp_buf, temp_len);

  /* Write first value from FIFO */
  lan9252_write_32(ESC_PRAM_WR_FIFO_REG, value);

  len -= temp_len;
  byte_offset += temp_len;
  fifo_cnt--;

  /* Select device. */
  spi_select(lan9252);
  /* Send command and address for incrementing write */
  data[0] = ESC_CMD_SERIAL_WRITE;
  data[1] = ((ESC_PRAM_WR_FIFO_REG >> 8) & 0xFF);
  data[2] = (ESC_PRAM_WR_FIFO_REG & 0xFF);
  write(lan9252, data, sizeof(data));

  /* Continue reading until we have read len */
  while (len > 0) {
    temp_len = (len > 4) ? 4 : len;
    value = 0;
    memcpy((uint8_t *)&value, (temp_buf + byte_offset), temp_len);
    /* Always write 4 byte */
    write(lan9252, (void *)&value, sizeof(value));

    fifo_cnt--;
    len -= temp_len;
    byte_offset += temp_len;
  }
  /* Un-select device. */
  spi_unselect(lan9252);
}

/** ESC read function used by the Slave stack.
 *
 * @param[in]   address     = address of ESC register to read
 * @param[out]  buf         = pointer to buffer to read in
 * @param[in]   len         = number of bytes to read
 */
void ESC_read(uint16_t address, void *buf, uint16_t len) {
  /* Select Read function depending on address, process data ram or not */
  if (address >= 0x1000) {
    ESC_read_pram(address, buf, len);
  } else {
    uint16_t size;
    uint8_t *temp_buf = (uint8_t *)buf;

    while (len > 0) {
      /* We write maximum 4 bytes at the time */
      size = (len > 4) ? 4 : len;
      /* Make size aligned to address according to LAN9252 datasheet
       * Table 12-14 EtherCAT CSR Address VS size and MicroChip SDK code
       */
      /* If we got an odd address size is 1 , 01b 11b is captured */
      if (address & BIT(0)) {
        size = 1;
      }
      /* If address 1xb and size != 1 and 3 , allow size 2 else size 1 */
      else if (address & BIT(1)) {
        size = (size & BIT(0)) ? 1 : 2;
      }
      /* size 3 not valid */
      else if (size == 3) {
        size = 1;
      }
      /* else size is kept AS IS */
      ESC_read_csr(address, temp_buf, size);

      /* next address */
      len -= size;
      temp_buf += size;
      address += size;
    }
  }
  /* To mimic the ET1100 always providing AlEvent on every read or write */
  ESC_read_csr(ESCREG_ALEVENT, (void *)&ESCvar.ALevent, sizeof(ESCvar.ALevent));
  ESCvar.ALevent = etohs(ESCvar.ALevent);
}

/** ESC write function used by the Slave stack.
 *
 * @param[in]   address     = address of ESC register to write
 * @param[out]  buf         = pointer to buffer to write from
 * @param[in]   len         = number of bytes to write
 */
void ESC_write(uint16_t address, void *buf, uint16_t len) {
  /* Select Write function depending on address, process data ram or not */
  if (address >= 0x1000) {
    ESC_write_pram(address, buf, len);
  } else {
    uint16_t size;
    uint8_t *temp_buf = (uint8_t *)buf;

    while (len > 0) {
      /* We write maximum 4 bytes at the time */
      size = (len > 4) ? 4 : len;
      /* Make size aligned to address according to LAN9252 datasheet
       * Table 12-14 EtherCAT CSR Address VS size  and MicroChip SDK code
       */
      /* If we got an odd address size is 1 , 01b 11b is captured */
      if (address & BIT(0)) {
        size = 1;
      }
      /* If address 1xb and size != 1 and 3 , allow size 2 else size 1 */
      else if (address & BIT(1)) {
        size = (size & BIT(0)) ? 1 : 2;
      }
      /* size 3 not valid */
      else if (size == 3) {
        size = 1;
      }
      // __disable_irq();
      /* else size is kept AS IS */
      ESC_write_csr(address, temp_buf, size);
      // __enable_irq();

      /* next address */
      len -= size;
      temp_buf += size;
      address += size;
    }
  }

  /* To mimic the ET1x00 always providing AlEvent on every read or write */
  /* 模仿ET1x00，每次读取或写入时始终提供AlEvent */
  ESC_read_csr(ESCREG_ALEVENT, (void *)&ESCvar.ALevent, sizeof(ESCvar.ALevent));
  ESCvar.ALevent = etohs(ESCvar.ALevent);
}

/* Un-used due to evb-lan9252-digio not havning any possability to
 * reset except over SPI.
 */
void ESC_reset(void) {}

uint8_t ESC_IsLAN9252() {
  volatile uint32_t value;
  uint16_t detectedChip, revision;

  /* Read */
  value = lan9252_read_32(ESC_ID_REV_REG);
  detectedChip = value >> 16;  // 取高16位
  revision = value & 0xFF;

  return detectedChip == LAN9252_ID_REV && revision >= 1;
}

void ESC_init(const esc_cfg_t *config) {
  uint32_t value, data;
  uint32_t intMask;

  spi_setup();

  rst_low();
  HAL_Delay(100);
  rst_high();
  HAL_Delay(100);

  /* 触发数字复位 */
  lan9252_write_32(ESC_RESET_CTRL_REG, ESC_DIGITAL_RST);
  do {
    value = lan9252_read_32(ESC_RESET_CTRL_REG);
  } while (value & ESC_RESET_CTRL_RST);  // 0100 0000 对应数字手册第42页

  /* Read test register 测试pdi接口 */
  do {
    value = lan9252_read_32(ESC_BYTE_TEST_REG);  // 0x064
  } while (value != ESC_TEST_VALUE);             // 0x87654321

  /* Check Ready flag 硬件配置寄存器 */
  do {
    value = lan9252_read_32(ESC_HW_CFG_REG);
  } while ((value & ESC_READY) == 0);  // 对应数据手册第304页

  if (!ESC_IsLAN9252()) {
    while (1)
      ;
  }  // 检查芯片版本，具体对应数据手册第302页

  /* Reset the ecat core here due to evb-lan9252-digio not having any GPIO
   * for that purpose.
   */
  // uint8_t dd = 0;
  // do {
  //   intMask = 0x0093;
  //   lan9252_write_32(ESCREG_ALEVENTMASK, intMask);
  //   intMask = 0;
  //   lan9252_write_32(ESCREG_ALEVENTMASK, intMask);
  //   if (dd++ > 5000) {
  //     HAL_GPIO_TogglePin(LED4_GPIO_Port, LED4_Pin);
  //   }
  // } while (intMask != 0x0093);

  // 配置IRQ enable ，IRQ polarity，IRQ buffer type
  data = 0x00000101;
  lan9252_write_32(ESC_IRQ_CFG_REG, data);  // 使能中断
  // 配置中断使能寄存器
  data = 0x00000001;
  lan9252_write_32(ESC_INT_EN_REG, data);  // 使能中断
  // 用于确认配置，可读取某个寄存器，例如 0x58
  lan9252_read_32(0x58);

  // 设置 AL 事件屏蔽寄存器为 所有位为1，即使所有 AL 事件都允许中断
  uint32_t dd = 0;
  do {
    intMask = 0x00;
    // intMask = 0xFFFFFFFF;
    ESC_ALeventmaskwrite(intMask);
    // if (dd++ > 5000) {
    //   HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
    //   dd = 0;
    // }
  } while (ESC_ALeventmaskread() != intMask);
  // ----- 中断相关配置代码结束 -----
}

uint8_t pdi_first_time_ = 0;
uint8_t sync0_first_time_ = 0;
/**
 * @brief 使能某个AL事件的中断
 *
 * @param mask
 * @note
 * 注意这里是因为使用的LAN9252支持标准的esc寄存器，所以可以直接使用soes库中esc.c中的接口
 *       直接往AL事件屏蔽寄存器写入掩码 0x204h
 */
void ESC_interrupt_enable(uint32_t mask) {
  // PDI interrupt
  ENABLE_ESC_PDI_INT;

  if (ESCREG_ALEVENT_DC_SYNC0 & mask) {
    ENABLE_ESC_SYNC0_INT;
    mask &= ~ESCREG_ALEVENT_DC_SYNC0;
  }
  if (ESCREG_ALEVENT_DC_SYNC1 & mask) {
    //    mask &= ~ESCREG_ALEVENT_DC_SYNC1;
  }
  if (ESCREG_ALEVENT_DC_LATCH & mask) {
    // mask &= ~ESCREG_ALEVENT_DC_LATCH;
  }

  ESC_ALeventmaskwrite(ESC_ALeventmaskread() | mask);
}

/**
 * @brief 失能某个AL事件的中断
 *
 * 掩码是要失能的那个，然后取反，就它变成0，其他都是1，然后独出现在什么是使能的，两个位相与，都是1才是1，也就是说，本来是1的，相与后仍然是1；
 * 原本不是1的，和1相与也是0
 * 但是掩码对应的那位是0，一和要失能的那位一与，就变成0了，这时，再将这个寄存器写入AL事件寄存器
 * 就可以在不干扰其它位的情况下，只失能掩码指定的那位中断
 * @param mask
 * @note
 * 注意这里是因为使用的LAN9252支持标准的esc寄存器，所以可以直接使用soes库中esc.c中的接口
 *       先ESC_ALeventread读出现在有什么事件，然后与掩码做与运算，最后写入AL事件寄存器
 */
void ESC_interrupt_disable(uint32_t mask) {
  DISABLE_ESC_PDI_INT;
  if (ESCREG_ALEVENT_DC_SYNC0 & mask) {
    DISABLE_ESC_SYNC0_INT;
    mask &= ~ESCREG_ALEVENT_DC_SYNC0;
  }
  if (ESCREG_ALEVENT_DC_SYNC1 & mask) {
    //    mask &= ~ESCREG_ALEVENT_DC_SYNC1;
  }
  if (ESCREG_ALEVENT_DC_LATCH & mask) {
    //    mask &= ~ESCREG_ALEVENT_DC_LATCH;
  }

  ESC_ALeventmaskwrite(~mask & ESC_ALeventmaskread());
}

uint32_t ESC_enable_DC(void) {
  uint8_t data = 0x00;

  /* check DC Sync Out bit: 0x140:10 检查DC使能标志 */
  ESC_read(ESCREG_ESC_CONFIG, &data, sizeof(data));
  if (!(data & DC_SYNC_OUT)) return 0;

  /* read set SYNC0 Cycle Time from 0x09A0 */
  uint32_t setsync0cycleTime = 0;
  ESC_read(ESCREG_SYNC0_CYCLE_TIME, &setsync0cycleTime, sizeof(uint32_t));
  setsync0cycleTime =
      etohl(setsync0cycleTime);  // 将读取到的周期时间转换为主机字节序

  // check Sync Unit assign 0x0980:0 ( 0 for ECAT, 1 for PDI )
  ESC_read(ESCREG_CYCLIC_UNIT_CONTROL, &data,
           sizeof(data));  // 检查同步单元分配给PDI 还是 ECAT
  if (data ==
      SYNC_OUT_PDI_CONTROL) {  // 如果
                               // 同步单元配置给了PDI，那么同步信号将由从站来控制
    // Sync Unit assigned to PDI, configuration needs to be finished by slave

    // set sync start time: read system time, add offset for writing start time
    // and activation
    /* 配置同步起始时间与激活同步 */
    ESC_read(ESCREG_LOCALTIME, (void *)&ESCvar.Time, sizeof(ESCvar.Time));
    ESCvar.Time = etohl(ESCvar.Time);
    uint32_t startTime = ESCvar.Time + SYNC_START_OFFSET;  // 配置同步起始时间

    ESC_write(ESCREG_SYNC_START_TIME, &startTime, sizeof(startTime));

    // activate cyclic operation and SYNC0 激活同步
    ESC_read(ESCREG_SYNC_ACT, &data,
             sizeof(data));  // 读取0981h寄存器的配置，是一个8位的数据
    data =
        data | ESCREG_SYNC_ACT_ACTIVATED |
        ESCREG_SYNC_SYNC0_EN;  // 然后对寄存器上的对应数据做使能，得到一个新的寄存器配置
    ESC_write(ESCREG_SYNC_ACT, &data, sizeof(data));  // 将寄存器配置再次写入
    data = 0x00;                                      // 清空缓存
    while (
        !(data &
          (ESCREG_SYNC_ACT_ACTIVATED |
           ESCREG_SYNC_SYNC0_EN))) {  // 0x00 & (0x01 | 0x02) --- > data & (0001
                                      // | 0010) --- > 检查data的前两位是否为1
      /* 如果接收到的data不合预期，就一直读 */
      ESC_read(ESCREG_SYNC_ACT, &data, sizeof(data));
    }
  }
  DPRINT("cycle time: %lu ns\r\n", setsync0cycleTime);
  return setsync0cycleTime;
}

uint32_t sync0cycleTime = 0;
int ESC_dc_watchdog_init(void) {
#define MIN_WATCHDOG_VALUE_NS 1000000  // 最小看门狗值为1毫秒（1000000纳秒）
  /* Indicate we run DC 全局变量标志位，说明系统启用了DC同步 */
  ESCvar.dcsync = 1;
  /* Fetch the sync counter limit (SDO10F1)
   * 为同步计数器设置一个上限值（通常来自SDO10F1），用于后续的看门狗计数判断 */
  ESCvar.synccounterlimit = 200;

  sync0cycleTime =
      ESC_enable_DC();  // 该函数内部会检测EtherCAT从站是否使能了DC同步功能，并读取SYNC0的周期时间
  int watchdog_value =
      2 * sync0cycleTime;  // 计算看门狗超时时间：将同步周期时间乘以2
  if (watchdog_value < MIN_WATCHDOG_VALUE_NS)
    watchdog_value =
        MIN_WATCHDOG_VALUE_NS;  // 如果计算结果小于最小值，则使用最小值
  return watchdog_value;
}