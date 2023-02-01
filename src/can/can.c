/*
 * can.c
 *
 *  Created on: Jan 11, 2023
 *      Author: marco
 */


#include "can.h"

#include "can_ll.h"

#include "stm32l432xx.h"
#include "libll/stm32l4xx_ll_bus.h"

#include "libll/stm32l4xx_ll_gpio.h"

#define REG_LEN 20

static volatile bool txmailboxfree[3] = {false};
static void (* volatile txcallbacks[3])(can_ll_txmbx_t mailbox) = {NULL};

typedef struct rxcallback_t rxcallback_t;
struct rxcallback_t {
	uint8_t id;
	void(*rxcb)(can_pkg_t *pkg);
};

static volatile struct rxcallback_t rxcallbacks[REG_LEN];

void CAN1_TX_IRQHandler() {
  if (can_ll_IsFlagSet_TxRqComplete(CAN_LL_TXMAILBOX0)) {
    can_ll_ClearFlag_TxRqComplete(CAN_LL_TXMAILBOX0);
    txmailboxfree[0] = true;
  }
  if (can_ll_IsFlagSet_TxRqComplete(CAN_LL_TXMAILBOX1)) {
    can_ll_ClearFlag_TxRqComplete(CAN_LL_TXMAILBOX1);
    txmailboxfree[1] = true;
  }
  if (can_ll_IsFlagSet_TxRqComplete(CAN_LL_TXMAILBOX2)) {
    can_ll_ClearFlag_TxRqComplete(CAN_LL_TXMAILBOX2);
    txmailboxfree[2] = true;
  }
}


/*
 * brief Function can_init() will init CAN
 */
void can_init() {
  can_ll_Init();
  can_ll_Start();

  can_ll_EnableIT_TXEMPTY();
}


/** brief Function can_handle() will do all periodic tasks that CAN needs
 *
 */
void can_handle(uint32_t time __attribute__((unused))) {
	for(int i = 0; i<3; i++) {
		if(txmailboxfree[i] == true) {
			txmailboxfree[i] = false;
			if(txcallbacks[i] != NULL) {
				//txcallbacks[i](i);
				txcallbacks[i] = NULL;
			}
		}
	}
  can_ll_EnableIT_TXEMPTY();
  while(can_ll_GetRxFifoLevel(CAN_LL_RXFIFO0) > 0) {
    _can_receive_pkg();
  }


}

/** brief Function _can_send_pkg() will send the data and checks if the mailbox is full
 *  param data is the data-array
 *  param len is the length of the pkg
 *  param callback is the func pointer to the callback that should be called
 *  returns -1 on error and mailbox id on success
 */
int can_send_pkg(uint32_t pkgid, uint8_t *data, uint8_t len, void (*callback)(can_ll_txmbx_t mailbox)) {
  static can_ll_msgheader_t	cantx;
	can_ll_txmbx_t mailboxid;
	cantx.StdId = pkgid;
	cantx.DLC = len;
	if (can_ll_AddTxMessage(&cantx, data, &mailboxid) != CAN_LL_OK) {
		return -1;
	}
	txcallbacks[mailboxid] = callback;
	return mailboxid;
}

/**	brief Function can_register_id will call the callback with pkg
 *  param id is the identifier for the pkg must be <0
 *  param callback is the func pointer to the callback that should be called
 *  returns 0 if okay retuns 1 if no id is assigned or wrong id is given
 */
int can_register_id(uint32_t id,  void (*callback)(can_pkg_t *pkg)) {
	if(id == 0) {
		return 1;
	}

	for(int i = 0; i<REG_LEN; i++) {
		if(rxcallbacks[i].id == 0) {
			rxcallbacks[i].id = id;
			rxcallbacks[i].rxcb = callback;
			return 0;
		}
	}

	return 1;

}

/** brief Function _can_receive_pkg() will receive the data and calls the associated callback to the id
 *
 */
void _can_receive_pkg() {
  static can_ll_msgheader_t	canrx;
	struct can_pkg_t pkg = {0};

	//HAL_CAN_GetRxMessage(&hcan1, CAN_FILTER_FIFO0, &canrx, pkg->data);
	can_ll_GetRxMessage(CAN_LL_RXFIFO0, &canrx, pkg.data);

	pkg.id = canrx.StdId;
	pkg.len = canrx.DLC;
  for (int i=0; i<REG_LEN; i++) {
    if (rxcallbacks[i].id == pkg.id) {
      rxcallbacks[i].rxcb(&pkg);
      break;
    }
  }
}

/** brief Function can_get_free_tx() will return the mailboxes that are ready for transmit
 * returns number of free mailboxes
 */
uint32_t can_get_free_tx() {
	//uint32_t fill = HAL_CAN_GetTxMailboxesFreeLevel(&hcan1);
	uint32_t fill = 3-can_ll_GetTxFifoLevel();
	return fill;
}

/** brief Function can_get_errors() will return public error state
 * 	returns the error that occurs
 */
int can_get_errors() {
	int errval = CAN_ERROR_NONE;

	//if(hcan1.ErrorCode == CAN_OK){return CAN_ERROR_NONE;}
	//if(hcan1.ErrorCode == CAN_EPV_ERROR){errval |= CAN_ERROR_EPV;} //ERROR Passive
	//if(hcan1.ErrorCode == CAN_BOF_ERROR){errval |= CAN_ERROR_BOF;} //ERROR Bus-off
	//if(hcan1.ErrorCode == CAN_STF_ERROR){errval |= CAN_ERROR_STF;} //ERROR Stuff
	//if(hcan1.ErrorCode == CAN_FOR_ERROR){errval |= CAN_ERROR_FOR;} //ERROR Form
  if (CAN1->ESR & CAN_ESR_EPVF) {errval |= CAN_ERROR_EPV;}
  if (CAN1->ESR & CAN_ESR_BOFF) {errval |= CAN_ERROR_BOF;}
  if (CAN1->ESR & CAN_ESR_LEC_Msk) {errval |= CAN1->ESR>>CAN_ESR_LEC_Pos;}

	return errval;

}
