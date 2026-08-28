/*
 * stm32f407xx_rcc_driver.h
 *
 *  Created on: Aug 1, 2026
 *      Author: dell
 */

#ifndef INC_STM32F407XX_RCC_DRIVER_H_
#define INC_STM32F407XX_RCC_DRIVER_H_

#include "stm32f407xx.h"

/*
 * Returns APB1 Clock value
 */
uint32_t RCC_GetPCLK1Value(void);

/*
 * Returns APB2 Clock value
 */
uint32_t RCC_GetPCLK2Value(void);

//not implemented
uint32_t RCC_GetPPLOutputClk(void);

#endif /* INC_STM32F407XX_RCC_DRIVER_H_ */
