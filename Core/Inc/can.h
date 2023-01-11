/*
 * can.h
 *
 *  Created on: Jan 11, 2023
 *      Author: marco
 */

#ifndef INC_CAN_H_
#define INC_CAN_H_



/** brief Function _uart_transmit() will transmit data with uart

 *  returns void
 */
HAL_StatusTypeDef _can_init(CAN_HandleTypeDef hcan, CAN_FilterTypeDef CAN_Filter, CAN_TxHeaderTypeDef CanTx);


#endif /* INC_CAN_H_ */
