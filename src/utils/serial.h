#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define SER_MAX_COMMANDS (10)
#define SER_CMDBUFLEN (100)
// currently, longest ack is 'NACK\n'
#define SER_MAX_ACKLEN (5)
#define SER_CMDBUFCNT (10)
// maximum length a command function can write to its response buffer
#define SER_MAX_RESPLEN (SER_CMDBUFLEN-SER_MAX_ACKLEN-4-1)

typedef struct {
  char cmd;
  uint8_t (*cb)(char*, char*);
} ser_cmds_TypeDef;

typedef struct ser_buf_TypeDef ser_buf_TypeDef;
struct ser_buf_TypeDef {
  char buf[SER_CMDBUFLEN+1];
  bool used;
  ser_buf_TypeDef * next_buffer;
};

void ser_init();
void ser_handle();
uint8_t ser_addcmd(char cmd, uint8_t (*func)(char*,char*));
void ser_txdata(ser_buf_TypeDef * buffer);
ser_buf_TypeDef * ser_get_free_buf();

#endif
