/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    subghz_phy_app.c
  * @author  MCD Application Team
  * @brief   Application of the SubGHz_Phy Middleware
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "platform.h"
#include "sys_app.h"
#include "subghz_phy_app.h"
#include "radio.h"

/* USER CODE BEGIN Includes */
#include "stdlib.h"

#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

#include "uart_if.h"
#include "devices_conf.h"

/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* Radio events function pointer */
static RadioEvents_t RadioEvents;
/* USER CODE BEGIN PV */

/* Radio buffers */
static uint8_t BufferTx[MAX_APP_BUFFER_SIZE];
static uint8_t BufferRx[MAX_APP_BUFFER_SIZE];

/* Transmitted and received values */
static uint16_t txMessage = 0;
static uint16_t rxMessage = 1;

/* Rx device rssi */
static int16_t rxRssi = 0;

/* Error calculation variables */
static uint64_t errorCnt = 0;
static uint64_t prevErrorCnt = 0;
static uint64_t txMsgSent = 0;
static float errPercentage = 0.0;

/* Send message task */
osThreadId_t SendTxTask_Id;

const osThreadAttr_t SendTxTask_attributes = {
  .name = "SendTxTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 512*2
};
void SendTxTask(void *argument);

/* Send message task */
osThreadId_t BERCheckTask_Id;

const osThreadAttr_t BERCheckTask_attributes = {
  .name = "BERCheckTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 512*2
};
void BERCheckTask(void *argument);

/*!
 * @brief Function that sends packet through radio
 */
void SendTx(void);

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/*!
 * @brief Function to be executed on Radio Tx Done event
 */
static void OnTxDone(void);

/**
  * @brief Function to be executed on Radio Rx Done event
  * @param  payload ptr of buffer received
  * @param  size buffer size
  * @param  rssi
  * @param  LoraSnr_FskCfo
  */
static void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t LoraSnr_FskCfo);

/**
  * @brief Function executed on Radio Tx Timeout event
  */
static void OnTxTimeout(void);

/**
  * @brief Function executed on Radio Rx Timeout event
  */
static void OnRxTimeout(void);

/**
  * @brief Function executed on Radio Rx Error event
  */
static void OnRxError(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions ---------------------------------------------------------*/
void SubghzApp_Init(void)
{
  /* USER CODE BEGIN SubghzApp_Init_1 */

  /* USER CODE END SubghzApp_Init_1 */

  /* Radio initialization */
  RadioEvents.TxDone = OnTxDone;
  RadioEvents.RxDone = OnRxDone;
  RadioEvents.TxTimeout = OnTxTimeout;
  RadioEvents.RxTimeout = OnRxTimeout;
  RadioEvents.RxError = OnRxError;

  Radio.Init(&RadioEvents);

  /* USER CODE BEGIN SubghzApp_Init_2 */

  /* Radio Set frequency */
  Radio.SetChannel(RF_FREQUENCY);

  /* Set seed for random number generation */
  srand(Radio.Random());

  Radio.SetTxConfig(MODEM_LORA, DevConf_GetConfValue(TX_PWR), 0, DevConf_GetConfValue(BW),
                    DevConf_GetConfValue(SF), LORA_CODINGRATE,
                    LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                    true, 0, 0, LORA_IQ_INVERSION_ON, TX_TIMEOUT_VALUE);

  /* Not used since it is Tx device */
  Radio.SetRxConfig(MODEM_LORA, DevConf_GetConfValue(BW), DevConf_GetConfValue(SF),
                    LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                    LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                    0, true, 0, 0, LORA_IQ_INVERSION_ON, true);

  Radio.SetMaxPayloadLength(MODEM_LORA, MAX_APP_BUFFER_SIZE);

  /* FreeRTOS tasks initialization */
  SendTxTask_Id = osThreadNew(SendTxTask, NULL, &SendTxTask_attributes);
  if (SendTxTask_Id == NULL)
  {
    Error_Handler();
  }

  BERCheckTask_Id = osThreadNew(BERCheckTask, NULL, &BERCheckTask_attributes);
  if (BERCheckTask_Id == NULL)
  {
    Error_Handler();
  }

  osThreadFlagsSet(SendTxTask_Id, 1);

  /* USER CODE END SubghzApp_Init_2 */
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private functions ---------------------------------------------------------*/
static void OnTxDone(void)
{
  /* USER CODE BEGIN OnTxDone */

	/* Start listening for UART incoming messages */
	if(DevConf_GetTxMode() == TWO_BYTES_NUM)
	{
		HAL_UART_Receive_IT(&huart1, BufferRx, FIXED_PAYLOAD_LEN);
	}
	else
	{
		HAL_UART_Receive_IT(&huart1, BufferRx, DevConf_GetPayloadLen() + 2);
	}

	osThreadFlagsSet(SendTxTask_Id, 50);

  /* USER CODE END OnTxDone */
}

static void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t LoraSnr_FskCfo)
{
  /* USER CODE BEGIN OnRxDone */

  /* USER CODE END OnRxDone */
}

static void OnTxTimeout(void)
{
  /* USER CODE BEGIN OnTxTimeout */

	osThreadFlagsSet(SendTxTask_Id, 50);

  /* USER CODE END OnTxTimeout */
}

static void OnRxTimeout(void)
{
  /* USER CODE BEGIN OnRxTimeout */

  /* USER CODE END OnRxTimeout */
}

static void OnRxError(void)
{
  /* USER CODE BEGIN OnRxError */

  /* USER CODE END OnRxError */
}

/* USER CODE BEGIN PrFD */

void SendTxTask(void *argument)
{
	uint8_t firstCall = 1;

	for(;;)
	{
		if(firstCall == 1)
		{
			osThreadFlagsWait(1, osFlagsWaitAll, osWaitForever);
			firstCall = 0;
		}

		if(DevConf_GetTxMode() == TWO_BYTES_NUM)
		{
			rxMessage = 0;
		}
		else
		{
			uint8_t payloadLen = DevConf_GetPayloadLen();

			for(uint8_t i = 2; i < payloadLen + 2; i++)
			{
				BufferRx[i] = 0;
			}
		}

		SendTx();

		osThreadFlagsWait(50, osFlagsWaitAll, osWaitForever);

		osDelay(DevConf_GetTxPeriod());

		/* Print actual results */
		if(DevConf_GetTxMode() == TWO_BYTES_NUM)
		{
			UART_IF_SendMessageToTerminal(pcTxUart, "tx: ");
			UART_IF_PrintBinary(pcTxUart, txMessage);
			UART_IF_SendMessageToTerminal(pcTxUart, "\n\rrx: ");
			UART_IF_PrintBinary(pcTxUart, rxMessage);
		}
		else
		{
			if(prevErrorCnt == errorCnt)
			{
				UART_IF_SendMessageToTerminal(pcTxUart, "No packets lost");
			}
			else if(errorCnt > prevErrorCnt)
			{
				UART_IF_SendMessageToTerminal(pcTxUart, "%2.0d packets lost", errorCnt - prevErrorCnt);
			}
		}

		/* Error percentage calculation */
		errPercentage = ((float)errorCnt / txMsgSent) * 100.0;

		UART_IF_SendMessageToTerminal(pcTxUart, " | errPackets: %d", errorCnt);
		UART_IF_SendMessageToTerminal(pcTxUart, " | errRate: %5.2f%%", errPercentage);
		UART_IF_SendMessageToTerminal(pcTxUart, " | rssi: %d\n\n\r", rxRssi);
	}
}

void BERCheckTask(void *argument)
{
	for(;;)
	{
		osThreadFlagsWait(1, osFlagsWaitAll, osWaitForever);

		rxRssi = ((int16_t)BufferRx[1] << 8 & 0xFF00) | ((int16_t)BufferRx[0] & 0x00FF);

		if(DevConf_GetTxMode() == TWO_BYTES_NUM)
		{
			rxMessage = ((uint16_t)BufferRx[3] << 8 & 0xFF00) | ((uint16_t)BufferRx[2] & 0x00FF);

			if(rxMessage == txMessage)
			{
				errorCnt--;
			}
		}
		else
		{
			uint8_t payloadLen = DevConf_GetPayloadLen();

			for(uint8_t i = 2; i < payloadLen + 2; i++)
			{
				if(BufferTx[i] == BufferRx[i])
				{
					errorCnt--;
				}
			}
		}
	}
}

void SendTx(void)
{
	/* Info: two first bytes of tx buffer are reserved for rssi value from rx */
	prevErrorCnt = errorCnt;

	if(DevConf_GetTxMode() == TWO_BYTES_NUM)
	{
		txMessage = (uint16_t)rand();
		errorCnt++;

		BufferTx[2] = (uint8_t)(txMessage & 0xFF);
		BufferTx[3] = (uint8_t)(txMessage >> 8 & 0xFF);

		Radio.Send(BufferTx, FIXED_PAYLOAD_LEN);
		txMsgSent++;
	}
	else
	{
		uint8_t payloadLen = DevConf_GetPayloadLen();

		for(uint8_t i = 2; i < payloadLen + 2; i++)
		{
			BufferTx[i] = (uint8_t)rand();
			errorCnt++;
			txMsgSent++;
		}

		Radio.Send(BufferTx, payloadLen + 2);
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	osThreadFlagsSet(BERCheckTask_Id, 1);
}

/* USER CODE END PrFD */
