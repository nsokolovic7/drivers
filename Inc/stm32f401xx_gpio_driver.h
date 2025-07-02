/*
 * stm32f401xx_gpio_driver.h
 *
 *  Created on: Jun 18, 2025
 *      Author: Nikola Sokolović
 */

#ifndef INC_STM32F401XX_GPIO_DRIVER_H_
#define INC_STM32F401XX_GPIO_DRIVER_H_

#include "stm32f401xx.h"

typedef struct
{
	uint8_t GPIO_PinNumber;
	uint8_t GPIO_PinMode;			//possible values from @GPIO_possible_modes
	uint8_t GPIO_PinSpeed;			//possible values from @GPIO_possible_speeds
	uint8_t GPIO_PinPuPdControl;	//possible values from @GPIO_PUPD
	uint8_t GPIO_PinOPType;			//possible values from @GPIO_OUT_TYPES
	uint8_t GPIO_PinAltFunMode;

}GPIO_PinConfig_t;

//Handle structure for a GPIO pin
typedef struct
{
	//pointer to hold the base address of the GPIO
	GPIO_RegDef_t* pGPIOx;						//pointer that holds the base address of the GPIO port
	GPIO_PinConfig_t GPIO_PinConfig;			//holds the configuration settings of the GPIO pin

}GPIO_Handle_t;

//@GPIO_PIN_NUMBER
//GPIO possible pin numbers
#define GPIO_PIN_NO_0			0
#define GPIO_PIN_NO_1			1
#define GPIO_PIN_NO_2			2
#define GPIO_PIN_NO_3			3
#define GPIO_PIN_NO_4			4
#define GPIO_PIN_NO_5			5
#define GPIO_PIN_NO_6			6
#define GPIO_PIN_NO_7			7
#define GPIO_PIN_NO_8			8
#define GPIO_PIN_NO_9			9
#define GPIO_PIN_NO_10			10
#define GPIO_PIN_NO_11			11
#define GPIO_PIN_NO_12			12
#define GPIO_PIN_NO_13			13
#define GPIO_PIN_NO_14			14
#define GPIO_PIN_NO_15			15

//@GPIO_possible_modes
#define GPIO_MODE_IN			0
#define GPIO_MODE_OUT			1
#define GPIO_MODE_ALTFN			2
#define GPIO_MODE_ANALOG		3
#define GPIO_MODE_IT_FT			4	//input mode falling edge
#define GPIO_MODE_IT_RT			5	//input mode rising edge
#define GPIO_MODE_IT_RFT		6	//input mode rising edge falling edge trigger

//@GPIO_OUT_TYPES
//GPIO possible output types
#define GPIO_OP_TYPE_PP			0	//push pull
#define GPIO_OP_TYPE_OD			1	//open drain

//@GPIO_possible_speeds
#define GPIO_SPEED_LOW			0
#define GPIO_SPEED_MEDIUM		1
#define GPIO_SPEED_FAST			2
#define GPIO_SPEED_HIGH			3

//@GPIO_PUPD
//Pin pull up pull down config macros
#define GPIO_NO_PUPD			0
#define GPIO_PIN_PU				1	//pull up
#define GPIO_PIN_PD				2	//pull down
//configuration 3 is forbidden

/**********************************************************************************************/
/*									APIs supported by this driver  							  */
/**********************************************************************************************/

//Peripheral clock setup
void GPIO_PeriClockControl(GPIO_RegDef_t *pGPIOx, uint8_t EnorDi);

//Init and De-Init of GPIO
void GPIO_Init(GPIO_Handle_t *pGPIOHandle);
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx);

//Data read and write
uint8_t GPIO_ReadFromInputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber);
uint16_t GPIO_ReadFromInputPort(GPIO_RegDef_t *pGPIOx);
void GPIO_WriteToOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber, uint8_t Value);
void GPIO_WriteToOutputPort(GPIO_RegDef_t *pGPIOx, uint16_t Value);
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber);

//IRQ configuration and ISR handling
void GPIO_IRQITConfig(uint8_t IRQNumber, uint8_t EnorDi);
void GPIO_IRQPriority_Config(uint8_t IRQNumber, uint32_t IRQPriority);
void GPIO_IRQHandling(uint8_t PinNumber);



#endif /* INC_STM32F401XX_GPIO_DRIVER_H_ */
