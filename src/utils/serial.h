#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

#define SER_MAX_COMMANDS (10)
#define SER_CMDBUFLEN (100)
// currently, longest ack is 'NACK\n'
#define SER_MAX_ACKLEN (5)
#define SER_CMDBUFCNT (10)
// maximum length a command function can write to its response buffer
#define SER_MAX_RESPLEN (SER_CMDBUFLEN-SER_MAX_ACKLEN-4-1)

void ser_init();
void ser_handle();

#endif
