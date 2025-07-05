/*
 * stm32f401xx.h
 *
 *  Created on: Jun 16, 2025
 *      Author: Nikola Sokolović
 */

#ifndef INC_STM32F401XX_H_
#define INC_STM32F401XX_H_

#include <stdint.h>
#include <stddef.h>

#define __vo volatile
#define __weak __attribute__((weak))

/******************************************************************************
 * 						Processor specific details
 ******************************************************************************/
#define DRV_NVIC_ISER0							((__vo uint32_t*)0xE000E100)
#define DRV_NVIC_ISER1							((__vo uint32_t*)0xE000E104)
#define DRV_NVIC_ISER2							((__vo uint32_t*)0xE000E108)
#define DRV_NVIC_ISER3							((__vo uint32_t*)0xE000E10C)
#define DRV_NVIC_ISER4							((__vo uint32_t*)0xE000E110)
#define DRV_NVIC_ISER5							((__vo uint32_t*)0xE000E114)
#define DRV_NVIC_ISER6							((__vo uint32_t*)0xE000E118)
#define DRV_NVIC_ISER7							((__vo uint32_t*)0xE000E11C)

#define DRV_NVIC_ICER0							((__vo uint32_t*)0XE000E180)
#define DRV_NVIC_ICER1							((__vo uint32_t*)0xE000E184)
#define DRV_NVIC_ICER2							((__vo uint32_t*)0xE000E188)
#define DRV_NVIC_ICER3							((__vo uint32_t*)0xE000E18C)
#define DRV_NVIC_ICER4							((__vo uint32_t*)0xE000E190)
#define DRV_NVIC_ICER5							((__vo uint32_t*)0xE000E194)
#define DRV_NVIC_ICER6							((__vo uint32_t*)0xE000E198)
#define DRV_NVIC_ICER7							((__vo uint32_t*)0xE000E19C)

#define DRV_NVIC_IPR_BASE_ADDR					((__vo uint32_t*)0xE000E400)

#define NO_PR_BITS_IMPLEMENTED							4



//Defining base addresses of Flash and SRAM
#define DRV_FLASH_BASEADDR					0x08000000U		//defining the base address of FLASH/Main memory
#define DRV_SRAM1_BASEADDR					0x20000000U		//defining the base address of SRAM1
#define DRV_ROM_BASEADDR					0x1FFF0000U		//defining the base address of ROM/System memory
#define DRV_SRAM 							DRV_SRAM1_BASEADDR	//there is only one SRAM on this controller


//Defining AHBx and APBx bus peripherals
#define DRV_PERIPH_BASEADDR					0x40000000U		//defining the base address of peripherals
#define DRV_APB1PERIPH_BASEADDR				DRV_PERIPH_BASEADDR	//APB1 is first
#define DRV_APB2PERIPH_BASEADDR				0x40010000U		//defining the base address for APB2
#define DRV_AHB1PERIPH_BASEADDR				0x40020000U		//defining the base address for AHB1
#define DRV_AHB2PERIPH_BASEADDR				0x50000000U		//defining the base address for AHB2


//Defining base addresses of peripherals connected to AHB1
//We define the addresses as the AHB1 base address + the offset for each GPIO
#define DRV_GPIOA_BASEADDR					(DRV_AHB1PERIPH_BASEADDR + 0x0000)
#define DRV_GPIOB_BASEADDR					(DRV_AHB1PERIPH_BASEADDR + 0x0400)
#define DRV_GPIOC_BASEADDR					(DRV_AHB1PERIPH_BASEADDR + 0x0800)
#define DRV_GPIOD_BASEADDR					(DRV_AHB1PERIPH_BASEADDR + 0x0C00)
#define DRV_GPIOE_BASEADDR					(DRV_AHB1PERIPH_BASEADDR + 0x1000)
#define DRV_GPIOH_BASEADDR					(DRV_AHB1PERIPH_BASEADDR + 0x1C00)
#define DRV_RCC_BASEADDR					(DRV_AHB1PERIPH_BASEADDR + 0x3800)


//Defining the base addresses of peripherals that are connected to the APB1 bus
#define DRV_I2C1_BASEADDR					(DRV_APB1PERIPH_BASEADDR + 0x5400)
#define DRV_I2C2_BASEADDR					(DRV_APB1PERIPH_BASEADDR + 0x5800)
#define DRV_I2C3_BASEADDR					(DRV_APB1PERIPH_BASEADDR + 0x5C00)
#define DRV_SPI2_BASEADDR					(DRV_APB1PERIPH_BASEADDR + 0x3800)
#define DRV_SPI3_BASEADDR					(DRV_APB1PERIPH_BASEADDR + 0x3C00)
#define DRV_USART2_BASEADDR					(DRV_APB1PERIPH_BASEADDR + 0x4400)


//Defining the base addresses of peripherals that are connected to the APB2 bus
#define DRV_SPI1_BASEADDR					(DRV_APB2PERIPH_BASEADDR + 0x3000)
#define DRV_SPI4_BASEADDR					(DRV_APB2PERIPH_BASEADDR + 0x3400)
#define DRV_USART1_BASEADDR					(DRV_APB2PERIPH_BASEADDR + 0x1000)
#define DRV_USART6_BASEADDR					(DRV_APB2PERIPH_BASEADDR + 0x1400)
#define DRV_SYSCFG_BASEADDR					(DRV_APB2PERIPH_BASEADDR + 0x3800)
#define DRV_EXTI_BASEADDR					(DRV_APB2PERIPH_BASEADDR + 0x3C00)




/*************************** Peripheral register definition structure *********************************/
//Defining GPIO peripheral register structure
typedef struct
{
	__vo uint32_t MODER;			//Defining the port mode register							Address offset: 0x00
	__vo uint32_t OTYPER;			//Defining the output type of I/O port						Address offset: 0x04
	__vo uint32_t OSPEEDR;			//Defining the I/O output speed								Address offset: 0x08
	__vo uint32_t PUPDR;			//Defining the I/O as pull up/down 							Address offset: 0x0C
	__vo uint32_t IDR;				//These registers can only be read							Address offset: 0x10
	__vo uint32_t ODR;				//to be added												Address offset: 0x14
	__vo uint32_t BSRR;				//to be added												Address offset: 0x18
	__vo uint32_t LCKR;				//Defining the port configuration locked reg				Address offset: 0x1C
	__vo uint32_t AFR[2];			//AFR[0] is the alternate function low reg, 				Address offset: 0x20
									//AFR[1] is the alternate function high reg  				Address offset: 0x24
}GPIO_RegDef_t;


/************************** RCC register definition structure ***********************************************/
typedef struct
{
	__vo uint32_t CR;				//															Address offset: 0x00
	__vo uint32_t PLLCFGR;			//															Address offset: 0x04
	__vo uint32_t CFGR;				//															Address offset: 0x08
	__vo uint32_t CIR;				//															Address offset: 0x0C
	__vo uint32_t AHB1RSTR;			//															Address offset: 0x10
	__vo uint32_t AHB2RSTR;			//															Address offset: 0x14
	uint32_t RESERVED0[2];			//Reserved register											Address offset: 0x18 - 0x1C
	__vo uint32_t APB1RSTR;			//															Address offset: 0x20
	__vo uint32_t APB2RSTR;			//															Address offset: 0x24
	uint32_t RESERVED1[2];			//Reserved register											Address offset: 0x28 - 0x2C
	__vo uint32_t AHB1ENR;			//															Address offset: 0x30
	__vo uint32_t AHB2ENR;			//															Address offset: 0x34
	uint32_t RESERVED2[2];			//Reserved register											Address offset: 0x38 - 0x3C
	__vo uint32_t APB1ENR;			//															Address offset: 0x40
	__vo uint32_t APB2ENR;			//															Address offset: 0x44
	uint32_t RESERVED3[2];			//Reserved register											Address offset: 0x48 - 0x4C
	__vo uint32_t AHB1LPENR;		//															Address offset: 0x50
	__vo uint32_t AHB2LPENR;		//															Address offset: 0x54
	uint32_t RESERVED4[2];			//Reserved register											Address offset: 0x58 - 0x5C
	__vo uint32_t APB1LPENR;		//															Address offset: 0x60
	__vo uint32_t APB2LPENR;		//															Address offset: 0x64
	uint32_t RESERVED5[2];			//Reserved register											Address offset: 0x68 - 0x6C
	__vo uint32_t BDCR;				//															Address offset: 0x70
	__vo uint32_t CSR;				//															Address offset: 0x74
	uint32_t RESERVED6[2];			//Reserved register											Address offset: 0x78 - 0x7C
	__vo uint32_t SSCGR;			//															Address offset: 0x80
	__vo uint32_t PLLI2SCFGR;		//															Address offset: 0x84
	uint32_t RESERVED7;				//Reserved register											Address offset: 0x88
	__vo uint32_t DCKCFGR;			//TIMPRE bit 24 for use, everything else is reserved		Address offset: 0x8C


}RCC_RegDef_t;


/************************** EXTI register definition structure ***********************************************/
typedef struct
{
	__vo uint32_t IMR;				//to be added												Address offset: 0x00
	__vo uint32_t EMR;				//to be added												Address offset: 0x04
	__vo uint32_t RTSR;				//to be added												Address offset: 0x08
	__vo uint32_t FTSR;				//to be added 												Address offset: 0x0C
	__vo uint32_t SWIER;			//to be added												Address offset: 0x10
	__vo uint32_t PR;				//to be added												Address offset: 0x14

}EXTI_RegDef_t;

/************************** SYSCFG register definition structure ***********************************************/
typedef struct
{
	__vo uint32_t MEMRMP;			//to be added												Address offset: 0x00
	__vo uint32_t PMC;				//to be added												Address offset: 0x04
	__vo uint32_t EXTICR[4];			//to be added											Address offset: 0x08 - 0x14
	uint32_t RESERVED1[2];			//Reserved register											Address offset: 0x18 - 0x1C
	__vo uint32_t CMPCR;			//to be added												Address offset: 0x20
	uint32_t RESERVED2[2];			//Reserved register											Address offset: 0x24 - 0x28
	__vo uint32_t CFGR;				//to be added												Address offset: 0x2C


}SYSCFG_RegDef_t;


/************************** SPI register definition structure ***********************************************/
typedef struct
{
	__vo uint32_t CR1;				//to be added												Address offset: 0x00
	__vo uint32_t CR2;			//Reserved register												Address offset: 0x04
	__vo uint32_t SR;				//to be added												Address offset: 0x08
	__vo uint32_t DR;				//to be added 												Address offset: 0x0C
	__vo uint32_t CRCPR;			//to be added												Address offset: 0x10
	__vo uint32_t RXCRCR;			//to be added											    Address offset: 0x14
	__vo uint32_t TXCRCR;			//to be added											    Address offset: 0x18
	__vo uint32_t I2SCFGR;			//to be added											    Address offset: 0x1C
	__vo uint32_t I2SPR;			//to be added											    Address offset: 0x20


}SPI_RegDef_t;

/************************** I2C register definition structure ***********************************************/
typedef struct
{
	__vo uint32_t CR1;				//to be added												Address offset: 0x00
	__vo uint32_t CR2;				//to be added												Address offset: 0x04
	__vo uint32_t OAR1;				//to be added												Address offset: 0x08
	__vo uint32_t OAR2;				//to be added 												Address offset: 0x0C
	__vo uint32_t DR;				//to be added												Address offset: 0x10
	__vo uint32_t SR1;				//to be added											    Address offset: 0x14
	__vo uint32_t SR2;				//to be added											    Address offset: 0x18
	__vo uint32_t CCR;				//to be added											    Address offset: 0x1C
	__vo uint32_t TRISE;			//to be added											    Address offset: 0x20
	__vo uint32_t FLTR;				//to be added											    Address offset: 0x24


}I2C_RegDef_t;




//Peripheral definitions
#define DRV_GPIOA						((GPIO_RegDef_t*) DRV_GPIOA_BASEADDR)
#define DRV_GPIOB						((GPIO_RegDef_t*) DRV_GPIOB_BASEADDR)
#define DRV_GPIOC						((GPIO_RegDef_t*) DRV_GPIOC_BASEADDR)
#define DRV_GPIOD						((GPIO_RegDef_t*) DRV_GPIOD_BASEADDR)
#define DRV_GPIOE						((GPIO_RegDef_t*) DRV_GPIOE_BASEADDR)
#define DRV_GPIOH						((GPIO_RegDef_t*) DRV_GPIOH_BASEADDR)

//Defining the Reset and clock control
#define DRV_RCC								((RCC_RegDef_t*) DRV_RCC_BASEADDR)

#define DRV_EXTI							((EXTI_RegDef_t*) DRV_EXTI_BASEADDR)

#define DRV_SYSCFG							((SYSCFG_RegDef_t*) DRV_SYSCFG_BASEADDR)

//Defining SPI
#define DRV_SPI1							((SPI_RegDef_t*) DRV_SPI1_BASEADDR)
#define DRV_SPI2							((SPI_RegDef_t*) DRV_SPI2_BASEADDR)
#define DRV_SPI3							((SPI_RegDef_t*) DRV_SPI3_BASEADDR)
#define DRV_SPI4							((SPI_RegDef_t*) DRV_SPI4_BASEADDR)

//Defining I2C
#define DRV_I2C1							((I2C_RegDef_t*) DRV_I2C1_BASEADDR)
#define DRV_I2C2							((I2C_RegDef_t*) DRV_I2C2_BASEADDR)
#define DRV_I2C3							((I2C_RegDef_t*) DRV_I2C3_BASEADDR)

/***************************** Defining peripheral clock enable macros *********************************/

//Clock enable macros for GPIOx peripherals
#define DRV_GPIOA_PCLK_EN()		(DRV_RCC->AHB1ENR |= (1 << 0));
#define DRV_GPIOB_PCLK_EN()		(DRV_RCC->AHB1ENR |= (1 << 1));
#define DRV_GPIOC_PCLK_EN()		(DRV_RCC->AHB1ENR |= (1 << 2));
#define DRV_GPIOD_PCLK_EN()		(DRV_RCC->AHB1ENR |= (1 << 3));
#define DRV_GPIOE_PCLK_EN()		(DRV_RCC->AHB1ENR |= (1 << 4));
#define DRV_GPIOH_PCLK_EN()		(DRV_RCC->AHB1ENR |= (1 << 7));

//Defining I2Cx clock enable macros
#define DRV_I2C1_PCLK_EN()		(DRV_RCC->APB1ENR |= (1 << 21));
#define DRV_I2C2_PCLK_EN()		(DRV_RCC->APB1ENR |= (1 << 22));
#define DRV_I2C3_PCLK_EN()		(DRV_RCC->APB1ENR |= (1 << 23));

//Defining SPI clock enable macros
#define DRV_SPI2_PCLK_EN()		(DRV_RCC->APB1ENR |= (1 << 14));
#define DRV_SPI3_PCLK_EN()		(DRV_RCC->APB1ENR |= (1 << 15));
#define DRV_SPI1_PCLK_EN()		(DRV_RCC->APB2ENR |= (1 << 12));
#define DRV_SPI4_PCLK_EN()		(DRV_RCC->APB2ENR |= (1 << 13));

//Defining USART clock enable macros
#define DRV_USART2_PCLK_EN()	(DRV_RCC->APB1ENR |= (1 << 17));
#define DRV_USART1_PCLK_EN()	(DRV_RCC->APB2ENR |= (1 << 4));
#define DRV_USART6_PCLK_EN()	(DRV_RCC->APB2ENR |= (1 << 5));

//Defining SYSCFG clock enable macros
#define DRV_SYSCFG_PCLK_EN()	(DRV_RCC->APB2ENR |= (1 << 14));


/***************************** Defining peripheral clock disable macros *********************************/

//Clock disable macros for GPIOx peripherals
#define DRV_GPIOA_PCLK_DI()		(DRV_RCC->AHB1ENR &= ~(1 << 0));
#define DRV_GPIOB_PCLK_DI()		(DRV_RCC->AHB1ENR &= ~(1 << 1));
#define DRV_GPIOC_PCLK_DI()		(DRV_RCC->AHB1ENR &= ~(1 << 2));
#define DRV_GPIOD_PCLK_DI()		(DRV_RCC->AHB1ENR &= ~(1 << 3));
#define DRV_GPIOE_PCLK_DI()		(DRV_RCC->AHB1ENR &= ~(1 << 4));
#define DRV_GPIOH_PCLK_DI()		(DRV_RCC->AHB1ENR &= ~(1 << 7));

//Defining I2Cx clock disable macros
#define DRV_I2C1_PCLK_DI()		(DRV_RCC->APB1ENR &= ~(1 << 21));
#define DRV_I2C2_PCLK_DI()		(DRV_RCC->APB1ENR &= ~(1 << 22));
#define DRV_I2C3_PCLK_DI()		(DRV_RCC->APB1ENR &= ~(1 << 23));

//Defining SPI clock disable macros
#define DRV_SPI2_PCLK_DI()		(DRV_RCC->APB1ENR &= ~(1 << 14));
#define DRV_SPI3_PCLK_DI()		(DRV_RCC->APB1ENR &= ~(1 << 15));
#define DRV_SPI1_PCLK_DI()		(DRV_RCC->APB2ENR &= ~(1 << 12));
#define DRV_SPI4_PCLK_DI()		(DRV_RCC->APB2ENR &= ~(1 << 13));

//Defining USART clock enable macros
#define DRV_USART2_PCLK_DI()	(DRV_RCC->APB1ENR &= ~(1 << 17));
#define DRV_USART1_PCLK_DI()	(DRV_RCC->APB2ENR &= ~(1 << 4));
#define DRV_USART6_PCLK_DI()	(DRV_RCC->APB2ENR &= ~(1 << 5));

//Defining SYSCFG clock disable macros
#define DRV_SYSCFG_PCLK_DI()	(DRV_RCC->APB2ENR &= ~(1 << 14));


//Macros to reset the GPIO peripherals
#define DRV_GPIOA_REG_RST()		do{(DRV_RCC->AHB1RSTR |= (1 << 0)); (DRV_RCC->AHB1RSTR &= ~(1 << 0));}while(0)
#define DRV_GPIOB_REG_RST()		do{(DRV_RCC->AHB1RSTR |= (1 << 1)); (DRV_RCC->AHB1RSTR &= ~(1 << 1));}while(0)
#define DRV_GPIOC_REG_RST()		do{(DRV_RCC->AHB1RSTR |= (1 << 2)); (DRV_RCC->AHB1RSTR &= ~(1 << 2));}while(0)
#define DRV_GPIOD_REG_RST()		do{(DRV_RCC->AHB1RSTR |= (1 << 3)); (DRV_RCC->AHB1RSTR &= ~(1 << 3));}while(0)
#define DRV_GPIOE_REG_RST()		do{(DRV_RCC->AHB1RSTR |= (1 << 4)); (DRV_RCC->AHB1RSTR &= ~(1 << 4));}while(0)
#define DRV_GPIOH_REG_RST()		do{(DRV_RCC->AHB1RSTR |= (1 << 7)); (DRV_RCC->AHB1RSTR &= ~(1 << 7));}while(0)


#define GPIO_BASEADDR_TO_CODE(x)	   ((x == DRV_GPIOA)?0:\
										(x == DRV_GPIOB)?1:\
										(x == DRV_GPIOC)?2:\
										(x == DRV_GPIOD)?3:\
										(x == DRV_GPIOE)?4:\
										(x == DRV_GPIOH)?7:0)


//IRQ numbers
#define IRQ_NO_EXTI0			6
#define IRQ_NO_EXTI1			7
#define IRQ_NO_EXTI2			8
#define IRQ_NO_EXTI3			9
#define IRQ_NO_EXTI4			10
#define IRQ_NO_EXTI9_5			23
#define IRQ_NO_EXTI15_10		40
#define IRQ_NO_EXTI17			41
#define IRQ_NO_EXTI18			42

#define IRQ_NO_SPI1				35
#define IRQ_NO_SPI2				36
#define IRQ_NO_SPI3				51
#define IRQ_NO_SPI4				84

#define IRQ_NO_I2C1_EV			31
#define IRQ_NO_I2C1_ER			32
#define IRQ_NO_I2C2_EV			33
#define IRQ_NO_I2C2_ER			34
#define IRQ_NO_I2C3_EV			72
#define IRQ_NO_I2C3_ER			73

//IRQ priority def
#define NVIC_IRQ_PRI0			0
#define NVIC_IRQ_PRI1			1
#define NVIC_IRQ_PRI2			2
#define NVIC_IRQ_PRI3			3
#define NVIC_IRQ_PRI4			4
#define NVIC_IRQ_PRI5			5
#define NVIC_IRQ_PRI6			6
#define NVIC_IRQ_PRI7			7
#define NVIC_IRQ_PRI8			8
#define NVIC_IRQ_PRI9			9
#define NVIC_IRQ_PRI10			10
#define NVIC_IRQ_PRI11			11
#define NVIC_IRQ_PRI12			12
#define NVIC_IRQ_PRI13			13
#define NVIC_IRQ_PRI14			14
#define NVIC_IRQ_PRI15			15

//Some generic macros
#define ENABLE 					1
#define DISABLE 				0
#define SET 					ENABLE
#define RESET 					DISABLE
#define GPIO_PIN_SET			SET
#define GPIO_PIN_RESET			RESET
#define FLAG_SET				SET
#define FLAG_RESET				RESET

/******************************************************************************
 * 					Bit position definitions of SPI peripheral
 ******************************************************************************/

//Defining macros for CR1
#define SPI_CR1_CPHA			0
#define SPI_CR1_CPOL			1
#define SPI_CR1_MSTR			2
#define SPI_CR1_BR				3
#define SPI_CR1_SPE				6
#define SPI_CR1_LSBFIRST 		7
#define SPI_CR1_SSI				8
#define SPI_CR1_SSM				9
#define SPI_CR1_RXONLY			10
#define SPI_CR1_DFF				11
#define SPI_CR1_CRCNEXT			12
#define SPI_CR1_CRCEN			13
#define SPI_CR1_BIDIOE			14
#define SPI_CR1_BIDIMODE		15

//Defining macros for CR2
#define SPI_CR2_RXDMAEN			0
#define SPI_CR2_TXDMAEN 		1
#define SPI_CR2_SSOE			2
#define SPI_CR2_FRF				4
#define SPI_CR2_ERRIE			5
#define SPI_CR2_RXNEIE			6
#define SPI_CR2_TXEIE			7

//Defining macros for SR
#define SPI_SR_RXNE				0
#define SPI_SR_TXE 				1
#define SPI_SR_CHSIDE			2
#define SPI_SR_UDR				3
#define SPI_SR_CRCERR			4
#define SPI_SR_MODF				5
#define SPI_SR_OVR				6
#define SPI_SR_BSY				7
#define SPI_SR_FRE				8


/******************************************************************************
 * 					Bit position definitions of I2C peripheral
 ******************************************************************************/

//Defining macros for CR1
#define I2C_CR1_PE				0
#define I2C_CR1_SMBUS			1
#define I2C_CR1_SMBTYPE			3
#define I2C_CR1_ENARP			4
#define I2C_CR1_ENPEC			5
#define I2C_CR1_ENGC			6
#define I2C_CR1_NOSTRETCH		7
#define I2C_CR1_START			8
#define I2C_CR1_STOP			9
#define I2C_CR1_ACK				10
#define I2C_CR1_POS				11
#define I2C_CR1_PEC				12
#define I2C_CR1_ALERT			13
#define I2C_CR1_SWRST			15

//Defining macros for CR2
#define I2C_CR2_FREQ			0
#define I2C_CR2_ITERREN			8
#define I2C_CR2_ITEVTEN			9
#define I2C_CR2_ENARP			10
#define I2C_CR2_ITBUFEN			11
#define I2C_CR2_DMAEN			12
#define I2C_CR2_LAST			13

//Defining macros for SR1
#define I2C_SR1_SB				0
#define I2C_SR1_ADDR			1
#define I2C_SR1_BTF				2
#define I2C_SR1_ADD10			3
#define I2C_SR1_STOPF			4
#define I2C_SR1_RXNE			6
#define I2C_SR1_TXE			7
#define I2C_SR1_BERR			8
#define I2C_SR1_ARLO			9
#define I2C_SR1_AF				10
#define I2C_SR1_OVR				11
#define I2C_SR1_PECERR			12
#define I2C_SR1_TIMEOUT			14
#define I2C_SR1_SMBALERT		15

//Defining macros for SR2
#define I2C_SR2_MSL				0
#define I2C_SR2_BUSY			1
#define I2C_SR2_TRA				2
#define I2C_SR2_GENCALL			4
#define I2C_SR2_SMBDEFAUL		5
#define I2C_SR2_SMBHOST			6
#define I2C_SR2_DUALF			7
#define I2C_SR2_PEC				8

//Defining macros for CCR
#define I2C_CCR_CCR				0
#define I2C_CCR_DUTY			14
#define I2C_CCR_FS				15






#include "stm32f401xx_gpio_driver.h"
#include "stm32f401xx_spi_driver.h"
#include "stm32f401xx_i2c_driver.h"


#endif /* INC_STM32F401XX_H_ */
