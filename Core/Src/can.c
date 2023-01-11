/*
 * can.c
 *
 *  Created on: Jan 11, 2023
 *      Author: marco
 */
#include "main.h"
#define CAN_ID ((uint32_t)0x1314)

HAL_StatusTypeDef _can_init(CAN_HandleTypeDef hcan, CAN_FilterTypeDef CAN_Filter, CAN_TxHeaderTypeDef CanTx)
{

	  CanTx.DLC = 8;														//Data length
	  CanTx.ExtId = CAN_ID;
	  CanTx.IDE = CAN_ID_EXT;												// Extended frame mode
	  CanTx.RTR = CAN_RTR_DATA;												//Data Frame
	  CanTx.StdId = 0x00;													// Replace zero when using extended frames
	  CanTx.TransmitGlobalTime = DISABLE;									// Timestamp

	  CAN_Filter.FilterMode = CAN_FILTERMODE_IDMASK;
	  CAN_Filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	  CAN_Filter.FilterBank = 0;
	  CAN_Filter.FilterScale = CAN_FILTERSCALE_32BIT;
	  CAN_Filter.FilterIdHigh = 0x000;
	  CAN_Filter.FilterIdLow = 0x000;
	  CAN_Filter.FilterMaskIdHigh = 0x000;
	  CAN_Filter.FilterMaskIdLow = 0x000;
	  CAN_Filter.SlaveStartFilterBank = 0;
	  CAN_Filter.FilterActivation = ENABLE;

	  if(HAL_CAN_ConfigFilter(&hcan, &CAN_Filter) != HAL_OK)
	  		return HAL_ERROR;

	  if(HAL_CAN_Start(&hcan) != HAL_OK) 									// Start CAN
	  		return HAL_ERROR;

	  return HAL_OK;
}


