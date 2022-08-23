#ifndef __UART_IF_H__
#define __UART_IF_H__

#define MSG_BUF_SIZE 		128
#define REC_BUF_SIZE 		16

#define ENTER_ASCII_CODE 	13

/* UART instances definition */
#define txRxUart &huart1
#define pcTxUart &huart2

/**
  * @brief  Sends message to terminal
  * @param  UART_HandleTypeDef - uart which you want to use
  * @retval None
  */
void UART_IF_SendMessageToTerminal(UART_HandleTypeDef *huart, char *str, ...);

/**
  * @brief  Reads input from PC
  * @param  UART_HandleTypeDef - uart which you want to use
  * @retval int - read value
  */
int UART_IF_ReadIntFromTerminal(UART_HandleTypeDef *huart);

/**
  * @brief  Reads input from PC
  * @param  UART_HandleTypeDef - uart which you want to use
  * @retval int - read value | a, b - valid range
  */
int UART_IF_ReadTillValueValid(UART_HandleTypeDef *huart, int a, int b);

/**
  * @brief  Waits for Enter button press
  * @param  UART_HandleTypeDef - uart which you want to use
  * @retval None
  */
void UART_IF_WaitTillEnterPress(UART_HandleTypeDef *huart);

/**
  * @brief  Converts from int and prints binary number
  * @param  UART_HandleTypeDef - uart which you want to use, int - number to convert
  * @retval None
  */
void UART_IF_PrintBinary(UART_HandleTypeDef *huart, int decNum);

#endif /* __UART_IF_H__ */
