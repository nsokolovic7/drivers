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

static void I2C_ExecuteAddressPhase(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr)
{
	SlaveAddr = SlaveAddr << 1;
	SlaveAddr &= ~(1);
	pI2Cx->DR = SlaveAddr;
}

static void I2C_ClearADDRFlag(I2C_RegDef_t *pI2Cx)
{
  uint32_t dummyRead = pI2Cx->SR1;
  dummyRead = pI2Cx->SR2;
  (void)dummyRead;
}

static void I2C_GenerateStopCondition(I2C_RegDef_t *pI2Cx)
{
	pI2Cx->CR1 |= (1 << I2C_CR1_STOP);
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

	//we programm the devices own address
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
}
void I2C_DeInit(I2C_RegDef_t *pI2Cx);


void I2C_MasterSendData(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint8_t Len, uint8_t SlaveAddr)
{
	//generate the start condition
	I2C_GenerateStartCondition(pI2CHandle->pI2Cx);

	//Confirm the start generation is completed by checking the SB flag
	//in SR1
	while(!I2CGetFlagStatus(pI2CHandle->pI2Cx, I2C_SB_FLAG));

	//Send the address of the slave
	I2C_ExecuteAddressPhase(pI2CHandle->pI2Cx, SlaveAddr);

	//Confirm the address phase is completed by checking the ADDR flag in SR1
	while(!I2CGetFlagStatus(pI2CHandle->pI2Cx, I2C_ADDR_FLAG));

	//Clear the ADDR flag according to its software sequence
	I2C_ClearADDRFlag(pI2CHandle->pI2Cx);

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
	I2C_GenerateStopCondition(pI2CHandle->pI2Cx);
}


