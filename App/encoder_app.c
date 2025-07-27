/**
 * @file encoder_app.c
 * @author Keten (2863861004@qq.com)
 * @brief
 * @version 0.1
 * @date 2025-07-15
 *
 * @copyright Copyright (c) 2025
 *
 * @attention :
 * @note : 测试简单协议，包头 0xDA 0xDB，包尾 0xDC 0xDE ,中间塞两个浮点数
 * ---完成测试
 *
 * @versioninfo :
 */
#include "encoder_app.h"

#include "bsp_dwt.h"
#include "bsp_gpio.h"
#include "esc_config.h"

extern DMA_HandleTypeDef ENCODER_DMA_Rx_Handle;
extern DMA_HandleTypeDef ENCODER_DMA_Tx_Handle;

BSP_UART_RxQueueType Encoder_Uart_Rx_Queue;  // 编码器接收队列
BSP_UART_TxQueueType Encoder_Uart_Tx_Queue;  // 编码器发送队列

uint8_t Encoder_Uart_Rx_Buf[64];   // 编码器接收缓冲区
uint8_t Encoder_Uart_Tx_Buf[256];  // 编码器发送缓冲区

/* 一些写入编码器设置信息的信号,如果收到编码器回传的信号确定成功后可以通知其他地方
 */
bool if_setting_encoder_zero_success = false;             // 写入编码器归零
bool if_setting_encoder_direction_success = false;        // 写入编码器递增方向
bool if_setting_encoder_specified_value_success = false;  // 写入指定值

BSP_UART_Instance_t Encoder_Uart_Instance = {
    .huart = &ENCODER_UART,
    .mode = BothReceiveAndSend,
    .hdma_rx = &ENCODER_DMA_Rx_Handle,
    .hdma_tx = &ENCODER_DMA_Tx_Handle,
    .rx_queue = &Encoder_Uart_Rx_Queue,  // 初始化接收队列
    .tx_queue = &Encoder_Uart_Tx_Queue,  // 初始化发送队列
    .rs485_enable = 1,                   // 启用RS485
    .gpio_port = 'G',
    .DE_Pin = 13,                    // RS485 DE引脚
    .dma_buf = Encoder_Uart_Rx_Buf,  // DMA接收缓冲区
    .dma_buf_size = sizeof(Encoder_Uart_Rx_Buf),
    .tx_dma_buf = Encoder_Uart_Tx_Buf,           // DMA发送缓冲区
    .IdleCallback = Encoder_Uart_Idle_Callback,  // 用户回调函数
};

union FloatToBytes {
  float f;
  uint8_t bytes[4];
} float_to_bytes[1];  // 用于将float转换为字节数组

EncoderData encoder_data = {0};  // 编码器数据结构初始化

static inline int32_t encoder_delta(uint32_t prev, uint32_t curr) {
  int32_t delta = (int32_t)(curr - prev);
  if (delta > ENCODER_HALF) {
    delta -= ENCODER_MAX + 1;  // 如果增量大于半个周期，说明跨越了零点
  } else if (delta < -ENCODER_HALF) {
    delta += ENCODER_MAX + 1;
  }
  return delta;
}

static inline int crc_calculate(unsigned char pbuf[], unsigned char num) {
  int i, j;
  unsigned int wcrc = 0xffff;
  for (i = 0; i < num; i++) {
    wcrc ^= (unsigned int)(pbuf[i]);
    for (j = 0; j < 8; j++) {
      if (wcrc & 0x0001) {
        wcrc >>= 1;
        wcrc ^= 0xa001;
      } else {
        wcrc >>= 1;
      }
    }
  }
  return wcrc;
}

void Encoder_Device_Init(void) {
  DMA_Configuration();                // 初始化DMA
  Uart_Configuration();               // 初始化串口
  EncoderRs485IO_DE_Configuration();  // 初始化控制脚

  BSP_Uart_Init(&Encoder_Uart_Instance);  // 初始化编码器串口设备
}

// 本函数将会放在中断上下文！不允许使用阻塞操作
void Encoder_Uart_Idle_Callback(struct BSP_UART_Instance_t *h, uint16_t len) {
  uint8_t buf[64];
  int avail = BSP_UART_RxQueueType_get_length(h->rx_queue);
  // 取最小值
  int to_read = (len < avail) ? len : avail;
  for (int i = 0; i < to_read; i++) {
    BSP_UART_RxQueueType_dequeue(h->rx_queue, &buf[i]);
  }
  // 数据包解析

  Encoder_GetData(&buf, to_read);
}

#ifdef TEST_ENCODER_TIME
float last_get_data = 0, dt_get_data = 0;
#endif

// 0 解析成功 -1 解析失败
int Encoder_GetData(uint8_t *buf, uint8_t len) {
  assert_param(buf != NULL);

  //  检查设备地址(写死一个设备了,留给后人去扩展)
  if (buf[0] != 1) {
    return -1;
  }
  // 检查功能码
  if (buf[1] != 0x03) {
    return -1;
  }
  uint8_t byte_count = buf[2];
  if (byte_count < 4) {
    return -1;
  }
  uint16_t crc_recv =
      (buf[3 + byte_count + 1] << 8) | buf[3 + byte_count];  // 接收的CRC值
  uint16_t crc_cal = crc_calculate(buf, 3 + byte_count);     // 计算CRC值
  if (crc_recv != crc_cal) {
    return -1;  // CRC校验失败
  }
#ifdef TEST_ENCODER_TIME
  dt_get_data = DWT_GetTimeline_ms() - last_get_data;
  last_get_data = DWT_GetTimeline_ms();

#endif
  encoder_data.Encoder_curt = (buf[3] << 24) | (buf[4] << 16 | (buf[5] << 8) |
                                                (buf[6]));  // 更新当前编码器值
  encoder_data.delta = encoder_delta(encoder_data.Encoder_last,
                                     encoder_data.Encoder_curt);  // 计算增量
  encoder_data.distance -=
      (float)encoder_data.delta / ENCODER_CYCLE * ENCODER_FACTOR;
  encoder_data.Encoder_last = encoder_data.Encoder_curt;  // 更新上次编码器值
  return 0;
}

// 如果关闭自动重传，需要定时发送查询编码器数据
int Encoder_SendStartMsg(void) {
  uint8_t start_msg[8] = {1, 0x03, 0x00, 0x00, 0x00, 0x02};  // 示例起始消息
  uint16_t crc = crc_calculate(start_msg, sizeof(start_msg) - 2);
  start_msg[6] = (crc >> 8) & 0xFF;  // CRC高字节
  start_msg[7] = crc & 0xFF;         // CRC低字节

  for (int i = 0; i < sizeof(start_msg); i++) {
    BSP_UART_TxQueueType_enqueue(Encoder_Uart_Instance.tx_queue, start_msg[i]);
  }

  return BSP_UART_SendData(&Encoder_Uart_Instance, start_msg,
                           sizeof(start_msg));
}

// 01 06 00 07 00 14 38 04
/**
 * @brief 设置回报率
 *
 * @param return_rate 可调范围：0 ~ 65535
 * ,注意：一旦编码器回传速率小于20ms，设置其他参数容易失败
 * @return int
 */
int Encoder_ConfigureReturnRate(int return_rate) {}

void test_RS485Send(void) {
  // const uint8_t msg[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x02,
  //                        0xDA, 0xDB, 0xDC, 0xDE};  //
  //                        示例消息，包含包头和包尾
  const uint8_t msg[] = "cao ni ma!\r\n";

  // 通过 BSP_UART_Transmit 发送到上位机
  // 注意长度不包括末尾的 '\0'
  BSP_UART_SendData(&Encoder_Uart_Instance, msg, sizeof(msg));
}

void handle_encoder_pos(EncoderData *encoder_data) {
  encoder_data->angle_splitter.f = (float)encoder_data->distance;
}

void Encoder_SetZero(void) {
  uint8_t set_zero_msg[8] = {0x01, 0x06, 0x00, 0x08, 0x00, 0x01};
  uint16_t crc = crc_calculate(set_zero_msg, sizeof(set_zero_msg) - 2);
  set_zero_msg[6] = crc & 0xFF;         // CRC低字节
  set_zero_msg[7] = (crc >> 8) & 0xFF;  // CRC高字节
  for (int i = 0; i < sizeof(set_zero_msg); i++) {
    BSP_UART_TxQueueType_enqueue(Encoder_Uart_Instance.tx_queue,
                                 set_zero_msg[i]);
  }

  BSP_UART_SendData(&Encoder_Uart_Instance, set_zero_msg,
                    sizeof(set_zero_msg));  // 发送设置归零消息
}