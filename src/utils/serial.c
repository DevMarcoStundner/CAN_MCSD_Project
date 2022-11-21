#include "serial.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "libll/stm32l4xx_ll_bus.h"
#include "libll/stm32l4xx_ll_usart.h"

typedef struct {
  char cmd;
  uint8_t (*cb)(char*, char*);
} ser_cmds_TypeDef;

typedef enum {
  CMD_ACK, 
  CMD_NACK
} ser_respack_TypeDef;

typedef struct {
  char buf[SER_CMDBUFLEN+1];
  bool free;
} ser_buf_TypeDef;

static ser_cmds_TypeDef commands[SER_MAX_COMMANDS] = {[0 ... SER_MAX_COMMANDS-1] = {'\0',NULL}};
static ser_buf_TypeDef cmd_buffers[SER_CMDBUFCNT] = {0};
static volatile ser_buf_TypeDef * cur_rx_buf = NULL;
static volatile ser_buf_TypeDef * cur_tx_buf = NULL;

void USART1_IRQHandler(void) {
  // if char match
  if (LL_USART_IsActiveFlag_CM(USART1)) {
    // mark buffer as valid
    // disable dma
    // switch to new buffer
    // enable dma
    // re-enable rx?
  } else if (LL_USART_IsActiveFlag_TC(USART1)) {
  // if tx complete
    // dma should be finished
    // mark buffer as free
  } else if (LL_USART_IsActiveFlag_ORE(USART1)) {
  // if rx overrun
  }
}

// USART1_TX
void DMA1_Channel4_IRQHandler(void){
  // if dma complete
    // just clear flag, we need to wait for usart tx complete
}

// USART1_RX
void DMA1_Channel5_IRQHandler(void){
  // if complete 
    // character match from uart should fire
    // check if last char is \n
    // else error response
      // mark buffer as free
      // send error response
      // restart dma on same buffer
}

void ser_init() {
  // enable clks
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);
  LL_USART_InitTypeDef usart_init;
  LL_USART_StructInit(&usart_init);
  // config baud rate
  usart_init.BaudRate = 9600;
  usart_init.TransferDirection = LL_USART_DIRECTION_NONE;
  LL_USART_Init(USART1, &usart_init);
  // config interrupts
  LL_USART_EnableIT_TC(USART1);
  LL_USART_EnableIT_CM(USART1);
  LL_USART_EnableIT_ERROR(USART1);
  // config character match
  LL_USART_ConfigNodeAddress(USART1, LL_USART_ADDRESS_DETECT_7B, '\n');

  // config dma for rx and tx
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
  // enable dma
  // enable uart rx
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

static void handle_command(ser_buf_TypeDef * buffer) {
  char scratchpad[SER_MAX_RESPLEN+1];
  scratchpad[SER_MAX_RESPLEN] = '\0';
  // check for sof
  // check for single eof
  if ((buffer->buf[0] == '#') && (strchr(buffer->buf,'\n') != NULL)) {
    for (int i=0; i<SER_MAX_COMMANDS; i++) {
      char cmd = commands[i].cmd;
      if ((isalpha(cmd)) && (buffer->buf[1] == cmd)) {
        // call command function
        uint8_t retval = commands[i].cb(scratchpad, strchr(buffer->buf, ','));
        if (retval) {
          snprintf(buffer->buf, SER_CMDBUFLEN, "NACK\n#?%s\n", scratchpad);
        } else {
          snprintf(buffer->buf, SER_CMDBUFLEN, "ACK\n#%c%s\n", cmd, scratchpad);
        }
        break;
      } else {
        snprintf(buffer->buf, SER_CMDBUFLEN, "NACK\n#?Command was not found\n");
        break;
      }
    }
  }
  // send response
}
