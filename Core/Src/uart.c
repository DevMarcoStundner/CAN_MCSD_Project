/*
 * uart.c
 *
 *  Created on: Nov 25, 2022
 *      Author: marco
 */
#include "main.h"


void _uart_transmit(UART_HandleTypeDef huart, uint8_t *pData, int data_len, int timeout)
{
	if(HAL_UART_Transmit(&huart, pData, data_len, timeout) != HAL_OK)
		Error_Handler();
}

void _uart_receive(UART_HandleTypeDef huart, uint8_t *pData, int data_len, int timeout)
{
	HAL_UART_Receive(&huart, pData, data_len, timeout);
}

int _frame_checker(uint8_t *pData, int data_len)
{

	if(pData[2] != ',')
		return 2;

	for(int i = 3; i <= data_len; i++)
	{
		if(pData[i] == '\n')
			return 0;
	}
	return 3;
}

int _uart_error_handler(UART_HandleTypeDef huart, int frame_check_value, uint8_t *pValue, int transmit_timeout)
{
	uint8_t error[] = {'E','R','R','O','R','_',' ','\n'};
	uint8_t ack_n[] = {'N','A','C','K','\n'};
	int ERROR_CMD = 52; //4
	int ERROR_EoF = 51; //3
	int ERROR_SEP = 50; //2
	int error_state = 2;
	switch(frame_check_value)
		  {
			  case 0:
				  error_state = 0;
				  break;

			  case 2:
				  pValue = &ack_n[0];
				  _uart_transmit(huart, pValue, sizeof(ack_n), transmit_timeout);
				  error[6] = ERROR_SEP;
				  pValue = &error[0];
				  _uart_transmit(huart, pValue, sizeof(error), transmit_timeout);
				  error_state = 1;
				  break;

			  case 3:

				  pValue = &ack_n[0];
				  _uart_transmit(huart, pValue, sizeof(ack_n), transmit_timeout);
				  error[6] = ERROR_EoF;
				  pValue = &error[0];
				  _uart_transmit(huart, pValue, sizeof(error), transmit_timeout);
				  error_state = 1;
				  if (HAL_UART_Init(&huart) != HAL_OK)
				  {
					 Error_Handler();
				  }
				  break;

			  case 4:
				  pValue = &ack_n[0];
				  _uart_transmit(huart, pValue, sizeof(ack_n), transmit_timeout);
				  error[6] = ERROR_CMD;
				  pValue = &error[0];
				  _uart_transmit(huart, pValue, sizeof(error), transmit_timeout);
				  error_state = 1;
				  break;

			  default:
				  Error_Handler();
		  }
	return error_state;
}
