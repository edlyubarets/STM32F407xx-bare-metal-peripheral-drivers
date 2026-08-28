/*
 * stm32f407xx_gpio_driver.h
 *
 *  Created on: Jul 12, 2026
 *      Author: dell
 */

#ifndef INC_STM32F407XX_GPIO_DRIVER_H_
#define INC_STM32F407XX_GPIO_DRIVER_H_


#include <stdint.h>
#include "stm32f407xx.h"


/*Configuration structure for a gpio pin*/
typedef struct
{
	uint8_t GPIO_PinNumber;			/*possible values from @GPIO_PIN_NUMBER*/
	uint8_t GPIO_PinMode;			/*possible values from @GPIO_PIN_MODE*/
	uint8_t GPIO_PinSpeed;			/*possible values from @GPIO_PIN_SPEED*/
	uint8_t GPIO_PinPuPdCtrl;		/*possible values from @GPIO_PIN_PUPD*/
	uint8_t GPIO_PinOPType;			/*possible values from @GPIO_PIN_OUTPUT*/
	uint8_t GPIO_PinAltFunMode;
}GPIO_PinConfig_t;


/* Handle structure for GPIO pin*/
typedef struct
{
	//pointer to hold base address of gpio peripheral
	GPIO_RegDef_t *GPIOx;		/*This holds the base address of the GPIO port to which the pin belongs*/
	GPIO_PinConfig_t GPIO_PinConfig;

}GPIO_Handle_t;



/* @GPIO_PIN_NUMBER
 * GPIO pin numbers */
#define GPIO_PIN_0		0
#define GPIO_PIN_1		1
#define GPIO_PIN_2		2
#define GPIO_PIN_3		3
#define GPIO_PIN_4		4
#define GPIO_PIN_5		5
#define GPIO_PIN_6		6
#define GPIO_PIN_7		7
#define GPIO_PIN_8		8
#define GPIO_PIN_9		9
#define GPIO_PIN_10		10
#define GPIO_PIN_11		11
#define GPIO_PIN_12		12
#define GPIO_PIN_13		13
#define GPIO_PIN_14		14
#define GPIO_PIN_15		15

/* @GPIO_PIN_MODE
 * GPIO pin possible modes*/
#define GPIO_MODE_IN 			0
#define GPIO_MODE_OUT			1
#define GPIO_MODE_ALTFN			2
#define GPIO_MODE_ANALOG		3
#define GPIO_MODE_IT_FT			4
#define GPIO_MODE_IT_RT			5
#define GPIO_MODE_IT_RFT		6

/* @GPIO_PIN_OUTPUT
 * GPIO possible output types*/
#define GPIO_OP_TYPE_PP			0
#define GPIO_OP_TYPE_OD			1

/* @GPIO_PIN_SPEED
 * GPIO possible output speeds*/
#define GPIO_SPEED_LOW			0
#define GPIO_SPEED_MEDIUM		1
#define GPIO_SPEED_FAST			2
#define GPIO_SPEED_HIGH			3

/* @GPIO_PIN_PUPD
 * GPIO pin PULL-UP & PULL-DOWN configuration macros*/
#define GPIO_NO_PUPD			0
#define GPIO_PIN_PU				1
#define GPIO_PIN_PD				2






/*****************************************************************************************
 * 								APIs supported by driver
 * 					Check the function definitions for more info
 *****************************************************************************************/

/*Peripheral clock setup*/
void GPIO_PClkCtrl(GPIO_RegDef_t *pGPIOx, uint8_t EnDi);

/*initialization & de-initialization*/
void GPIO_Init(GPIO_Handle_t *pGPIOHandle);
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx);

/*Data read & write*/
uint8_t GPIO_ReadInputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber);
uint16_t GPIO_ReadInputPort(GPIO_RegDef_t *pGPIOx);
void GPIO_WriteOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber, uint8_t value);
void GPIO_WriteOutputPort(GPIO_RegDef_t *pGPIOx, uint16_t value);
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber);

/*IRQ configuration and ISR handling*/
void GPIO_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnDi);
void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void GPIO_IRQHandling(uint8_t PinNumber);



#endif /* INC_STM32F407XX_GPIO_DRIVER_H_ */
