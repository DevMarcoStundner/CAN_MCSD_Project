/*
 * can.c
 *
 *  Created on: Jan 11, 2023
 *      Author: marco
 */


#include "main.h"
#include "can.h"

#define pkg_len 8

static CAN_HandleTypeDef   	hcan1;
static CAN_FilterTypeDef    CAN_Filter;
static CAN_TxHeaderTypeDef	CanTx;
static CAN_RxHeaderTypeDef 	CanRx;

	CanTx.StdId 				= 0x00;
	CanTx.ExtId 				= 0x00;
	CanTx.IDE 				= CAN_ID_STD;
	CanTx.RTR 				= CAN_RTR_DATA;
	CanTx.DLC 				= 8;
	CanTx.TransmitGlobalTime		= DISABLE;

	CAN_Filter.FilterMode 			= CAN_FILTERMODE_IDMASK;
	CAN_Filter.FilterFIFOAssignment 	= CAN_FILTER_FIFO0;
	CAN_Filter.FilterBank 			= 0;
	CAN_Filter.FilterScale 			= CAN_FILTERSCALE_32BIT;
	CAN_Filter.FilterIdHigh 		= 0x000;
	CAN_Filter.FilterIdLow 			= 0x000;
	CAN_Filter.FilterMaskIdHigh		= 0x000;
	CAN_Filter.FilterMaskIdLow 		= 0x000;
	CAN_Filter.SlaveStartFilterBank 	= 14;
	CAN_Filter.FilterActivation 		= ENABLE;



/** brief Function _can_init() will config the filter and inits CAN
 */
void can_init()
{
	HAL_CAN_ConfigFilter(&hcan1, &CAN_Filter);
	HAL_CAN_Start(&hcan1);
	HAL_CAN_ActivateNotification(&hcan1,CAN_IT_RX_FIFO0_MSG_PENDING);
}


/** brief Function can_handle() will do all periodic tasks that CAN needs
 *
 */
void can_handle()
{

}

/** brief Function _can_send_pkg() will send the data and checks if the mailbox is full
 *  param data is the data-array
 *  param len is the length of the pkg
 *  param callback is the func pointer to the callback that should be called
 *  returns 0 if no error occurs and none zero if an error occurs
 *///CAN_HandleTypeDef hcan, const CAN_TxHeaderTypeDef pHeader,
int can_send_pkg(uint8_t *Data, uint8_t len, void (*callback)(uint32_t Mailbox))
{
	uint32_t TxMailbox;
	uint8_t aData[8];
	Data = &aData[0];
	HAL_CAN_AddTxMessage(&hcan1, &CanTx, aData, &TxMailbox);
	if((HAL_CAN_IsTxMessagePending(&hcan1, TxMailbox)) == 1)
	{
		callback(TxMailbox);
		return 0;
	}
	return 1;

}

/**	brief Function can_register_id will call the callback with pkg
 *  param id is the identifier for the pkg
 *  param callback is the func pointer to the callback that should be called
 *  returns 0 if okay retuns 1 if no id is assigned
 */
int can_register_id(uint32_t id,  void (*callback)(can_pkg_t *pkg))
{
	if(id <= reg_len && id >= 0)
	{
		callback(&can_pkg_reg[id]);
		return 0;
	}
	return 1;
}


void _can_receive_pkg(uint8_t *Data)
{
	uint8_t aData[8];
	int i = 0;
	while(can_pkg_reg->full[i] == true)
	{
		i++;
	}
	can_pkg_reg->data[i] = &aData[0];
	HAL_CAN_GetRxMessage(&hcan1, CAN_FILTER_FIFO0, &CanRx, aData);
	can_pkg_reg->id[i] = i;

}

/** brief Function can_get_free_tx() will return the mailboxes that are ready for transmit
 * returns number of free mailboxes
 */
uint32_t can_get_free_tx()
{
	uint32_t fill = HAL_CAN_GetTxMailboxesFreeLevel(&hcan1);
	return fill;
}

/** brief Function can_get_errors() will return public error state
 * 	returns the error that occurs
 */
can_error_t can_get_errors()
{
	int CAN_ERROR_VALUE =0;

	if(hcan1.ErrorCode == CAN_OK){return CAN_ERROR_NONE;}
	if(hcan1.ErrorCode == CAN_EPV_ERROR){CAN_ERROR_VALUE |= CAN_ERROR_EPV;} //ERROR Passive
	if(hcan1.ErrorCode == CAN_BOF_ERROR){CAN_ERROR_VALUE |= CAN_ERROR_BOF;} //ERROR Bus-off
	if(hcan1.ErrorCode == CAN_STF_ERROR){CAN_ERROR_VALUE |= CAN_ERROR_STF;} //ERROR Stuff
	if(hcan1.ErrorCode == CAN_FOR_ERROR){CAN_ERROR_VALUE |= CAN_ERROR_FOR;} //ERROR Form

	return CAN_ERROR_VALUE;

}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)			// Packet receive interrupt
{
	HAL_CAN_DeactivateNotification(&hcan1,CAN_IT_RX_FIFO0_MSG_PENDING);
}

