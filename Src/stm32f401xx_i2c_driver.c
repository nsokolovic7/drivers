/*
 * stm32f401xx_i2c_driver.c
 *
 *  Created on: Jun 29, 2025
 *      Author: Nikola Sokolović
 */

#include "stm32f401xx_i2c_driver.h"


uint16_t AHB_PreScaler[9] = {2, 4, 8, 16, 32, 64, 128, 256, 512};
uint16_t APB_PreScaler[4] = {2, 4, 8, 16};




uint8_t I2CGetFlagStatus(I2C_RegDef_t *pI2C, uint32_t FlagSet)
{
	if (pI2C->SR1 & FlagSet) return FLAG_SET;
	return FLAG_RESET;
}

static void I2C_GenerateStartCondition(I2C_RegDef_t *pI2Cx)
{
	pI2Cx->CR1 |= (1 << I2C_CR1_START);
}

static void I2C_ExecuteAddressPhaseRead(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr)
{
	SlaveAddr = SlaveAddr << 1;
	SlaveAddr |= 1;
	pI2Cx->DR = SlaveAddr;
}

static void I2C_ExecuteAddressPhaseWrite(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr)
{
	SlaveAddr = SlaveAddr << 1;
	SlaveAddr &= ~(1);
	pI2Cx->DR = SlaveAddr;
}

static void I2C_ClearADDRFlag(I2C_Handle_t *pI2CHandle)
{
	uint32_t dummy_read;
	//check device mode
	if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_MSL))
	{
		//device is in master mode
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
		{
			if (pI2CHandle->RxSize == 1)
			{
				//first disable teh ACK
				I2C_ManageAcking(pI2CHandle->pI2Cx, DISABLE);

				//clear ADDR flag
				//read SR1 and SR2
				dummy_read = pI2CHandle->pI2Cx->SR1;
				dummy_read = pI2CHandle->pI2Cx->SR2;
				(void)dummy_read;
			}
		}
		else
			{

				//clear ADDR flag
				//read SR1 and SR2
				dummy_read = pI2CHandle->pI2Cx->SR1;
				dummy_read = pI2CHandle->pI2Cx->SR2;
				(void)dummy_read;

			}
	}
	else
	{
		//device is in slave mode
		//clear ADDR flag
		//read SR1 and SR2
		dummy_read = pI2CHandle->pI2Cx->SR1;
		dummy_read = pI2CHandle->pI2Cx->SR2;
		(void)dummy_read;

	}
}

void I2C_GenerateStopCondition(I2C_RegDef_t *pI2Cx)
{
	pI2Cx->CR1 |= (1 << I2C_CR1_STOP);
}

void I2C_ManageAcking(I2C_RegDef_t *pI2Cx, uint8_t EnOrDi)
{
	if(EnOrDi == I2C_ACK_ENABLE)
	{
		//enable the ACK
		pI2Cx->CR1 |= (1 << I2C_CR1_ACK);
	}
	else
	{
		//disable the ACK
		pI2Cx->CR1 &= ~(1 << I2C_CR1_ACK);
	}
}

void I2C_CloseRecieveData(I2C_Handle_t *pI2CHandle)
{
	//disable ITBUFEN control bit
	pI2CHandle->pI2Cx->CR2 &= ~(1 << I2C_CR2_ITBUFEN);

	//disable ITEVFEN control bit
	pI2CHandle->pI2Cx->CR2 &= ~(1 << I2C_CR2_ITEVTEN);

	pI2CHandle->TxRxState = I2C_READY;
	pI2CHandle->pRxBuffer = NULL;
	pI2CHandle->RxLen = 0;
	pI2CHandle->RxSize = 0;
	if (pI2CHandle->I2CConfig.I2C_ACKControl == I2C_ACK_ENABLE)
	{
		I2C_ManageAcking(pI2CHandle->pI2Cx, ENABLE);
	}
}

void I2C_CloseSendData(I2C_Handle_t *pI2CHandle)
{
	//disable ITBUFEN control bit
	pI2CHandle->pI2Cx->CR2 &= ~(1 << I2C_CR2_ITBUFEN);

	//disable ITEVFEN control bit
	pI2CHandle->pI2Cx->CR2 &= ~(1 << I2C_CR2_ITEVTEN);

	pI2CHandle->TxRxState = I2C_READY;
	pI2CHandle->pTxBuffer = NULL;
	pI2CHandle->TxLen = 0;
}


void I2C_PeripheralControl(I2C_RegDef_t *pI2Cx, uint8_t EnOrDi)
{
	if (EnOrDi == ENABLE)
	{
		pI2Cx->CR1 |= (1 << I2C_CR1_PE);
	}
	else{
		pI2Cx->CR1 &= ~(1 << I2C_CR1_PE);
	}
}


void I2C_PeriClockControl(I2C_RegDef_t *pI2Cx, uint8_t EnorDi)
{
	if (EnorDi == ENABLE)
	{
		if (pI2Cx == DRV_I2C1)
		{
			DRV_I2C1_PCLK_EN();
		}
		else if (pI2Cx == DRV_I2C2)
		{
			DRV_I2C2_PCLK_EN();
		}
		else
		{
			DRV_I2C3_PCLK_EN();
		}
	}
	else{
		if (pI2Cx == DRV_I2C1)
		{
			DRV_I2C1_PCLK_DI();
		}
		else if (pI2Cx == DRV_I2C2)
		{
			DRV_I2C2_PCLK_DI();
		}
		else
		{
			DRV_I2C3_PCLK_DI();
		}
	}

}

uint32_t RCC_GetPLLOutputClock()
{
  return 0;
}



uint32_t RCC_GetPCLK1Value(void)
{
	uint32_t pclk1, SystemClk, temp;
	uint8_t clksrc, ahbp, apb1p;

	clksrc =((DRV_RCC->CFGR >> 2) & 0x3);

	if(clksrc == 0)
	{
		SystemClk = 16000000;
	}
	else if(clksrc == 1)
	{
		SystemClk = 8000000;
	}
	else if(clksrc == 2)
	{
		SystemClk = RCC_GetPLLOutputClock();
	}

	//for ahb
	temp = ((DRV_RCC->CFGR >> 4) & 0xF);

	if(temp < 8)
	{
		ahbp = 1;
	}
	else
	{
	  ahbp = AHB_PreScaler[temp - 8];
	}

	//for apb1
	temp = ((DRV_RCC->CFGR >> 10) & 0x7);

	if(temp < 4)
	{
		apb1p = 1;
	}
	else
	{
		apb1p = APB_PreScaler[temp - 4];
	}

	pclk1 = (SystemClk - ahbp)/apb1p;

	return pclk1;
}

uint32_t RCC_GetPCLK2Value(void)
{
	uint32_t pclk2, SystemClk, temp;
	uint8_t clksrc, ahbp, apb2p;

	clksrc =((DRV_RCC->CFGR >> 2) & 0x3);

	if(clksrc == 0)
	{
		SystemClk = 16000000;
	}
	else if(clksrc == 1)
	{
		SystemClk = 8000000;
	}
	else if(clksrc == 2)
	{
		SystemClk = RCC_GetPLLOutputClock();
	}

	//for ahb
	temp = ((DRV_RCC->CFGR >> 4) & 0xF);

	if(temp < 8)
	{
		ahbp = 1;
	}
	else
	{
	  ahbp = AHB_PreScaler[temp - 8];
	}

	//for apb2
	temp = ((DRV_RCC->CFGR >> 13) & 0x7);

	if(temp < 4)
	{
		apb2p = 1;
	}
	else
	{
		apb2p = APB_PreScaler[temp - 4];
	}

	pclk2 = (SystemClk - ahbp)/apb2p;

	return pclk2;
}

//Init and De-Init of I2C
void I2C_Init(I2C_Handle_t *pI2CHandle)
{
	uint32_t tempreg = 0;

	tempreg |= pI2CHandle->I2CConfig.I2C_ACKControl << 10;
	pI2CHandle->pI2Cx->CR1 = tempreg;

	//configure the FREQ field of CR2
	tempreg |= 0;
	tempreg |= RCC_GetPCLK1Value() / 1000000U;
	pI2CHandle->pI2Cx->CR2 |= (tempreg & 0x3F);

	//we program the devices own address
	tempreg |= pI2CHandle->I2CConfig.I2C_DeviceAddress << 1;
	tempreg |= ( 1 << 14);
	pI2CHandle->pI2Cx->OAR1 |= tempreg;

	//CCR calculations
	uint16_t ccr_value = 0;
	tempreg = 0;
	if (pI2CHandle->I2CConfig.I2C_SCLSpeed <= I2C_SCL_SPEED_SM)
	{
		//standard mode
		ccr_value = RCC_GetPCLK1Value()/(2 * pI2CHandle->I2CConfig.I2C_SCLSpeed);
		tempreg |= (ccr_value & 0xFFF);
	}
	else
	{
		//fast mode
		tempreg |= (1 << 15);
		tempreg |= (pI2CHandle->I2CConfig.I2C_FMDutyCycle << 14);
		if (pI2CHandle->I2CConfig.I2C_FMDutyCycle == I2C_FM_DUTY_9)
		{
			ccr_value = RCC_GetPCLK1Value()/(3 * pI2CHandle->I2CConfig.I2C_SCLSpeed);
		}
		else
		{
			ccr_value = RCC_GetPCLK1Value()/(25 * pI2CHandle->I2CConfig.I2C_SCLSpeed);
		}
		ccr_value |= (ccr_value & 0xFFF);
	}
	pI2CHandle->pI2Cx->CCR = tempreg;

	//TRISE configuration
	if (pI2CHandle->I2CConfig.I2C_SCLSpeed <= I2C_SCL_SPEED_SM)
		{
			//standard mode

			tempreg = (RCC_GetPCLK1Value()/1000000U) + 1;
		}
		else
		{
			//fast mode
			tempreg = ((RCC_GetPCLK1Value()*300)/1000000000U) + 1;
		}
	pI2CHandle->pI2Cx->TRISE = (tempreg & 0x3F);

}
void I2C_DeInit(I2C_RegDef_t *pI2Cx);


void I2C_MasterSendData(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint8_t Len, uint8_t SlaveAddr, uint8_t Sr)
{
	//generate the start condition
	I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

	//Confirm the start generation is completed by checking the SB flag
	//in SR1
	while(!I2CGetFlagStatus(pI2CHandle->pI2Cx, I2C_SB_FLAG));

	//Send the address of the slave
	I2C_ExecuteAddressPhaseWrite(pI2CHandle->pI2Cx, SlaveAddr);

	//Confirm the address phase is completed by checking the ADDR flag in SR1
	while(!I2CGetFlagStatus(pI2CHandle->pI2Cx, I2C_ADDR_FLAG));

	//Clear the ADDR flag according to its software sequence
	I2C_ClearADDRFlag(pI2CHandle);

	//Send data until the Len = 0
	while(Len > 0)
	{
		while(!I2CGetFlagStatus(pI2CHandle->pI2Cx, I2C_TXE_FLAG)); //wait until TXE is set
		pI2CHandle->pI2Cx->DR = *pTxBuffer;
		pTxBuffer++;
		Len--;
	}

	//We wait until TXE = 1 and BTF = 1
	while(!I2CGetFlagStatus(pI2CHandle->pI2Cx, I2C_TXE_FLAG));

	while(!I2CGetFlagStatus(pI2CHandle->pI2Cx, I2C_BTF_FLAG));

	//Send the STOP condition
	if(Sr == I2C_ENABLE_SR)
	{
		I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
	}

}

void I2C_MasterRecieveData(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer, uint8_t Len, uint8_t SlaveAddr, uint8_t Sr)
{
	//Generate the start condition
	I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

	//confirm that start generation is completed by checking the SB flag in the SRI
	//NOTE: until SB is cleared, SCL will be stretched
	while(!I2CGetFlagStatus(pI2CHandle->pI2Cx, I2C_SB_FLAG));

	//send the address of the slave with r/nw bit set to R(1) (total 8 bits)
	I2C_ExecuteAddressPhaseRead(pI2CHandle->pI2Cx, SlaveAddr);

	//wait until address phase is completed by checking ADDR flag in SR1
	while(!I2CGetFlagStatus(pI2CHandle->pI2Cx, I2C_ADDR_FLAG));

	//procedure to read only 1 byte from slave
	if (Len == 1)
	{
		//Disable Acking
		I2C_ManageAcking(pI2CHandle->pI2Cx, I2C_ACK_DISABLE);


		//clear the ADDR flag
		I2C_ClearADDRFlag(pI2CHandle);

		//wait until RXNE becomes 1
		while(!I2CGetFlagStatus(pI2CHandle->pI2Cx, I2C_RXNE_FLAG)); //wait until RXNE is set

		//Generate STOP condition
		if(Sr == I2C_DISABLE_SR)
			{
				I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
			}



		//read data into buffer
		*pRxBuffer = pI2CHandle->pI2Cx->DR;
	}
	//procedure to read more than byte from slave
	if (Len > 1)
		{

			//clear the ADDR flag
			I2C_ClearADDRFlag(pI2CHandle);

			//Read the data until Len becomes zero
			for(uint32_t i = Len; i > 0; i++)
			{

				//wait until RXNE becomes 1
				while(!I2CGetFlagStatus(pI2CHandle->pI2Cx, I2C_RXNE_FLAG)); //wait until RXNE is set

				if (i == 2)
				{
					//clear the ACK bit
					I2C_ManageAcking(pI2CHandle->pI2Cx, I2C_ACK_DISABLE);

					//generate stop condition
					if(Sr == I2C_DISABLE_SR)
						{
							I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
						}

				}


				//read data into buffer
				*pRxBuffer = pI2CHandle->pI2Cx->DR;

				pRxBuffer++;

			}

		}

	if(pI2CHandle->I2CConfig.I2C_ACKControl == I2C_ACK_ENABLE)
	{
		I2C_ManageAcking(pI2CHandle->pI2Cx, I2C_ACK_DISABLE);

	}
}

uint8_t I2C_MasterSendDataIT(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint8_t Len, uint8_t SlaveAddr, uint8_t Sr)
{
	uint8_t busystate = pI2CHandle->TxRxState;

	if( (busystate != I2C_BUSY_IN_TX) && (busystate != I2C_BUSY_IN_RX))
	{
		pI2CHandle->pTxBuffer = pTxBuffer;
		pI2CHandle->TxLen = Len;
		pI2CHandle->TxRxState = I2C_BUSY_IN_TX;
		pI2CHandle->DevAddr = SlaveAddr;
		pI2CHandle->Sr = Sr;

		//Implement code to Generate START Condition
		I2C_GenerateStartCondition(pI2CHandle->pI2Cx);


		//Implement the code to enable ITBUFEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITBUFEN);

		//Implement the code to enable ITEVFEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITEVTEN);


		//Implement the code to enable ITERREN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITERREN);

	}

	return busystate;
}
uint8_t I2C_MasterRecieveDataIT(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer, uint8_t Len, uint8_t SlaveAddr, uint8_t Sr)
{
	uint8_t busystate = pI2CHandle->TxRxState;

	if( (busystate != I2C_BUSY_IN_TX) && (busystate != I2C_BUSY_IN_RX))
	{
		pI2CHandle->pRxBuffer = pRxBuffer;
		pI2CHandle->RxLen = Len;
		pI2CHandle->TxRxState = I2C_BUSY_IN_RX;
		pI2CHandle->RxSize = Len; //Rxsize is used in the ISR code to manage the data reception
		pI2CHandle->DevAddr = SlaveAddr;
		pI2CHandle->Sr = Sr;

		//Implement code to Generate START Condition
		I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

		//Implement the code to enable ITBUFEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITBUFEN);

		//Implement the code to enable ITEVFEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITEVTEN);


		//Implement the code to enable ITERREN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITERREN);

	}

	return busystate;
}

static void I2C_MasterHandleTXEInterrupt(I2C_Handle_t *pI2CHandle)
{
	if (pI2CHandle->TxLen > 0)
	{
		//we load the data into DR
		pI2CHandle->pI2Cx->DR = *(pI2CHandle->pTxBuffer);

		//decrement TxLen
		pI2CHandle->TxLen--;

		//increment buffer address
		pI2CHandle->pTxBuffer++;
	}
}

static void I2C_MasterHandleRXNEInterrupt(I2C_Handle_t *pI2CHandle)
{
	if (pI2CHandle->RxSize == 1)
	{
		*pI2CHandle->pRxBuffer = pI2CHandle->pI2Cx->DR;
		pI2CHandle->RxLen--;

	}

	if (pI2CHandle->RxSize > 1)
	{
		if(pI2CHandle->RxLen == 2)
		{
			//clear the ACK bit
			I2C_ManageAcking(pI2CHandle->pI2Cx, I2C_ACK_DISABLE);

		}
		*pI2CHandle->pRxBuffer = pI2CHandle->pI2Cx->DR;
		pI2CHandle->pRxBuffer++;
		pI2CHandle->RxLen--;
	}

	if (pI2CHandle->RxSize == 0)
	{
		//close the I2C data reception and notify the app
		//generate the stop condition
		if(pI2CHandle->Sr == I2C_DISABLE_SR)
		{
			I2C_GenerateStartCondition(pI2CHandle->pI2Cx);
		}

		//close i2c rx
		I2C_CloseRecieveData(pI2CHandle);

		//notify the application
		I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_RX_CMPLT);
	}
}

void I2C_SlaveSendData(I2C_RegDef_t *pI2Cx, uint8_t data)
{
	pI2Cx->DR = data;
}
uint8_t I2C_SlaveRecieveData(I2C_RegDef_t *pI2Cx)
{
	return (uint8_t)pI2Cx->DR;
}


void I2C_EV_IRQHandling(I2C_Handle_t *pI2CHandle)
{
  //Interrupt handling for slave and master mode of the device

	uint32_t temp1, temp2, temp3;

	temp1 = pI2CHandle->pI2Cx->CR2 & (1 << I2C_CR2_ITEVTEN);
	temp2 = pI2CHandle->pI2Cx->CR2 & (1 << I2C_CR2_ITBUFEN);
	temp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_SB);

	//Handle interrupt generated by SB event
	if (temp1 && temp3)
	{
		//SB flag is set
		//We execute teh address phase
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
		{
			I2C_ExecuteAddressPhaseRead(pI2CHandle->pI2Cx, pI2CHandle->DevAddr);
		}
		else if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX)
		{
			I2C_ExecuteAddressPhaseWrite(pI2CHandle->pI2Cx, pI2CHandle->DevAddr);
		}
	}

	//Handle interrupt generated by ADDR event
	temp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_ADDR);
	if (temp1 && temp3)
		{
			//ADDR flag is set
		I2C_ClearADDRFlag(pI2CHandle);
		}

	//Handle the BTF event
	temp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_BTF);
	if (temp1 && temp3)
		{
			//BTF flag is set
			if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX)
			{
				//make sure TXE is also set
				if(pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_TXE))
				{
					//BTF and TXE both set
					//close the transmision
					if(pI2CHandle->TxLen == 0)
					{

						if (pI2CHandle->Sr == I2C_DISABLE_SR)
						{
							I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
						}
						I2C_CloseSendData(pI2CHandle);

						I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_TX_CMPLT);
						}

				}
			else if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
			{
				//This block is empty
			}
		}

	//Handle interrupt for STOPF event
	temp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_STOPF);
	if (temp1 && temp3)
		{
			//STOPF flag is set
			//clear the STOPF flag
			//we clear by reading SR1 and writing to CR1
			//we have read SR1 before the if so no need to  do it again
			pI2CHandle->pI2Cx->CR1 |= 0x0000;

			//notifying the app that stop is detected
			I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_STOP);
		}

	//Handle interrupt for the TXE event
	temp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_TXE);
	if (temp1 && temp2 && temp3)
	{
		if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_MSL))
		{
			//master mode
			//TXE flag is set
			//we have to do data transmit
			if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX)
			{
				I2C_MasterHandleTXEInterrupt(pI2CHandle);

			}
		}
		else
		{
			//slave mode
			if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_TRA))
			{
				//device is in transmitter mode
				I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_DATA_REQ);
			}
		}
	}
}

	//Handle interrupt for the RXNE event
	temp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_RXNE);
	if (temp1 && temp2 && temp3)
	{
		//check device mode
		if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_MSL))
		{
			//device is master

			//RXNE flag is set
			if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
			{
				I2C_MasterHandleRXNEInterrupt(pI2CHandle);


			}
		}
		else
		{
			//slave mode
			if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_TRA))
			{
				//device is in transmitter mode
				I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_DATA_RCV);
			}
		}

	}
}


void I2C_ER_IRQHandling(I2C_Handle_t *pI2CHandle)
{
	uint32_t temp1, temp2;

	//read the ITERREN flag
	temp2 = (pI2CHandle->pI2Cx->CR2) & (1 << I2C_CR2_ITERREN);

	/***********************Check for Bus error************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1<< I2C_SR1_BERR);
	if(temp1  && temp2 )
	{
		//This is Bus error

		//Implement the code to clear the buss error flag
		pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_BERR);

		//Implement the code to notify the application about the error
	   I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_BERR);
	}

	/***********************Check for arbitration lost error************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_ARLO );
	if(temp1  && temp2)
	{
		//This is arbitration lost error

		//Implement the code to clear the arbitration lost error flag
		pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_ARLO);

		//Implement the code to notify the application about the error
		I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_ARLO);

	}

	/***********************Check for ACK failure  error************************************/

	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_AF);
	if(temp1  && temp2)
	{
		//This is ACK failure error

		//Implement the code to clear the ACK failure error flag
		pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_AF);

		//Implement the code to notify the application about the error
		I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_AF);
	}

	/***********************Check for Overrun/underrun error************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_OVR);
	if(temp1  && temp2)
	{
		//This is Overrun/underrun

		//Implement the code to clear the Overrun/underrun error flag
		pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_OVR);

		//Implement the code to notify the application about the error
		I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_OVR);
	}

	/***********************Check for Time out error************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_TIMEOUT);
	if(temp1  && temp2)
	{
		//This is Time out error

		//Implement the code to clear the Time out error flag
		pI2CHandle->pI2Cx->SR1 &= ~( 1 << I2C_SR1_TIMEOUT);

		//Implement the code to notify the application about the error
		I2C_ApplicationEventCallback(pI2CHandle,I2C_ERROR_TIMEOUT);
	}
}
