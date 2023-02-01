#ifndef CAN_LL_H
#define CAN_LL_H

#include <stdint.h>
#include <stdbool.h>

typedef struct can_ll_msgheader_t can_ll_msgheader_t;
typedef struct can_ll_msgheader_t {
  uint32_t StdId;
  uint32_t IDE;
  uint32_t DLC;
};

typedef enum can_ll_rxfifo_t can_ll_rxfifo_t;
typedef enum can_ll_rxfifo_t {
  CAN_LL_RXFIFO0 = 0,
  CAN_LL_RXFIFO1 = 1
};

typedef enum can_ll_txmbx_t can_ll_txmbx_t;
typedef enum can_ll_txmbx_t {
  CAN_LL_TXMAILBOX0 = 0,
  CAN_LL_TXMAILBOX1 = 1,
  CAN_LL_TXMAILBOX2 = 2
};

typedef enum can_ll_error_t can_ll_error_t;
typedef enum can_ll_error_t {
  CAN_LL_OK = 0,
  CAN_LL_ERROR = 1
};

void can_ll_Init();
can_ll_error_t can_ll_Start();
void can_ll_EnableIT_TXEMPTY();
void can_ll_DisableIT_TXEMPTY();
bool can_ll_IsFlagSet_TxRqComplete(can_ll_txmbx_t mailbox);
void can_ll_ClearFlag_TxRqComplete(can_ll_txmbx_t mailbox);
can_ll_error_t can_ll_AddTxMessage(can_ll_msgheader_t *header, uint8_t *data, can_ll_txmbx_t *mailboxid);
can_ll_error_t can_ll_GetRxMessage(can_ll_rxfifo_t fifo, can_ll_msgheader_t *header, uint8_t *data);
uint8_t can_ll_GetRxFifoLevel(can_ll_rxfifo_t fifo);
uint8_t can_ll_GetTxFifoLevel();

#endif
