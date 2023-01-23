#ifndef CAN_H
#define CAN_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct can_pkg_t can_pkg_t;
struct can_pkg_t {
  uint32_t id;
  uint8_t * data;
  uint8_t len;
};

typedef enum {
  CAN_ERROR_NONE=0x0U,
  CAN_ERROR_EPV =0x1U,
  CAN_ERROR_BOF =0x2U,
  CAN_ERROR_STF =0x4U,
  CAN_ERROR_FOR =0x8U
} can_error_t;

void can_init();
void can_handle();
int can_send(uint8_t *data, uint8_t len, void (*callback)());
int can_register_id(uint32_t id,  void (*callback)(can_pkg_t *pkg));
uint8_t can_get_free_tx();
can_error_t can_get_errors();

#endif
