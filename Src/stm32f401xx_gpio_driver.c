/*
 * stm32f401xx_gpio_driver.c
 *
 *  Created on: Jun 18, 2025
 *      Author: Nikola Sokolović
 */

#include "stm32f401xx_gpio_driver.h"


/*************************************************************
 * @Function:			GPIO_PeriClockControl
 *
 * @Description:		This function enables or disables the clock for the GPIO port
 *
 * @Parameter[in]		Address to the GPIO peripheral
 * @Parameter[in]		ENABLE or DISABLE macros
 * @Parameter[in]
 *
 * @Return:				None
 *
 * @Note:				None
 *
 */
//Peripheral clock setup
void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx, uint8_t EnorDi)
{
	if (EnorDi == ENABLE)
	{
		if (pGPIOx == DRV_GPIOA)
		{
			DRV_GPIOA_PCLK_EN();
		}
		else if (pGPIOx == DRV_GPIOB)
		{
			DRV_GPIOB_PCLK_EN();
		}
		else if (pGPIOx == DRV_GPIOC)
		{
			DRV_GPIOC_PCLK_EN();
		}
		else if (pGPIOx == DRV_GPIOD)
		{
			DRV_GPIOD_PCLK_EN();
		}
		else if (pGPIOx == DRV_GPIOE)
		{
			DRV_GPIOE_PCLK_EN();
		}
		else
		{
			DRV_GPIOH_PCLK_EN();
		}
	}
	else{
		if (pGPIOx == DRV_GPIOA)
		{
			DRV_GPIOA_PCLK_DI();
		}
		else if (pGPIOx == DRV_GPIOB)
		{
			DRV_GPIOB_PCLK_DI();
		}
		else if (pGPIOx == DRV_GPIOC)
		{
			DRV_GPIOC_PCLK_DI();
		}
		else if (pGPIOx == DRV_GPIOD)
		{
			DRV_GPIOD_PCLK_DI();
		}
		else if (pGPIOx == DRV_GPIOE)
		{
			DRV_GPIOE_PCLK_DI();
		}
		else
		{
			DRV_GPIOH_PCLK_DI();
		}
	}

	return;
}


//Init and De-Init of GPIO
/*************************************************************
 * @Function:			GPIO_Init
 *
 * @Description:		This function initializes a GPIO pin
 *
 * @Parameter[in]		Address to the GPIO peripheral
 * @Parameter[in]
 * @Parameter[in]
 *
 * @Return:				None
 *
 * @Note:				None
 *
 */
void GPIO_Init(GPIO_Handle_t *pGPIOHandle)
{

	uint32_t temp = 0;

	 GPIO_PeriClockControl(pGPIOHandle->pGPIOx, ENABLE);

	//configuring the mode of GPIO
	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode <= GPIO_MODE_ANALOG)
	{
		//non interrupt mode
		temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinMode << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
		pGPIOHandle->pGPIOx->MODER &= ~(0x3 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber); //clearing
		pGPIOHandle->pGPIOx->MODER |= temp;	//setting
	}
	else
	{
		//interrupt mode
		if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_FT)
		{
			//configure the falling edge interrupt/ FTSR
			DRV_EXTI->FTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

			//clear corresponding RTSR bit
			DRV_EXTI->RTSR &= ~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
		}
		else if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RT)
		{
			//configure the RTSR register
			DRV_EXTI->RTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

			//clear corresponding FTSR bit
			DRV_EXTI->FTSR &= ~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
		}
		else if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RFT)
		{
			//configure both FTSR and RTSR registers

			DRV_EXTI->FTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
			DRV_EXTI->RTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
		}

		//configure the port selection in SYSCFG_EXTICR
		uint8_t temp1 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber / 4;
		uint8_t temp2 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber % 4;
		uint8_t portcode = GPIO_BASEADDR_TO_CODE(pGPIOHandle->pGPIOx);
		DRV_SYSCFG_PCLK_EN();
		DRV_SYSCFG->EXTICR[temp1] |= (portcode << (temp2 *4)); //temp2 * 4


		//enable the exti interrupt delivery
		DRV_EXTI->IMR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	}

	//configuring the speed
	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinSpeed  << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
	pGPIOHandle->pGPIOx->OSPEEDR &= ~(0x3 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber); //clearing
	pGPIOHandle->pGPIOx->OSPEEDR |= temp;

	//configure pull up/down settings
	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinPuPdControl  << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber));
	pGPIOHandle->pGPIOx->PUPDR &= ~(0x3 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber); //clearing
	pGPIOHandle->pGPIOx->PUPDR |= temp;

	//configuring the optype
	temp = (pGPIOHandle->GPIO_PinConfig.GPIO_PinOPType  <<  pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
	pGPIOHandle->pGPIOx->OTYPER &= ~(0x1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber); //clearing
	pGPIOHandle->pGPIOx->OTYPER |= temp;

	//configuring the alternate functionality mode
	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_ALTFN)
	{
		//configure the alt function reg
		uint8_t temp1, temp2; //we use temp1 and temp2 to identify the location of the register we need

		temp1 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber/8; //if 1 it is the upper 8, if 0 the it is in the lower 8
		temp2 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber%8;	//
		pGPIOHandle->pGPIOx->AFR[temp1] &= ~(0xF << (4 * temp2));
		pGPIOHandle->pGPIOx->AFR[temp1] |= (pGPIOHandle->GPIO_PinConfig.GPIO_PinAltFunMode << (4 * temp2));
	}


	return;
}

/*************************************************************
 * @Function:			GPIO_DeInit
 *
 * @Description:		This function initializes a GPIO pin
 *
 * @Parameter[in]		Address to the GPIO port
 * @Parameter[in]
 * @Parameter[in]
 *
 * @Return:				None
 *
 * @Note:				None
 *
 */
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx)
{
	if (pGPIOx == DRV_GPIOA)
	{
		DRV_GPIOA_REG_RST();
	}
	else if (pGPIOx == DRV_GPIOB)
	{
		DRV_GPIOB_REG_RST();
	}
	else if (pGPIOx == DRV_GPIOC)
	{
		DRV_GPIOC_REG_RST();
	}
	else if (pGPIOx == DRV_GPIOD)
	{
		DRV_GPIOD_REG_RST();
	}
	else if (pGPIOx == DRV_GPIOE)
	{
		DRV_GPIOE_REG_RST();
	}
	else
	{
		DRV_GPIOH_REG_RST();
	}
}

//Data read and write
/*************************************************************
 * @Function:			GPIO_ReadFromInputPin
 *
 * @Description:		This function reads from input pin
 *
 * @Parameter[in]		Address to the GPIO port
 * @Parameter[in]		Pin number
 * @Parameter[in]
 *
 * @Return:				Read value
 *
 * @Note:				None
 *
 */
uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber)
{
	uint8_t value;
	value = (uint8_t)((pGPIOx->IDR >> PinNumber) & 0x00000001);
	return value;
}

/*************************************************************
 * @Function:			GPIO_ReadFromInputPort
 *
 * @Description:		This function reads from input port
 *
 * @Parameter[in]		Address to the GPIO port
 * @Parameter[in]
 * @Parameter[in]
 *
 * @Return:				Read value
 *
 * @Note:				None
 *
 */
uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx)
{
	uint16_t value;
	value = (uint16_t)(pGPIOx->IDR);
	return value;
}

/*************************************************************
 * @Function:			GPIO_WriteToOutputPin
 *
 * @Description:		This function writes to the output pin
 *
 * @Parameter[in]		Address to the GPIO port
 * @Parameter[in]		Number of the used pin
 * @Parameter[in]		Value that should be written
 *
 * @Return:				None
 *
 * @Note:				None
 *
 */
void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber, uint8_t Value)
{
	if(Value == GPIO_PIN_SET)
	{
		//we write 1 to the output data register corresponding to the PinNumber
		pGPIOx->ODR |= (1 << PinNumber);
	}
	else
	{
		//write 0
		pGPIOx->ODR &= ~(1 << PinNumber);
	}
}

/*************************************************************
 * @Function:			GPIO_WriteToOutputPort
 *
 * @Description:		This function writes to the output port
 *
 * @Parameter[in]		Address to the GPIO port
 * @Parameter[in]		Value that should be written
 * @Parameter[in]
 *
 * @Return:				None
 *
 * @Note:				None
 *
 */
void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx, uint16_t Value)
{
	pGPIOx->ODR = Value;
}

/*************************************************************
 * @Function:			GPIO_ToggleOutputPin
 *
 * @Description:		This function toggles the output port
 *
 * @Parameter[in]		Address to the GPIO port
 * @Parameter[in]		Pin number
 * @Parameter[in]
 *
 * @Return:				None
 *
 * @Note:				None
 *
 */
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber)
{
	pGPIOx->ODR ^= (1 << PinNumber);
}


//IRQ configuration and ISR handling
void GPIO_IRQITConfig(uint8_t IRQNumber, uint8_t EnorDi)
{
	if (EnorDi == ENABLE)
	{
		if(IRQNumber <= 31)
		{
			//programm the ISER0 reg
			*DRV_NVIC_ISER0 |= ( 1 << IRQNumber);
		}
		else if (IRQNumber > 31 && IRQNumber < 64)
		{
			//programm the ISER1 reg
			*DRV_NVIC_ISER1 |= ( 1 << (IRQNumber % 32));
		}
		else if (IRQNumber > 64 && IRQNumber < 96)
		{
			//programm the ISER2  reg
			*DRV_NVIC_ISER2 |= ( 1 << (IRQNumber % 64));
		}

	}
	else
	{
		if(IRQNumber <= 31)
				{
					//programm the ICER0 reg
					*DRV_NVIC_ICER0 |= ( 1 << IRQNumber);
				}
				else if (IRQNumber > 31 && IRQNumber < 64)
				{
					//programm the ICER1 reg
					*DRV_NVIC_ICER1 |= ( 1 << (IRQNumber % 32));
				}
				else if (IRQNumber > 64 && IRQNumber < 96)
				{
					//programm the ICER2  reg
					*DRV_NVIC_ICER2 |= ( 1 << (IRQNumber % 64));
				}
	}
}

void GPIO_IRQPriority_Config(uint8_t IRQNumber, uint32_t IRQPriority)
{
	uint8_t iprx = IRQNumber / 4;
	uint8_t iprx_section = IRQNumber % 4;

	uint32_t	shift_amount = (8 * iprx_section) + (8 - NO_PR_BITS_IMPLEMENTED);
	*(DRV_NVIC_IPR_BASE_ADDR + iprx) |= (IRQPriority << shift_amount);
}

void GPIO_IRQHandling(uint8_t PinNumber)
{
	if(DRV_EXTI->PR & (1 << PinNumber)){
		//clear
		DRV_EXTI->PR |= (1 << PinNumber);
	}
}
