/*
 * can.h
 *
 *  Created on: Jan 11, 2023
 *      Author: marco
 */

#ifndef INC_CAN_H_
#define INC_CAN_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define  CAN_ERROR_NONE	0x0U
#define  CAN_ERROR_EPV 	0x1U
#define  CAN_ERROR_BOF 	0x2U
#define  CAN_ERROR_STF 	0x4U
#define  CAN_ERROR_FOR 	0x8U

typedef struct can_pkg_t can_pkg_t;
struct can_pkg_t {
  uint32_t id;
  uint8_t * data;
  uint8_t len;
};

void can_init();
void can_handle();
int can_send_pkg(uint32_t pkgid, uint8_t *data, uint8_t len, void (*callback)(uint32_t mailbox));
int can_register_id(uint32_t id,  void (*callback)(can_pkg_t *pkg));
uint32_t can_get_free_tx();
int can_get_errors();
void _can_receive_pkg();



#endif /* INC_CAN_H_ */
