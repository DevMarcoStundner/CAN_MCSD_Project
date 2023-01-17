/*
 * can.h
 *
 *  Created on: Jan 11, 2023
 *      Author: marco
 */

#ifndef INC_CAN_H_
#define INC_CAN_H_

/**
  * @brief  CAN Status structures definition
  */
typedef enum
{
  CAN_OK      			 = 0x00,
  CAN_FILTER_ERROR   	 = 0x01,
  CAN_START_ERROR    	 = 0x02,
  CAN_ACTIVATE_IT_ERROR  = 0x03,
  CAN_MAILBOX_ERROR		 = 0x04,
  CAN_MSG_ERROR			 = 0x05
} CAN_Status;

/**
  * @brief  CAN Id structures definition
  */
typedef enum
{
	MOTOR  = 0x00,
	ENCODER = 0x01
}	ID;

// Callbackfunctions
void CAN_Motor_Callback();
void CAN_Encoder_Callback();

/** brief Function _can_init() will config the filter and the TxHeader
 *  param hcan is the handle for the HAL
 *  param CAN_Filter is the struct for the Filter
 *  param CanTx is the struct for the TxHeader
 *  returns CAN_OK if no error occurs
 *  returns CAN_ERROR if one of the HAL functions wont work
 */
CAN_Status _can_init(CAN_HandleTypeDef hcan, CAN_FilterTypeDef CAN_Filter, CAN_TxHeaderTypeDef CanTx);

/** brief Function _can_send_pkg() will send the data and checks if the mailbox is full
 *  param hcan is the handle for the HAL
 *  param pHeader is the struct for the TxHeader
 *  param aData is the data-array
 *  param pTxMailbox is which mailbox is used
 *  returns CAN_OK if no error occurs
 *  returns CAN_ERROR if one of the HAL functions wont work
 */
CAN_Status _can_send_pkg(CAN_HandleTypeDef *hcan, const CAN_TxHeaderTypeDef *pHeader, const uint8_t aData[], uint32_t *pTxMailbox);

/** brief Function _can_receive_pkg() will receive the data and assigns Id
 *  param hcan is the handle for the HAL
 *  param RxFifo Fifo number of the received message to be read
 *  param pHeader is the struct for the RxHeader
 *  param aData is the data-array
 *  returns CAN_OK if no error occurs
 *  returns CAN_ERROR if one of the HAL functions wont work
 */
CAN_Status _can_receive_pkg(CAN_HandleTypeDef *hcan, uint32_t RxFifo, CAN_RxHeaderTypeDef *pHeader, uint8_t aData[], ID id);


#endif /* INC_CAN_H_ */
