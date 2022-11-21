#include "serial.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

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

void ser_init() {
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
  //scratchpad[SER_MAX_RESPLEN] = '\0';
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
