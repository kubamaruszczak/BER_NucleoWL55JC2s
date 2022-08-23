#include "usart.h"
#include "subghz_phy_app.h"

#include "uart_if.h"
#include "devices_conf.h"

#define TX_PERIOD_MIN 500
#define TX_PERIOD_MAX 10000

static int8_t confValues[CONFIG_BUFn] = {0x00, 7, 0, -10};
static uint32_t txPeriod = TX_PERIOD_MIN;
static DevConfTxMode_t txMode = TWO_BYTES_NUM;
static uint8_t payloadLength = 4;

/**
  * @brief  Send config values with message id and waits for the ack
  * @param	messageId - message id value | ack - 1 ack needed
  * @retval None
  */
static void DevConf_Tx_SendConfWithMsgId(uint8_t messageId, uint8_t ack);

void DevConf_Tx_CollectConfValues(void)
{
	UART_IF_SendMessageToTerminal(pcTxUart, "\n\n\r--- Tx device configuration ---\n\r");

	/* SF read */
	UART_IF_SendMessageToTerminal(pcTxUart, "ENTER SPREADING FACTOR [7 - 12]\n\r");
	confValues[SF] = UART_IF_ReadTillValueValid(pcTxUart, 7, 12);
	UART_IF_SendMessageToTerminal(pcTxUart, "Spreading factor set to: %d\n\r", confValues[SF]);

	/* BW read */
	UART_IF_SendMessageToTerminal(pcTxUart, "ENTER BANDWIDTH [0: 125kHz, 1: 250kHz, 2: 500kHz]\n\r");
	confValues[BW] = UART_IF_ReadTillValueValid(pcTxUart, 0, 2);
	UART_IF_SendMessageToTerminal(pcTxUart, "Bandwidth option: %d\n\r", confValues[BW]);

	/* TxPower read */
	UART_IF_SendMessageToTerminal(pcTxUart, "ENTER TX POWER [-16dBm - 22dBm]\n\r");
	confValues[TX_PWR] = UART_IF_ReadTillValueValid(pcTxUart, -16, 22);
	UART_IF_SendMessageToTerminal(pcTxUart, "Tx power set to: %ddBm\n\r", confValues[TX_PWR]);

	/* Tx period read */
	UART_IF_SendMessageToTerminal(pcTxUart, "ENTER TX PERIOD [%dms - %dms]\n\r",
								  TX_PERIOD_MIN, TX_PERIOD_MAX);
	txPeriod = UART_IF_ReadTillValueValid(pcTxUart, TX_PERIOD_MIN, TX_PERIOD_MAX);
	UART_IF_SendMessageToTerminal(pcTxUart, "Tx period set to: %dms\n\r", txPeriod);

	/* Tx period read */
	UART_IF_SendMessageToTerminal(pcTxUart, "CHOOSE TX MODE [0: TWO-BYTE NUMBER, 1: SET PAYLAOD LENGTH]\n\r");
	txMode = UART_IF_ReadTillValueValid(pcTxUart, TWO_BYTES_NUM, SET_LENGTH);
	UART_IF_SendMessageToTerminal(pcTxUart, "Chosen Tx mode: %d\n\r", txMode);

	if(txMode == SET_LENGTH)
	{
		/* Payload len read */
		UART_IF_SendMessageToTerminal(pcTxUart, "ENTER PAYLAOD LENGTH [%d - %d]\n\r",
									  MIN_APP_BUFFER_SIZE, MAX_APP_BUFFER_SIZE - 2);
		payloadLength = UART_IF_ReadTillValueValid(pcTxUart, MIN_APP_BUFFER_SIZE, MAX_APP_BUFFER_SIZE - 2);
		UART_IF_SendMessageToTerminal(pcTxUart, "Payload length set to: %d\n\r", payloadLength);
	}
}

void DevConf_Tx_SendConfValues(void)
{
	UART_IF_SendMessageToTerminal(pcTxUart, "\n\rPress ENTER to start Rx device configuration... ");
	UART_IF_WaitTillEnterPress(pcTxUart);
	UART_IF_SendMessageToTerminal(pcTxUart, "\n\rConfiguring Rx device... ");

	/* Sends configuration values */
	DevConf_Tx_SendConfWithMsgId(CONFIG_VALUES_MSG, 1);

	UART_IF_SendMessageToTerminal(pcTxUart, "Done!\n\r");
}

void DevConf_Rx_ReceiveConfValues(void)
{
	uint8_t txData = RX_DEVICE_ACK;
	HAL_StatusTypeDef status = HAL_ERROR;

	/* Wait till communication is initialized */
	while(confValues[MSG_ID] != CONFIG_VALUES_MSG)
	{
		status = HAL_UART_Receive(txRxUart, (uint8_t*)confValues, CONFIG_BUFn, 1000);
	}

	/* Send message that configuration went correct */
	if(status == HAL_OK)
	{
		HAL_UART_Transmit(txRxUart, &txData, 1, HAL_MAX_DELAY);
	}
}

static void DevConf_Tx_SendConfWithMsgId(uint8_t messageId, uint8_t ack)
{
	uint8_t rxData = 0x00;

	confValues[MSG_ID] = messageId;
	while(rxData != RX_DEVICE_ACK)
	{
		/* Send configuration values to Rx device via UART */
		HAL_UART_Transmit(txRxUart, (uint8_t*)confValues, CONFIG_BUFn, HAL_MAX_DELAY);

		if(ack == 1)
		{
			/* Wait for the ACK */
			HAL_UART_Receive(txRxUart, &rxData, 1, 1000);
		}
		else
		{
			break;
		}
	}
}

int8_t DevConf_GetConfValue(DevConfValue_t value)
{
	return confValues[value];
}

uint32_t DevConf_GetTxPeriod(void)
{
	return txPeriod;
}

DevConfTxMode_t DevConf_GetTxMode(void)
{
	return txMode;
}

uint8_t DevConf_GetPayloadLen(void)
{
	return payloadLength;
}
