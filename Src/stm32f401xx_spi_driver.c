/*
 * stm32f401xx_spi_driver.c
 *
 *  Created on: Jun 24, 2025
 *      Author: Nikola Sokolović
 */

#include "stm32f401xx_spi_driver.h"

//We implement a few private functions to handle certain interrupts
//we use the key woed static to declare these functions as private
static void spi_txe_interrupt_handle(SPI_Handle_t *pSPIHandle);
static void spi_rxne_interrupt_handle(SPI_Handle_t *pSPIHandle);
static void spi_ovr_interrupt_handle(SPI_Handle_t *pSPIHandle);

//Peripheral clock setup
void SPI_PeriClockControl(SPI_RegDef_t *pSPIx, uint8_t EnorDi)
{
	if (EnorDi == ENABLE)
		{
			if (pSPIx == DRV_SPI1)
			{
				DRV_SPI1_PCLK_EN();
			}
			else if (pSPIx == DRV_SPI2)
			{
				DRV_SPI2_PCLK_EN();
			}
			else if (pSPIx == DRV_SPI3)
			{
				DRV_SPI3_PCLK_EN();
			}
			else //pSPIx == DRV_SPI4
			{
				DRV_SPI4_PCLK_EN();
			}
		}
		else{
			if (pSPIx == DRV_SPI1)
			{
				DRV_SPI1_PCLK_DI();
			}
			else if (pSPIx == DRV_SPI2)
			{
				DRV_SPI2_PCLK_DI();
			}
			else if (pSPIx == DRV_SPI3)
			{
				DRV_SPI3_PCLK_DI();
			}
			else //pSPIx == DRV_SPI4
			{
				DRV_SPI4_PCLK_DI();
			}
		}

		return;
}

//Init and De-Init of GPIO
void SPI_Init(SPI_Handle_t *pSPIHandle)
{
	uint32_t tempreg = 0;

	SPI_PeriClockControl(pSPIHandle->pSPIx, ENABLE);

	//configure the device mode
	tempreg |= pSPIHandle->SPIConfig.SPI_DeviceMode << 2;

	//configure the bus config
	if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_FD)
	{
		//bidi mode should be cleared
		tempreg &= ~(1 << 15);
	}
	else if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_HD)
	{
		//bidi mode should be set
				tempreg |= (1 << 15);
	}
	else if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_SIMPLEX_RXONLY)
	{
		//bidi mode should be cleared
		tempreg &= ~(1 << 15);
		//RXONLY bit should be set
		tempreg |= (1 << 10);
	}

	//configure the spi serial clock speed
	tempreg |= pSPIHandle->SPIConfig.SPI_SclkSpeed << 3;

	//configure the DFF
	tempreg |= pSPIHandle->SPIConfig.SPI_DFF << 11;

	//configure the CPOL
	tempreg |= pSPIHandle->SPIConfig.SPI_CPOL << 1;

	//configure the CPHA
	tempreg |= pSPIHandle->SPIConfig.SPI_CPHA << 0;


	pSPIHandle->pSPIx->CR1 = tempreg;
}
void SPI_DeInit(SPI_RegDef_t *pSPIx); //To do

uint8_t SPIGetFlagStatus(SPI_RegDef_t *pSPIx, uint32_t FlagSet)
{
	if (pSPIx->SR & FlagSet) return FLAG_SET;
	return FLAG_RESET;
}

//Data send and receive
void SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t Len)
{
	while (Len > 0)
	{
		//We wait until TXE is set
		while(SPIGetFlagStatus(pSPIx, SPI_TXE_FLAG) == FLAG_RESET);

		//Check the CR1 bit
		if(pSPIx->CR1 & (1 << SPI_CR1_DFF))
		{
			//16 bit DFF
			//load the data into the DR
			pSPIx->DR = *((uint16_t*)pTxBuffer);
			Len -= 2;
			(uint16_t*)pTxBuffer++;
		}
		else
		{
			pSPIx->DR = *(pTxBuffer);
			Len--;
			pTxBuffer++;
		}


	}
}


void SPI_PeripheralControl(SPI_RegDef_t *pSPIx, uint8_t EnOrDi){
	if (EnOrDi == ENABLE)
	{
		pSPIx->CR1 |= (1 << SPI_CR1_SPE);
	}
	else{
		pSPIx->CR1 &= ~(1 << SPI_CR1_SPE);

	}
}

void SPI_SSIConfig(SPI_RegDef_t *pSPIx, uint8_t EnOrDi){
	if (EnOrDi == ENABLE)
	{
		pSPIx->CR1 |= (1 << SPI_CR1_SSI);
	}
	else{
		pSPIx->CR1 &= ~(1 << SPI_CR1_SSI);

	}
}

void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t Len){
	while (Len > 0)
		{
			//We wait until TXE is set
			while(SPIGetFlagStatus(pSPIx, SPI_RXNE_FLAG) == FLAG_RESET);

			//Check the CR1 bit
			if(pSPIx->CR1 & (1 << SPI_CR1_DFF))
			{
				//16 bit DFF
				//load the data into the DR
				*((uint16_t*)pRxBuffer) = pSPIx->DR;
				Len -= 2;
				(uint16_t*)pRxBuffer++;
			}
			else
			{
				*(pRxBuffer) = pSPIx->DR;
				Len--;
				pRxBuffer++;
			}


		}
}

//IRQ configuration and ISR handling
void SPI_IRQITConfig(uint8_t IRQNumber, uint8_t EnorDi);

void SPI_IRQPriority_Config(uint8_t IRQNumber, uint32_t IRQPriority);

void SPI_IRQHandling(SPI_Handle_t *pSPIHandle)
{
	uint8_t temp1, temp2;

	//check the TXE
	temp1 = pSPIHandle->pSPIx->SR & (1 << SPI_SR_TXE);
	temp2 = pSPIHandle->pSPIx->CR2 & (1 << SPI_CR2_TXEIE);

	if (temp1 && temp2){
		//handle TXE
		spi_txe_interrupt_handle(pSPIHandle);
	}

	//check the RXNE
	temp1 = pSPIHandle->pSPIx->SR & (1 << SPI_SR_RXNE);
	temp2 = pSPIHandle->pSPIx->CR2 & (1 << SPI_CR2_RXNEIE);

	if (temp1 && temp2){
		//handle RXNE
		spi_rxne_interrupt_handle(pSPIHandle);
	}

	//check for ovr flag
	temp1 = pSPIHandle->pSPIx->SR & (1 << SPI_SR_OVR);
	temp2 = pSPIHandle->pSPIx->CR2 & (1 << SPI_CR2_ERRIE);

	if (temp1 && temp2){
		//handle OVR
		spi_ovr_interrupt_handle(pSPIHandle);
	}
}

uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pTxBuffer, uint32_t Len)
{
	uint8_t state = pSPIHandle->TxState;

	if(state != SPI_BUSY_IN_TX)
	{
		//we save the Tx buffer addr and len info in some global variables
		pSPIHandle->pTxBuffer = pTxBuffer;
		pSPIHandle->TxLen = Len;

		//Mark the SPI state as busy in transmission so that
		//no other code can take over the SPI peripheral until transmission is over
		pSPIHandle->TxState = SPI_BUSY_IN_TX;

		//enable the TXEIE control bit to get interrupt whenever TXE flag is set in SR
		pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_TXEIE);
	}
	return state;
}

uint8_t SPI_ReceiveDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pRxBuffer, uint32_t Len)
{
	uint8_t state = pSPIHandle->RxState;

		if(state != SPI_BUSY_IN_RX)
		{
			//we save the Tx buffer addr and len info in some global variables
			pSPIHandle->pRxBuffer = pRxBuffer;
			pSPIHandle->RxLen = Len;

			//Mark the SPI state as busy in transmission so that
			//no other code can take over the SPI peripheral until transmission is over
			pSPIHandle->RxState = SPI_BUSY_IN_RX;

			//enable the TXEIE control bit to get interrupt whenever TXE flag is set in SR
			pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_RXNEIE);
		}
		return state;
}

static void spi_txe_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
	if(pSPIHandle->pSPIx->CR1 & (1 << SPI_CR1_DFF))
	{
		//16 bit DFF
		//load the data into the DR
		pSPIHandle->pSPIx->DR = *((uint16_t*)pSPIHandle->pTxBuffer);
		pSPIHandle->TxLen -= 2;
		(uint16_t*)pSPIHandle->pTxBuffer++;
	}
	else
	{
		pSPIHandle->pSPIx->DR = *(pSPIHandle->pTxBuffer);
		pSPIHandle->TxLen--;
		pSPIHandle->pTxBuffer++;
	}

	if (!pSPIHandle->TxLen){
		//TxLen is zero and we need to close the comm
		SPI_CloseTransmission(pSPIHandle);
		SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_TX_CMPLT);
	}
}
static void spi_rxne_interrupt_handle(SPI_Handle_t *pSPIHandle){
	if (pSPIHandle->pSPIx->CR1 & (1 << 11)){
		//16bit
		*((uint16_t*)pSPIHandle->pRxBuffer) = (uint16_t)pSPIHandle->pSPIx->DR;
		pSPIHandle->RxLen -= 2;
		pSPIHandle->pRxBuffer -= 2;
	}
	else{
		//8bit
		*(pSPIHandle->pRxBuffer) = (uint8_t) pSPIHandle->pSPIx->DR;
		pSPIHandle->RxLen--;
		pSPIHandle->pRxBuffer--;
	}

	if(!pSPIHandle->RxLen){
		//reception is complete
		//lets turn off the RXNEIE interrupt
		SPI_CloseReception(pSPIHandle);
		SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_RX_CMPLT);

	}
}
static void spi_ovr_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
	uint8_t temp;

	if (pSPIHandle->TxState != SPI_BUSY_IN_TX){

		temp = pSPIHandle->pSPIx->DR;
		temp = pSPIHandle->pSPIx->SR;
		(void)temp;
	}
	SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_OVR_ERR);
}


void SPI_ClearOVRFlag(SPI_RegDef_t *pSPIx){
	uint8_t temp;
	temp = pSPIx->DR;
	temp = pSPIx->SR;
	(void)temp;
}
void SPI_CloseTransmission(SPI_Handle_t *pSPIHandle)
{
	pSPIHandle->pSPIx->CR2 &= ~(1 << SPI_CR2_TXEIE);
	pSPIHandle->pTxBuffer = NULL;
	pSPIHandle->TxLen = 0;
	pSPIHandle->TxState = SPI_READY;
}
void SPI_CloseReception(SPI_Handle_t *pSPIHandle)
{
	pSPIHandle->pSPIx->CR2 &= ~(1 << SPI_CR2_RXNEIE);
	pSPIHandle->pRxBuffer = NULL;
	pSPIHandle->RxLen = 0;
	pSPIHandle->RxState = SPI_READY;
}


__weak void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle, uint8_t AppEv){
	//this is a weak implementation and it can be overriden by the app
}
