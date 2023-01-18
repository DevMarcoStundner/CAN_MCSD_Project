/*
 * can.c
 *
 *  Created on: Jan 11, 2023
 *      Author: marco
 */
#include "main.h"
#include "can.h"


CAN_Status _can_init(CAN_HandleTypeDef hcan, CAN_FilterTypeDef CAN_Filter)
{

	if(HAL_CAN_ConfigFilter(&hcan, &CAN_Filter) != HAL_OK)
	    return CAN_FILTER_ERROR;

	if(HAL_CAN_Start(&hcan) != HAL_OK)
	    return CAN_START_ERROR;

	if(HAL_CAN_ActivateNotification(&hcan,CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) // Activate CAN_IT_RX_FIFO0_MSG_PENDING Interrupt
		return CAN_ACTIVATE_IT_ERROR;

	return CAN_OK;
}

CAN_Status _can_send_pkg(CAN_HandleTypeDef hcan, const CAN_TxHeaderTypeDef pHeader, const uint8_t aData[], uint32_t pTxMailbox)
{

	if(_can_mailbox_fill(hcan) == 0)
		return CAN_MAILBOX_ERROR;

	if(HAL_CAN_AddTxMessage(&hcan, &pHeader, aData, &pTxMailbox) != HAL_OK)
		return CAN_MSG_ERROR;

	return CAN_OK;
}

CAN_Status _can_receive_pkg(CAN_HandleTypeDef hcan, uint32_t RxFifo, CAN_RxHeaderTypeDef pHeader, uint8_t aData[], ID id)
{
	if(HAL_CAN_GetRxMessage(&hcan, RxFifo, &pHeader, aData) != HAL_OK)
		return CAN_MSG_ERROR;
	/*
	 * Hier die ID ermitteln und vergeben und callback aufrufen aber da nochmal genau fragen
	 */
	return CAN_OK;
}

uint32_t _can_mailbox_fill(CAN_HandleTypeDef hcan)
{

	uint32_t fill = HAL_CAN_GetTxMailboxesFreeLevel(&hcan);
	return fill;
}


CAN_Status _can_error_check(const CAN_HandleTypeDef *hcan)
{

	switch(hcan->ErrorCode)
		  {
			case CAN_OK:
					return CAN_OK;
					break;
			case CAN_EPV_ERROR: 						//ERROR Passive
					return CAN_EPV_ERROR;
					break;
			case CAN_BOF_ERROR:							//ERROR Bus-off
					return CAN_BOF_ERROR;
					break;
			case CAN_STF_ERROR:							//ERROR Stuff
					return CAN_STF_ERROR;
					break;
			case CAN_FOR_ERROR:							//ERROR Form
					return CAN_FOR_ERROR;
					break;
			default:
				return CAN_ERROR;						// another ERROR occured that is not implemented
				break;
		  }
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

