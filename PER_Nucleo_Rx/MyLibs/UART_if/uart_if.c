#include "usart.h"
#include "gpio.h"

#include "stdio.h"
#include "string.h"

#include "uart_if.h"

/* Terminal tx message buffer */
static uint8_t toSendBuf[MSG_BUF_SIZE];

/* Rx message buffer */
static uint8_t rxChar;
static uint8_t rxDataBuffer[REC_BUF_SIZE];

void UART_IF_SendMessageToTerminal(UART_HandleTypeDef *huart, char *str, ...)
{
	int bufSize;

	bufSize = sprintf((char*)toSendBuf, str);
	HAL_UART_Transmit(huart, toSendBuf, bufSize, HAL_MAX_DELAY);
}

int UART_IF_ReadIntFromTerminal(UART_HandleTypeDef *huart)
{
	uint8_t i = 0;
	int readValue = -1000;

	HAL_UART_Receive(huart, &rxChar, 1, HAL_MAX_DELAY);

	while(rxChar != ENTER_ASCII_CODE)
	{
		rxDataBuffer[i] = rxChar;
		i++;
		HAL_UART_Receive(huart, &rxChar, 1, HAL_MAX_DELAY);
	}
	rxDataBuffer[i] = '\0';
	sscanf((char*)rxDataBuffer, "%d", &readValue);

	return readValue;
}

int UART_IF_ReadTillValueValid(UART_HandleTypeDef *huart, int a, int b)
{
	int readValue = 0;

	while(1)
	{
		readValue = UART_IF_ReadIntFromTerminal(huart);

		if(readValue < a || readValue > b)
		{
			UART_IF_SendMessageToTerminal(huart, "Wrong value! Try again...\n\r");
		}
		else
		{
			return readValue;
		}
	}
}

void UART_IF_WaitTillEnterPress(UART_HandleTypeDef *huart)
{
	uint8_t ch;

	HAL_UART_Receive(huart, &ch, 1, HAL_MAX_DELAY);

		while(rxChar != ENTER_ASCII_CODE) {}
}

void UART_IF_PrintBinary(UART_HandleTypeDef *huart, int decNum)
{
	char bin[17] = "0000000000000000"; 	// string to hold bin number
	uint8_t len = 16;					// string length
	uint8_t i = 0;

	do
	{
		if(decNum % 2 == 0)
		{
			bin[len - i - 1] = '0';
		}
		else
		{
			bin[len - i - 1] = '1';
		}

		decNum /= 2;
		i++;

	} while(decNum != 0);

	bin[len] = '\0';

	UART_IF_SendMessageToTerminal(huart, "%s", bin);
}
