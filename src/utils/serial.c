#include "serial.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "libll/stm32l4xx_ll_bus.h"
#include "libll/stm32l4xx_ll_usart.h"
#include "libll/stm32l4xx_ll_dma.h"
#include "libll/stm32l4xx_ll_gpio.h"

typedef struct {
  char cmd;
  uint8_t (*cb)(char*, char*);
} ser_cmds_TypeDef;

typedef enum {
  CMD_ACK, 
  CMD_NACK
} ser_respack_TypeDef;

typedef struct ser_buf_TypeDef ser_buf_TypeDef;
struct ser_buf_TypeDef {
  char buf[SER_CMDBUFLEN+1];
  bool used;
  ser_buf_TypeDef * next_buffer;
};

static ser_cmds_TypeDef commands[SER_MAX_COMMANDS] = {[0 ... SER_MAX_COMMANDS-1] = {'\0',NULL}};
static ser_buf_TypeDef cmd_buffers[SER_CMDBUFCNT] = {0};
static ser_buf_TypeDef * volatile cur_rx_buf = NULL;
static ser_buf_TypeDef * volatile cur_tx_buf = NULL;

static void handle_command(ser_buf_TypeDef * buffer);
static void transmit_response(ser_buf_TypeDef *buffer);
static ser_buf_TypeDef * get_free_buf();
static ser_buf_TypeDef * get_first_buf(ser_buf_TypeDef * startbuf);
static ser_buf_TypeDef * get_last_buf(ser_buf_TypeDef * startbuf);
static ser_buf_TypeDef * get_prev_buf(ser_buf_TypeDef * startbuf);

void USART1_IRQHandler(void) {
  // if char match
  if (LL_USART_IsActiveFlag_CM(USART1)) {
    LL_USART_ClearFlag_CM(USART1);
    // mark buffer as valid
    //LL_USART_DisableDirectionRx(USART1);
    cur_rx_buf->used = true;
    LL_USART_EnableIT_IDLE(USART1);
    // re-enable rx?
  } else if (LL_USART_IsActiveFlag_IDLE(USART1)) {
    LL_USART_DisableIT_IDLE(USART1);
    LL_USART_ClearFlag_IDLE(USART1);
    // disable dma
    if (cur_rx_buf->used == true) {
      LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_5);
      // switch to new buffer
      ser_buf_TypeDef * newbuf = get_free_buf();
      if (newbuf == NULL) {
        cur_rx_buf->used = false;
      } else {
        cur_rx_buf->next_buffer = newbuf;
        cur_rx_buf = newbuf;
      }
      while (LL_DMA_IsEnabledChannel(DMA1, LL_DMA_CHANNEL_5));
      LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_5, (uint32_t)cur_rx_buf);
      LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_5, SER_CMDBUFLEN);
      // re-enable dma
      LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_5);
      LL_USART_EnableDirectionRx(USART1);
    }
  } else if (LL_USART_IsActiveFlag_TC(USART1)) {
    LL_USART_ClearFlag_TC(USART1);
  // if tx complete
    // dma should be finished
  } else if (LL_USART_IsActiveFlag_ORE(USART1)) {
  // if rx overrun
  }
}

// USART1_TX
void DMA1_Channel4_IRQHandler(void){
  // if dma complete
  if (LL_DMA_IsActiveFlag_TC4(DMA1)) {
    LL_DMA_ClearFlag_TC5(DMA1);
    LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_4);
    cur_tx_buf->used=false;
    if (cur_tx_buf->next_buffer != NULL) {
      cur_tx_buf = cur_tx_buf->next_buffer;
      LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_4, (uint32_t)cur_tx_buf);
      LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_4, strlen(cur_tx_buf->buf));
      LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_4);
    } else {
      cur_tx_buf = NULL;
    }
  }
}

// USART1_RX
void DMA1_Channel5_IRQHandler(void){
  // if complete 
  if (LL_DMA_IsActiveFlag_TC5(DMA1)) {
    LL_DMA_ClearFlag_TC5(DMA1);
    // command was too long
    if (cur_rx_buf->used == false) {
      ser_buf_TypeDef * txbuf = get_free_buf();
      if (txbuf != NULL) {
        sprintf(txbuf->buf, "NACK\n#?,Command too long\n");
        transmit_response(txbuf);
      }
      LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_5, (uint32_t)cur_rx_buf);
      LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_5, SER_CMDBUFLEN);
      // enable dma
      LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_5);
    }
  }
  // else error response
    // mark buffer as free
    // send error response
    // restart dma on same buffer
}

void ser_handle() {
  // no command/response is waited on
  if (cur_rx_buf != NULL) {
    ser_buf_TypeDef * buf = get_first_buf(cur_rx_buf);
    if (buf != NULL) {
      handle_command(buf);
    }
  }
}

void ser_init() {
  // config io pins
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
  LL_GPIO_InitTypeDef gpio_init;
  LL_GPIO_StructInit(&gpio_init);
  gpio_init.Pin = LL_GPIO_PIN_9;
  gpio_init.Mode = LL_GPIO_MODE_ALTERNATE;
  gpio_init.Alternate = LL_GPIO_AF_7;
  LL_GPIO_Init(GPIOA, &gpio_init);
  gpio_init.Pin = LL_GPIO_PIN_10;
  LL_GPIO_Init(GPIOA, &gpio_init);

  // enable clks
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);
  LL_USART_InitTypeDef usart_init;
  LL_USART_StructInit(&usart_init);
  // config baud rate
  usart_init.BaudRate = 9600;
  usart_init.TransferDirection = LL_USART_DIRECTION_TX_RX;
  LL_USART_Init(USART1, &usart_init);
  //LL_USART_SetRXPinLevel(USART1, LL_USART_RXPIN_LEVEL_INVERTED);
  //LL_USART_SetTXPinLevel(USART1, LL_USART_TXPIN_LEVEL_INVERTED);
  //LL_USART_ClearFlag_TC(USART1);
  LL_USART_EnableDMAReq_RX(USART1);
  LL_USART_EnableDMAReq_TX(USART1);
  // config interrupts
  //LL_USART_DisableIT_TC(USART1);
  LL_USART_EnableIT_CM(USART1);
  LL_USART_EnableIT_ERROR(USART1);
  NVIC_EnableIRQ(USART1_IRQn);
  NVIC_SetPriority(USART1_IRQn, 0x02);
  // config character match
  LL_USART_ConfigNodeAddress(USART1, LL_USART_ADDRESS_DETECT_7B, '\n');

  cur_rx_buf = &cmd_buffers[0];

  // config dma for rx and tx
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
  LL_DMA_InitTypeDef dma_init;
  // dma for rx
  LL_DMA_StructInit(&dma_init);
  dma_init.PeriphOrM2MSrcAddress = (uint32_t)&(USART1->RDR);
  dma_init.MemoryOrM2MDstAddress = (uint32_t)cur_rx_buf->buf;
  dma_init.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
  dma_init.Direction = LL_DMA_DIRECTION_PERIPH_TO_MEMORY;
  dma_init.NbData = SER_CMDBUFLEN;
  dma_init.PeriphRequest = LL_DMA_REQUEST_2;
  dma_init.Priority = LL_DMA_PRIORITY_HIGH;
  LL_DMA_Init(DMA1, LL_DMA_CHANNEL_5, &dma_init);

  // dma for tx
  dma_init.PeriphOrM2MSrcAddress = (uint32_t)&(USART1->TDR);
  dma_init.MemoryOrM2MDstAddress = (uint32_t)NULL;
  dma_init.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
  dma_init.NbData = 0;
  dma_init.PeriphRequest = LL_DMA_REQUEST_2;
  dma_init.Priority = LL_DMA_PRIORITY_MEDIUM;
  LL_DMA_Init(DMA1, LL_DMA_CHANNEL_4, &dma_init);
  // config dma interrups
  LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_5);
  LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_4);
  NVIC_EnableIRQ(DMA1_Channel5_IRQn);
  NVIC_SetPriority(DMA1_Channel5_IRQn, 0x03);
  NVIC_EnableIRQ(DMA1_Channel4_IRQn);
  NVIC_SetPriority(DMA1_Channel4_IRQn, 0x03);
  // enable dma
  LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_5);
  // enable uart
  LL_USART_Enable(USART1);
}

/*
 * @brief Add command to console system
 * @note the maximum number of times this function can be called depends on SER_MAX_COMMANDS
 * @param cmd character with which to call the command
 * @param func callback to call to get results string
 * @param argcnt number of arguments to expect
 * @return 0 when successful, >0 when no free slots or incorrect arguments
 */
uint8_t ser_addcmd(char cmd, uint8_t (*func)(char*,char*)) {
  if ((cmd == '\0') || (func == NULL)) return 1;
  for (int i=0; i<SER_MAX_COMMANDS; i++) {
    if (commands[i].cmd == '\0') {
      commands[i].cmd = cmd;
      commands[i].cb = func;
      return 0;
    }
  }
  return 1;
}

static ser_buf_TypeDef * get_free_buf() {
  for (int i=0; i<SER_CMDBUFCNT; i++) {
    ser_buf_TypeDef * buf = &cmd_buffers[i];
    if ((buf->used == false) && (buf != cur_rx_buf) && (buf != cur_tx_buf)) {
      return buf;
    }
  }
  return NULL;
}

static ser_buf_TypeDef * get_first_buf(ser_buf_TypeDef * startbuf) {
  ser_buf_TypeDef * buf = startbuf;
  ser_buf_TypeDef * tmp = NULL;
  do {
    tmp = NULL;
    for (int i=0; i<SER_CMDBUFCNT; i++) {
      if (cmd_buffers[i].next_buffer == buf) {
        buf = &cmd_buffers[i];
        tmp = buf;
        break;
      }
    }
  } while (tmp != NULL);
  return (startbuf == buf)?NULL:buf;
}

static ser_buf_TypeDef * get_last_buf(ser_buf_TypeDef * startbuf) {
  ser_buf_TypeDef * buf = startbuf;
  int i=0;
  while (buf->next_buffer != NULL) {
    if (buf == startbuf) {
      // circular loop
      return NULL;
    }
    buf = buf->next_buffer;
    i++;
  }
  return buf;
}

static ser_buf_TypeDef * get_prev_buf(ser_buf_TypeDef * startbuf) {
  ser_buf_TypeDef * tmp = NULL;
  for (int i=0; i<SER_CMDBUFCNT; i++) {
    if (cmd_buffers[i].next_buffer == startbuf) {
      tmp = startbuf;
      break;
    }
  }
  return tmp;
}

static void transmit_response(ser_buf_TypeDef *buffer) {
  if (buffer != NULL) {
    buffer->used = true;
    if (cur_tx_buf == NULL) {
      buffer->next_buffer = NULL;
      cur_tx_buf = buffer;
      // dma setup
      LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_4, strlen(buffer->buf));
      LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_4, (uint32_t)buffer->buf);
      LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_4);
    } else {
      get_last_buf(cur_tx_buf)->next_buffer = buffer;
    }
  }
}

static void handle_command(ser_buf_TypeDef * buffer) {
  char scratchpad[SER_MAX_RESPLEN+1];
  scratchpad[SER_MAX_RESPLEN] = '\0';
  buffer->next_buffer = NULL;
  // check for sof
  // check for single eof
  if ((buffer->buf[0] == '#') && (strchr(buffer->buf,'\n') != NULL)) {
    for (int i=0; i<SER_MAX_COMMANDS; i++) {
      char cmd = commands[i].cmd;
      if ((isalpha(cmd)) && (buffer->buf[1] == cmd)) {
        // call command function
        uint8_t retval = commands[i].cb(scratchpad, strchr(buffer->buf, ','));
        if (retval) {
          snprintf(buffer->buf, SER_CMDBUFLEN, "NACK\n#?,%s\n", scratchpad);
        } else {
          snprintf(buffer->buf, SER_CMDBUFLEN, "ACK\n#%c,%s\n", cmd, scratchpad);
        }
        break;
      } else {
        snprintf(buffer->buf, SER_CMDBUFLEN, "NACK\n#?,Command was not found\n");
        break;
      }
    }
  }
  // send response
  transmit_response(buffer);
}
