# PER system for Nucleo-WL55JC2 boards

## General info
This project is a simple payload error rate system and its aim is to allow testing receiver radio sensitivity. Project runs on two Nucleo-WL55JC2 boards - Tx and Rx device. Tx device randomly generates payload which is sent to Rx device via radio with LoRa modulation. Rx device after receiving the payload sends it back to Tx device by using UART interface. Tx device calculates PER and prints results to the terminal. 

## Setup 

```
                                         V    V
             --------------------------  |    |  --------------------------
             |        Tx device       |  |    |  |        Rx device       |
             |                        |--|    |--|                        |
   ComPort<--|                        |          |                        |
             |                        |<-------->|                        |
             --------------------------   UART   -------------------------- 
```

## User interface
Project has implemented simple user interface by which devices can be configured. User is able to configure Spreading Factor, Bandwidth, Tx output power, Tx period, set tx mode and choose payload length.

<img src="/images/BER_System.png" width=600 height=300>

## Connections
| Tx device | Rx device |
| :-------- | :-------- |
|     PC13  | NRST      |
| USART1_TX | USART1_RX |
| USART1_RX | USART1_TX |

## How to use it?
- make devices connections
- rebuild all files and flash projects to the devices
- open terminal for Tx device (UART Config = 115200, 8b, 1 stopbit, no parity, no flow control)
- reset Tx device
- configure device by user interface
