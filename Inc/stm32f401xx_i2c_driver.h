/*
 * stm32f401xx_i2c_driver.h
 *
 *  Created on: Jun 29, 2025
 *      Author: Nikola Sokolović
 */

#ifndef INC_STM32F401XX_I2C_DRIVER_H_
#define INC_STM32F401XX_I2C_DRIVER_H_

#include "stm32f401xx.h"


//We define the I2C configuration structure
typedef struct{
	uint32_t I2C_SCLSpeed;
	uint8_t I2C_DeviceAddress; //mentioned by the user
	uint8_t I2C_ACKControl;
	uint16_t I2C_FMDutyCycle;

}I2C_Config_t;


//We define the handle structure for I2C
typedef struct{
	I2C_RegDef_t  *pI2Cx;
	I2C_Config_t  I2CConfig;

	uint8_t       *pTxBuffer; //store the app Tx buffer
	uint8_t 	  *pRxBuffer; //store the app Rx buffer
	uint32_t 	  TxLen; //store the app Tx len
	uint32_t 	  RxLen; //store the app Rx len
	uint8_t 	  TxRxState; //store the app Tx buffer
	uint8_t 	  DevAddr; //store slave/device address
	uint32_t 	  RxSize; // to store Rx size
	uint8_t 	  Sr; //To store repeated start value

}I2C_Handle_t;

//@I2C_SCLSpeed
#define I2C_SCL_SPEED_SM		100000
#define I2C_SCL_SPEED_FM		400000

//@I2C_ACKControl
#define I2C_ACK_ENABLE			1
#define I2C_ACK_DISABLE			0

//@I2C_FMDutyCycle
#define I2C_FM_DUTY_9			0
#define I2C_FM_DUTY_16_9		1

//I2C application status
#define I2C_READY				0
#define I2C_BUSY_IN_RX			1
#define I2C_BUSY_IN_TX			2

//I2C related status flags
#define I2C_TXE_FLAG						(1 << I2C_SR1_TXNE)
#define I2C_RXNE_FLAG						(1 << I2C_SR1_RXNE)
#define I2C_SB_FLAG							(1 << I2C_SR1_SB)
#define I2C_ADDR_FLAG						(1 << I2C_SR1_ADDR)
#define I2C_BTF_FLAG						(1 << I2C_SR1_BTF)
#define I2C_STOPF_FLAG						(1 << I2C_SR1_STOPF)
#define I2C_BERR_FLAG						(1 << I2C_SR1_BERR)
#define I2C_ARLO_FLAG						(1 << I2C_SR1_ARLO)
#define I2C_AF_FLAG							(1 << I2C_SR1_AF)
#define I2C_OVR_FLAG						(1 << I2C_SR1_OVR)
#define I2C_PECERR_FLAG						(1 << I2C_SR1_PECERR)
#define I2C_TIMEOUT_FLAG					(1 << I2C_SR1_TIMEOUT)

#define I2C_NO_SR 							RESET
#define I2C_SR 								SET

/*
 * 				We define the APIs supported by this driver
 * */

//Peripheral clock setup
void I2C_PeriClockControl(I2C_RegDef_t *pI2Cx, uint8_t EnorDi);

//Init and De-Init of I2C
void I2C_Init(I2C_Handle_t *pI2CHandle);
void I2C_DeInit(I2C_RegDef_t *pI2Cx);
uint8_t I2CGetFlagStatus(I2C_RegDef_t *pI2Cx, uint32_t FlagSet);

//IRQ configuration and ISR handling
void I2C_IRQITConfig(uint8_t IRQNumber, uint8_t EnorDi);
void I2C_IRQPriority_Config(uint8_t IRQNumber, uint32_t IRQPriority);
void I2C_EV_IRQHandling(I2C_Handle_t *pI2CHandle);
void I2C_ER_IRQHandling(I2C_Handle_t *pI2CHandle);

//Other peripheral control APIs
void I2C_PeripheralControl(I2C_RegDef_t *pI2Cx, uint8_t EnOrDi);
void I2C_ManageAcking(I2C_RegDef_t *pI2Cx, uint8_t EnOrDi);

void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle, uint8_t AppEv);

void I2C_MasterSendData(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint8_t Len, uint8_t SlaveAddr, uint8_t Sr);
void I2C_MasterRecieveData(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer, uint8_t Len, uint8_t SlaveAddr, uint8_t Sr);

uint8_t I2C_MasterSendDataIT(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint8_t Len, uint8_t SlaveAddr, uint8_t Sr);
uint8_t I2C_MasterRecieveDataIT(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer, uint8_t Len, uint8_t SlaveAddr, uint8_t Sr);



#endif /* INC_STM32F401XX_I2C_DRIVER_H_ */
