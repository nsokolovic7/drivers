/*
 * stm32f401xx_spi_driver.h
 *
 *  Created on: Jun 24, 2025
 *      Author: Nikola Soklović
 */

#ifndef INC_STM32F401XX_SPI_DRIVER_H_
#define INC_STM32F401XX_SPI_DRIVER_H_

#include "stm32f401xx.h"


//We define the SPI configuration structure
typedef struct{
	uint8_t SPI_DeviceMode;
	uint8_t SPI_BusConfig;
	uint8_t SPI_SclkSpeed;
	uint8_t SPI_DFF;
	uint8_t SPI_CPOL;
	uint8_t SPI_CPHA;
	uint8_t SPI_SSM;

}SPI_Config_t;


//We define the handle structure for SPI
typedef struct{
	SPI_RegDef_t *pSPIx;
	SPI_Config_t SPIConfig;

	uint8_t *pTxBuffer;		//to store the app. TX buffer address
	uint8_t *pRxBuffer; 	//to store the app. RX buffer address
	uint32_t TxLen;			//to store Tx len
	uint32_t RxLen;			//to store Rx len
	uint8_t  TxState;		//to store Tx state
	uint8_t  RxState;		//to store Rx state

}SPI_Handle_t;


//@SPI_DeviceMode
#define SPI_DEVICE_MODE_MASTER				1
#define SPI_DEVICE_MODE_SLAVE				0

//@SPI_BusConfig
#define SPI_BUS_CONFIG_FD					1
#define SPI_BUS_CONFIG_HD					2
#define SPI_BUS_CONFIG_SIMPLEX_TXONLY		3
#define SPI_BUS_CONFIG_SIMPLEX_RXONLY		4

//@SPI_SclkSpeed
#define SPI_SCLK_SPEED_DIV2					0
#define SPI_SCLK_SPEED_DIV4					1
#define SPI_SCLK_SPEED_DIV8					2
#define SPI_SCLK_SPEED_DIV16				3
#define SPI_SCLK_SPEED_DIV32				4
#define SPI_SCLK_SPEED_DIV64				5
#define SPI_SCLK_SPEED_DIV128				6
#define SPI_SCLK_SPEED_DIV256				7

//@SPI_DFF
#define SPI_DFF_8BITS						0
#define SPI_DFF_16BITS						1

//@SPI_CPOL
#define SPI_CPOL_HIGH						1
#define SPI_CPOL_LOW						0

//@SPI_CPHA
#define SPI_CPHA_HIGH						1
#define SPI_CPHA_LOW						0

//@SPI_SSM
#define SPI_SSM_EN							1
#define SPI_SSM_DI							0

#define SPI_TXE_FLAG						(1 << SPI_SR_TXE)
#define SPI_RXNE_FLAG						(1 << SPI_SR_RXNE)
#define SPI_BUSY_FLAG						(1 << SPI_SR_BUSY)

//Possible SPI application states
#define SPI_READY							0
#define SPI_BUSY_IN_RX						1
#define SPI_BUSY_IN_TX						2

//Possible SPI application events
#define SPI_EVENT_TX_CMPLT					1
#define SPI_EVENT_RX_CMPLT					2
#define SPI_EVENT_OVR_ERR					3
#define SPI_EVENT_CRC_ERR					4

/*
 * 				We define the APIs supported by this driver
 * */

//Peripheral clock setup
void SPI_PeriClockControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi);

//Init and De-Init of GPIO
void SPI_Init(SPI_Handle_t *pSPIHandle);
void SPI_DeInit(SPI_RegDef_t *pSPIx);
uint8_t SPIGetFlagStatus(SPI_RegDef_t *pSPIx, uint32_t FlagSet);

//Data send and receive
void SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t Len);
void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t Len);

//IRQ configuration and ISR handling
void SPI_IRQITConfig(uint8_t IRQNumber, uint8_t EnorDi);
void SPI_IRQPriority_Config(uint8_t IRQNumber, uint32_t IRQPriority);
void SPI_IRQHandling(SPI_Handle_t *pSPIHandle);

//Other peripheral control APIs
void SPI_PeripheralControl(SPI_RegDef_t *pSPIx, uint8_t EnOrDi);
void SPI_SSIConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi);

uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pTxBuffer, uint32_t Len);
uint8_t SPI_ReceiveDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pRxBuffer, uint32_t Len);

void SPI_ClearOVRFlag(SPI_RegDef_t *pSPIx);
void SPI_CloseTransmission(SPI_Handle_t *pSPIHandle);
void SPI_CloseReception(SPI_Handle_t *pSPIHandle);

void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle, uint8_t AppEv);

#endif /* INC_STM32F401XX_SPI_DRIVER_H_ */
