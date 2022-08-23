#ifndef __DEVICES_CONF_H__
#define __DEVICES_CONF_H__

#define CONFIG_BUFn	4

#define CONFIG_VALUES_MSG	0x40
#define RX_DEVICE_ACK		0x50
#define RESET_REQ			0x60

typedef enum
{
	MSG_ID = 0,
	SF,
	BW,
	TX_PWR
} DevConfValue_t;

typedef enum
{
	TWO_BYTES_NUM = 0,
	SET_LENGTH
} DevConfTxMode_t;

/**
  * @brief  Collects configuration values from Tx device
  * @retval None
  */
void DevConf_Tx_CollectConfValues(void);

/**
  * @brief  Sends configuration values to Rx device
  * @retval None
  */
void DevConf_Tx_SendConfValues(void);

/**
  * @brief  Receive config values from Rx device
  * @retval None
  */
void DevConf_Rx_ReceiveConfValues(void);

/**
  * @brief  Returns given in argument configuration value
  * @param	DevConfValue_t value to return
  * @retval int8_t
  */
int8_t DevConf_GetConfValue(DevConfValue_t value);

/**
  * @brief  Returns set tx period
  * @retval uint32_t - tx period in ms
  */
uint32_t DevConf_GetTxPeriod(void);

/**
  * @brief  Returns set tx mode
  * @retval DevConfTxMode_t - tx mode
  */
DevConfTxMode_t DevConf_GetTxMode(void);

/**
  * @brief  Returns payload length
  * @retval DevConfTxMode_t - tx mode
  */
uint8_t DevConf_GetPayloadLen(void);

#endif /* __DEVICES_CONF_H__ */
