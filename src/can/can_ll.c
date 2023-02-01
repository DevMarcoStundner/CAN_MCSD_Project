#include "can_ll.h"

#include "libll/stm32l4xx_ll_bus.h"
#include "libll/stm32l4xx_ll_gpio.h"

void can_ll_Init() {
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);

  LL_GPIO_InitTypeDef s_gpioinit;
  LL_GPIO_StructInit(&s_gpioinit);
  s_gpioinit.Pin = LL_GPIO_PIN_11 | LL_GPIO_PIN_12;
  s_gpioinit.Mode = LL_GPIO_MODE_ALTERNATE;
  s_gpioinit.Speed = LL_GPIO_SPEED_HIGH;
  s_gpioinit.Alternate = LL_GPIO_AF_9; // CAN
  LL_GPIO_Init(GPIOA, &s_gpioinit);

  // enable clock
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_CAN1);

  // enable init mode
  CAN1->MCR |= CAN_MCR_INRQ;
  while(!(CAN1->MSR & CAN_MSR_INAK));
  CAN1->FMR |= CAN_FMR_FINIT;
  
  // config can parameters
  //hcan1.Init.Prescaler = 16;
  //hcan1.Init.SyncJumpWidth = CAN_SJW_2TQ;
  //hcan1.Init.TimeSeg1 = CAN_BS1_8TQ;
  //hcan1.Init.TimeSeg2 = CAN_BS2_8TQ;
  CAN1->BTR = (16U & CAN_BTR_BRP_Msk) | ((8U-1U)<<CAN_BTR_TS1_Pos) | ((8U-1U)<<CAN_BTR_TS2_Pos) | ((2U-1U)<<CAN_BTR_SJW_Pos);

  //hcan1.Init.Mode = CAN_MODE_NORMAL;
  // enable silent loopback in case of no can shield
  CAN1->BTR |= CAN_BTR_LBKM | CAN_BTR_SILM;
  //CAN1->BTR |= CAN_BTR_LBKM;

  // defaults
  //hcan1.Init.TimeTriggeredMode = DISABLE;

  //hcan1.Init.AutoBusOff = ENABLE;
  CAN1->MCR |= CAN_MCR_ABOM;

  // defaults, maybe enable?
  CAN1->MCR |= CAN_MCR_AWUM;
  //hcan1.Init.AutoWakeUp = DISABLE;

  // maybe dont do this
  //hcan1.Init.AutoRetransmission = DISABLE;

  // defaults
  //hcan1.Init.ReceiveFifoLocked = DISABLE;
  //hcan1.Init.TransmitFifoPriority = DISABLE;

	//HAL_CAN_ConfigFilter(&hcan1, &canfilter);
  CAN1->FA1R = 1U;
  CAN1->sFilterRegister[0].FR1 = 0;
  CAN1->sFilterRegister[0].FR2 = 0;
  // enable filter
  CAN1->FMR &= ~(CAN_FMR_FINIT);

  // enable nvic lines
  NVIC_EnableIRQ(CAN1_TX_IRQn);
  NVIC_SetPriority(CAN1_TX_IRQn, 0x01);
}

/* @brief Enable the can interface, must be called after can_ll_Init
 * @return 0 when successful, >0 otherwise
 */
can_ll_error_t can_ll_Start() {
  if (CAN1->MCR & CAN_MSR_INAK) {
    // enable normal mode
    CAN1->MCR &= ~(CAN_MCR_INRQ);
    while(CAN1->MSR & CAN_MSR_INAK);
    CAN1->MCR &= ~(CAN_MCR_SLEEP);
    return 0;
  } else {
    return 1;
  }
}

void can_ll_EnableIT_TXEMPTY() {
  CAN1->IER |= CAN_IER_TMEIE;
}

void can_ll_DisableIT_TXEMPTY() {
  CAN1->IER &= ~(CAN_IER_TMEIE);
}

bool can_ll_IsFlagSet_TxRqComplete(can_ll_txmbx_t mailbox) {
  bool value = false;
  switch (mailbox) {
    case CAN_LL_TXMAILBOX0:
      value = CAN1->TSR & CAN_TSR_RQCP0;
      break;
    case CAN_LL_TXMAILBOX1:
      value = CAN1->TSR & CAN_TSR_RQCP1;
      break;
    case CAN_LL_TXMAILBOX2:
      value = CAN1->TSR & CAN_TSR_RQCP2;
      break;
  }
  return value;
}

void can_ll_ClearFlag_TxRqComplete(can_ll_txmbx_t mailbox) {
  switch (mailbox) {
    case CAN_LL_TXMAILBOX0:
      CAN1->TSR |= (CAN_TSR_RQCP0);
      break;
    case CAN_LL_TXMAILBOX1:
      CAN1->TSR |= (CAN_TSR_RQCP1);
      break;
    case CAN_LL_TXMAILBOX2:
      CAN1->TSR |= (CAN_TSR_RQCP2);
      break;
  }
}

can_ll_error_t can_ll_AddTxMessage(can_ll_msgheader_t *header, uint8_t *data, can_ll_txmbx_t *mailboxid) {
  uint8_t tmpid = (CAN1->TSR & CAN_TSR_CODE_Msk)>>CAN_TSR_CODE_Pos;
  CAN_TxMailBox_TypeDef * mailbox = CAN1->sTxMailBox + tmpid;
  if (mailbox->TIR & CAN_TI0R_TXRQ) {
    // tx mailbox is not empty -> overrun
    return CAN_LL_ERROR;
  }
  *mailboxid = tmpid;

  mailbox->TDTR = header->DLC & CAN_TDT0R_DLC_Msk;
  mailbox->TIR = (header->StdId << CAN_TI0R_STID_Pos) & CAN_TI0R_STID_Msk;

  mailbox->TDHR = ((uint32_t)data[7] << CAN_TDH0R_DATA7_Pos) |
                  ((uint32_t)data[6] << CAN_TDH0R_DATA6_Pos) |
                  ((uint32_t)data[5] << CAN_TDH0R_DATA5_Pos) |
                  ((uint32_t)data[4] << CAN_TDH0R_DATA4_Pos);
  mailbox->TDLR = ((uint32_t)data[3] << CAN_TDL0R_DATA3_Pos) |
                  ((uint32_t)data[2] << CAN_TDL0R_DATA2_Pos) |
                  ((uint32_t)data[1] << CAN_TDL0R_DATA1_Pos) |
                  ((uint32_t)data[0] << CAN_TDL0R_DATA0_Pos);

  mailbox->TIR |= CAN_TI0R_TXRQ;
  return CAN_LL_OK;
}

can_ll_error_t can_ll_GetRxMessage(can_ll_rxfifo_t fifo, can_ll_msgheader_t *header, uint8_t *data) {
  CAN_FIFOMailBox_TypeDef * mailbox;
  uint32_t * rfr;
  if (fifo == CAN_LL_RXFIFO0) {
    mailbox = CAN1->sFIFOMailBox + 0;
    rfr = &CAN1->RF0R;
  } else {
    mailbox = CAN1->sFIFOMailBox + 1;
    rfr = &CAN1->RF1R;
  }
  if (*rfr & CAN_RF0R_FMP0_Msk) {
    header->StdId = (mailbox->RIR & CAN_RI0R_STID_Msk) >> CAN_RI0R_STID_Pos;
    header->DLC = (mailbox->RDTR & CAN_RDT0R_DLC_Msk) >> CAN_RDT0R_DLC_Pos;
    data[0] = (uint8_t)((CAN_RDL0R_DATA0 & mailbox->RDLR) >> CAN_RDL0R_DATA0_Pos);
    data[1] = (uint8_t)((CAN_RDL0R_DATA1 & mailbox->RDLR) >> CAN_RDL0R_DATA1_Pos);
    data[2] = (uint8_t)((CAN_RDL0R_DATA2 & mailbox->RDLR) >> CAN_RDL0R_DATA2_Pos);
    data[3] = (uint8_t)((CAN_RDL0R_DATA3 & mailbox->RDLR) >> CAN_RDL0R_DATA3_Pos);
    data[4] = (uint8_t)((CAN_RDH0R_DATA4 & mailbox->RDHR) >> CAN_RDH0R_DATA4_Pos);
    data[5] = (uint8_t)((CAN_RDH0R_DATA5 & mailbox->RDHR) >> CAN_RDH0R_DATA5_Pos);
    data[6] = (uint8_t)((CAN_RDH0R_DATA6 & mailbox->RDHR) >> CAN_RDH0R_DATA6_Pos);
    data[7] = (uint8_t)((CAN_RDH0R_DATA7 & mailbox->RDHR) >> CAN_RDH0R_DATA7_Pos);

    *rfr |= CAN_RF0R_RFOM0;
    return CAN_LL_OK;
  } else {
    return CAN_LL_ERROR;
  }
}

uint8_t can_ll_GetRxFifoLevel(can_ll_rxfifo_t fifo) {
  if (fifo == CAN_LL_RXFIFO0) {
    return CAN1->RF0R & CAN_RF0R_FMP0_Msk;
  } else {
    return CAN1->RF1R & CAN_RF1R_FMP1_Msk;
  }
}

uint8_t can_ll_GetTxFifoLevel() {
  uint8_t level = 0;
  if (CAN1->TSR & CAN_TSR_TME0) {
    level++;
  }
  if (CAN1->TSR & CAN_TSR_TME1) {
    level++;
  }
  if (CAN1->TSR & CAN_TSR_TME2) {
    level++;
  }
  return level;
}
