/*
 * can.c
 *
 *  Created on: Jan 11, 2023
 *      Author: marco
 */
#include "main.h"


CAN_Status _can_init(CAN_HandleTypeDef hcan, CAN_FilterTypeDef CAN_Filter, CAN_TxHeaderTypeDef CanTx)
{

	CanTx.StdId 				= 0x00;
	CanTx.ExtId 				= 0x00;
	CanTx.IDE 					= CAN_ID_STD;
	CanTx.RTR 					= CAN_RTR_DATA;
	CanTx.DLC 					= 8;
	CanTx.TransmitGlobalTime	= DISABLE;


	CAN_Filter.FilterMode 					= CAN_FILTERMODE_IDMASK;
	CAN_Filter.FilterFIFOAssignment 		= CAN_FILTER_FIFO0;
	CAN_Filter.FilterBank 					= 0;
	CAN_Filter.FilterScale 					= CAN_FILTERSCALE_32BIT;
	CAN_Filter.FilterIdHigh 				= 0x000;
	CAN_Filter.FilterIdLow 					= 0x000;
	CAN_Filter.FilterMaskIdHigh				= 0x000;
	CAN_Filter.FilterMaskIdLow 				= 0x000;
	CAN_Filter.SlaveStartFilterBank 		= 14;
	CAN_Filter.FilterActivation 			= ENABLE;

	if(HAL_CAN_ConfigFilter(&hcan, &CAN_Filter) != HAL_OK)
	    return CAN_FILTER_ERROR;

	if(HAL_CAN_Start(&hcan) != HAL_OK)
	    return CAN_START_ERROR;

	if(HAL_CAN_ActivateNotification(&hcan,CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) // Activate CAN_IT_RX_FIFO0_MSG_PENDING Interrupt
		return CAN_ACTIVATE_IT_ERROR;

	return CAN_OK;
}

CAN_Status _can_send_pkg(CAN_HandleTypeDef *hcan, const CAN_TxHeaderTypeDef *pHeader, const uint8_t aData[], uint32_t *pTxMailbox)
{

	if(HAL_CAN_GetTxMailboxesFreeLevel(&hcan) > 3)
		return CAN_MAILBOX_ERROR;

	if(HAL_CAN_AddTxMessage(&hcan, &pHeader, aData, &pTxMailbox) != HAL_OK)
		return CAN_MSG_ERROR;

	return CAN_OK;
}

CAN_Status _can_receive_pkg(CAN_HandleTypeDef *hcan, uint32_t RxFifo, CAN_RxHeaderTypeDef *pHeader, uint8_t aData[], ID id)
{
	if(HAL_CAN_GetRxMessage(&hcan, RxFifo, &pHeader, aData) != HAL_OK)
		return CAN_MSG_ERROR;
	/*
	 * Hier die ID ermitteln und vergeben und callback aufrufen aber da nochmal genau fragen
	 */
	return CAN_OK;
}

/**
  * brief  Motor-Pkg callback.
  * return None
  */
__weak void CAN_Motor_Callback()
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the Callback could be implemented in the
            user file
   */
}
/**
  * brief  Encoder-Pkg callback.
  * return None
  */
__weak void CAN_Encoder_Callback()
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the Callback could be implemented in the
            user file
   */
}

