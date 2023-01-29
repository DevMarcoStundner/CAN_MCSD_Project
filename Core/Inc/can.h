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

#define REG_LEN 20
#define RQCP0 0
#define RQCP1 8
#define RQCP2 16

#define  CAN_ERROR_NONE	0x0U
#define  CAN_ERROR_EPV 	0x1U
#define  CAN_ERROR_BOF 	0x2U
#define  CAN_ERROR_STF 	0x4U
#define  CAN_ERROR_FOR 	0x8U

/**
  * @brief  CAN Status structures definition private
  */
typedef enum
{
  CAN_OK      			 = 0x00,
  CAN_ERROR				 = 0x09,
  CAN_FILTER_ERROR   	 = 0x01,
  CAN_START_ERROR    	 = 0x06,
  CAN_ACTIVATE_IT_ERROR  = 0x03,
  CAN_MAILBOX_ERROR		 = 0x07,
  CAN_MSG_ERROR			 = 0x05,
  CAN_EPV_ERROR			 = 0x02U,
  CAN_BOF_ERROR			 = 0x04U,
  CAN_STF_ERROR			 = 0x08U,
  CAN_FOR_ERROR			 = 0x10U
} CAN_Status;

/**
  * @brief  CAN Pkg
  */
typedef struct can_pkg_t can_pkg_t;
struct can_pkg_t {
  uint32_t id;
  uint8_t * data;
  uint8_t len;
};

typedef struct rxcallback rxcallback;
struct rxcallback {
	uint8_t id;
	void(*rxcb)(can_pkg_t *pkg);
};




void can_init();
void can_handle();
int can_send_pkg(uint32_t pkgid, uint8_t *data, uint8_t len, void (*callback)(uint32_t mailbox));
int can_register_id(uint32_t id,  void (*callback)(can_pkg_t *pkg));
uint32_t can_get_free_tx();
int can_get_errors();
void _can_receive_pkg();



#endif /* INC_CAN_H_ */
