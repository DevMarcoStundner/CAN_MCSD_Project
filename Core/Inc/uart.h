/*
 * uart.h
 *
 *  Created on: Nov 25, 2022
 *      Author: marco
 */

#ifndef INC_UART_H_
#define INC_UART_H_


#define TRANSMIT_TIMEOUT 100

/** brief Function _uart_transmit() will transmit data with uart
 *  param huart is the handle for the HAL
 *  param pData pointer to the dataarray
 *  param data_len length of the array
 *  param uart transmit timeout
 *  returns void
 */
void _uart_transmit(UART_HandleTypeDef huart, uint8_t *pData, int data_len, int timeout);

/** brief Function _uart_receive() will receive data with uart
 *  param huart is the handle for the HAL
 *  param pData pointer to the dataarray
 *  param data_len length of the array
 *  param uart transmit timeout
 *  returns void
 */
void _uart_receive(UART_HandleTypeDef huart, uint8_t *pData, int data_len, int timeout);

/** brief Function _frame_checker looks if the frame is correct formated
 *  param pData pointer to the dataarray
 *  returns 0 if frame is correct,
 *  1 if SoF is wrong,
 *  2 if SEP is wrong,
 *  3 if EoF is wrong
 *  +---+---+---+---+-[...]--+---+---+
    | S | C | S |                | E |
    | o | M | E |     DATA       | o |
    | F | D | P |                | F |
    +---+---+---+---+-[...]--+---+---+
      #   r   ,       0:100        \n
 */
int _frame_checker(uint8_t *pData, int data_len);

/** brief Function _uart_error_handler() will handle the different error codes
 *  param huart is the handle for the HAL
 *  param frame_check_value is the result from the _frame_checker function
 *  param pValue pointer to the framearray
 *  param transmit_timeout is the timout for the _uart_transmit function
 *  returns 1 if error occurs, 0 if not
 */
int _uart_error_handler(UART_HandleTypeDef huart, int frame_check_value, uint8_t *pValue, int transmit_timeout);


#endif /* INC_UART_H_ */
