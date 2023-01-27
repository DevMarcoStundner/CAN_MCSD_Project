/*
 * can.c
 *
 *  Created on: Jan 11, 2023
 *      Author: marco
 */


#include "main.h"
#include "can.h"

static CAN_HandleTypeDef   	hcan1;
static CAN_FilterTypeDef    canfilter;
static CAN_TxHeaderTypeDef	cantx;
static CAN_RxHeaderTypeDef 	canrx;
static volatile int txmailboxfree = 4;
static void (* volatile txcallbacks[3])(uint32_t mailbox) = {NULL};
static void (* volatile rxcallbackarray[20])() = {NULL};

/*void CAN1_TX_IRQHandler()
{
	if(CAN->TSR &(1<<RQCP0))
	{
		txmailboxfree = 0;
	}
	if(CAN->TSR &(1<<RQCP1))
	{
		txmailboxfree = 1;
	}
	if(CAN->TSR &(1<<RQCP2))
	{
		txmailboxfree = 2;
	}

}

/** brief Function _can_init() will config the filter and inits CAN
 */
void can_init()
{
	cantx.StdId 					= 0x00;
	cantx.ExtId 					= 0x00;
	cantx.IDE 						= CAN_ID_STD;
	cantx.RTR 						= CAN_RTR_DATA;
	cantx.DLC 						= 8;
	cantx.TransmitGlobalTime		= DISABLE;

	canfilter.FilterMode 				= CAN_FILTERMODE_IDMASK;
	canfilter.FilterFIFOAssignment 		= CAN_FILTER_FIFO0;
	canfilter.FilterBank 				= 0;
	canfilter.FilterScale 				= CAN_FILTERSCALE_32BIT;
	canfilter.FilterIdHigh 				= 0x000;
	canfilter.FilterIdLow 				= 0x000;
	canfilter.FilterMaskIdHigh			= 0x000;
	canfilter.FilterMaskIdLow 			= 0x000;
	canfilter.SlaveStartFilterBank 		= 14;
	canfilter.FilterActivation 			= ENABLE;

	HAL_CAN_ConfigFilter(&hcan1, &canfilter);
	HAL_CAN_Start(&hcan1);
	//HAL_CAN_ActivateNotifaction(&hcan1,);
}


/** brief Function can_handle() will do all periodic tasks that CAN needs
 *
 */
void can_handle()
{
	for(int i = 0; i<=2; i++)
	{
		if(txmailboxfree == i)
		{
			if(txcallbacks[i]!= NULL)
			{
				txcallbacks[i](i);
				txcallbacks[i] = NULL;
			}
		}
	}
	if(HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0) != 0)
	{
		_can_receive_pkg();
	}

}

/** brief Function _can_send_pkg() will send the data and checks if the mailbox is full
 *  param data is the data-array
 *  param len is the length of the pkg
 *  param callback is the func pointer to the callback that should be called
 *  returns -1 on error and mailbox id on success
 *///CAN_HandleTypeDef hcan, const CAN_TxHeaderTypeDef pHeader,
int can_send_pkg(uint32_t pkgid, uint8_t *data, uint8_t len, void (*callback)(uint32_t mailbox))
{
	uint32_t mailboxid;
	cantx.StdId = pkgid;
	cantx.DLC = len;
	if (HAL_CAN_AddTxMessage(&hcan1, &cantx, data, &mailboxid) != HAL_OK)
	{
		return -1;
	}
	txcallbacks[mailboxid] = callback;
	return mailboxid;
}

/**	brief Function can_register_id will call the callback with pkg
 *  param id is the identifier for the pkg
 *  param callback is the func pointer to the callback that should be called
 *  returns 0 if okay retuns 1 if no id is assigned
 */
int can_register_id(uint32_t id,  void (*callback)(can_pkg_t *pkg))
{
	if(id<=20)
	{
		if(rxcallbackarray[id] == NULL)
		{
			rxcallbackarray[id] = callback;
			return 0;
		}
	}
	return 1;
}

/** brief Function _can_receive_pkg() will receive the data and calls the associated callback to the id
 *
 */
void _can_receive_pkg()
{
	struct can_pkg_t pkg = {0,0,0};


	HAL_CAN_GetRxMessage(&hcan1, CAN_FILTER_FIFO0, &canrx, pkg.data);

	pkg.id = canrx.StdId;
	pkg.len = canrx.DLC;
	rxcallbackarray[pkg.id](pkg);

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
int can_get_errors()
{
	int errval = CAN_ERROR_NONE;

	if(hcan1.ErrorCode == CAN_OK){return CAN_ERROR_NONE;}
	if(hcan1.ErrorCode == CAN_EPV_ERROR){errval |= CAN_ERROR_EPV;} //ERROR Passive
	if(hcan1.ErrorCode == CAN_BOF_ERROR){errval |= CAN_ERROR_BOF;} //ERROR Bus-off
	if(hcan1.ErrorCode == CAN_STF_ERROR){errval |= CAN_ERROR_STF;} //ERROR Stuff
	if(hcan1.ErrorCode == CAN_FOR_ERROR){errval |= CAN_ERROR_FOR;} //ERROR Form

	return errval;

}

void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan)
{
	txmailboxfree = 0;
}

void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan)
{
	txmailboxfree = 1;
}

void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan)
{
	txmailboxfree = 2;
}
