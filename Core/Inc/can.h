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



#define reg_len 20

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
  CAN_EPV_ERROR			 = (0x00000002U),
  CAN_BOF_ERROR			 = (0x00000004U),
  CAN_STF_ERROR			 = (0x00000008U),
  CAN_FOR_ERROR			 = (0x00000010U)
} CAN_Status;

/**
  * @brief  CAN Pkg
  */
typedef struct can_pkg_t can_pkg_t;
struct can_pkg_t {
  int id;
  uint8_t * data;
  uint8_t len;
  bool full;
};

/**
  * @brief  CAN Status structures definition public
  */
typedef enum {
  CAN_ERROR_NONE	=0x0U,
  CAN_ERROR_EPV 	=0x1U,
  CAN_ERROR_BOF 	=0x2U,
  CAN_ERROR_STF 	=0x4U,
  CAN_ERROR_FOR 	=0x8U
} can_error_t;

can_pkg_t can_pkg_reg[reg_len];


void can_init();
void can_handle();
int can_send_pkg(uint8_t *data, uint8_t len, void (*callback)(uint32_t Mailbox));
int can_register_id(uint32_t id,  void (*callback)(can_pkg_t *pkg));
uint32_t can_get_free_tx();
can_error_t can_get_errors();

/** brief Function _can_receive_pkg() will receive the data and assigns Id
 *  param hcan is the handle for the HAL
 *  param RxFifo Fifo number of the received message to be read
 *  param pHeader is the struct for the RxHeader
 *  param aData is the data-array
 *  returns CAN_OK if no error occurs
 *  returns CAN_ERROR if one of the HAL functions wont work
 */
void _can_receive_pkg(uint8_t *Data);



#endif /* INC_CAN_H_ */
