/*
 * stm32f407xx_gpio.c
 *
 *  Created on: Jul 12, 2026
 *      Author: dell
 */

#include "stm32f407xx.h"


/*Peripheral clock setup*/

/*********************************************************************************
 * @fn				- GPIO_PClkCtrl
 *
 * @brief			- This function enables or disables peripheral clock for GPIO port
 *
 * @param[in]		- base address of gpio peripheral
 * @param[in]		- ENABLE or DISABLE macros
 * @param[in]
 *
 * @return			- none
 *
 * @note			- none
 *
 */
void GPIO_PClkCtrl(GPIO_RegDef_t *pGPIOx, uint8_t EnDi)
{
	if(EnDi == ENABLE)
	{
		if(pGPIOx == GPIOA)
		{
			GPIOA_PCLK_EN();

		} else if(pGPIOx == GPIOB)
		{
			GPIOB_PCLK_EN();

		} else if(pGPIOx == GPIOC)
		{
			GPIOC_PCLK_EN();

		} else if(pGPIOx == GPIOD)
		{
			GPIOD_PCLK_EN();

		} else if(pGPIOx == GPIOE)
		{
			GPIOE_PCLK_EN();

		} else if(pGPIOx == GPIOF)
		{
			GPIOF_PCLK_EN();

		} else if(pGPIOx == GPIOG)
		{
			GPIOG_PCLK_EN();

		} else if(pGPIOx == GPIOH)
		{
			GPIOH_PCLK_EN();

		} else if(pGPIOx == GPIOI)
		{
			GPIOI_PCLK_EN();

		}

	} else
	{
		if(pGPIOx == GPIOA)
		{
			GPIOA_PCLK_DI();

		} else if(pGPIOx == GPIOB)
		{
			GPIOB_PCLK_DI();

		} else if(pGPIOx == GPIOC)
		{
			GPIOC_PCLK_DI();

		} else if(pGPIOx == GPIOD)
		{
			GPIOD_PCLK_DI();

		} else if(pGPIOx == GPIOE)
		{
			GPIOE_PCLK_DI();

		} else if(pGPIOx == GPIOF)
		{
			GPIOF_PCLK_DI();

		} else if(pGPIOx == GPIOG)
		{
			GPIOG_PCLK_DI();

		} else if(pGPIOx == GPIOH)
		{
			GPIOH_PCLK_DI();

		} else if(pGPIOx == GPIOI)
		{
			GPIOI_PCLK_DI();

		}
	}
}



/*initialization & deinitialization*/

/*********************************************************************************
 * @fn				- GPIO_Init
 *
 * @brief			- This function initializes given GPIO Pin with various parameters
 *
 * @param[in]		- Base address of configuration structure for a gpio pin
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- none
 *
 * @note			- none
 *
 */
void GPIO_Init(GPIO_Handle_t *pGPIOHandle)
{
	uint32_t temp;		//temp register


	//enable the peripheral clock
	GPIO_PClkCtrl(pGPIOHandle->GPIOx, ENABLE);


	//1. configure the mode of gpio pin
	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode <= GPIO_MODE_ANALOG) {

		//non-interrupt mode
		temp = ( pGPIOHandle->GPIO_PinConfig.GPIO_PinMode << ( 2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber ) );
		pGPIOHandle->GPIOx->MODER &= ~(0x3 << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber) );
		pGPIOHandle->GPIOx->MODER |= temp;

	} else {

		//interrupt mode

		if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_FT) {

			//1. Configure the FTSR
			EXTI->FTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
			//clear the corresponding RTSR bit
			EXTI->RTSR &= ~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

		} else if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RT) {

			//1. Configure the RTSR
			EXTI->RTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
			//clear the corresponding FTSR bit
			EXTI->FTSR &= ~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

		} else if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_IT_RFT) {

			//1. Configure both FTSR and RTSR
			EXTI->FTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);
			EXTI->RTSR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

		}

		//2. Configure the GPIO port selection in SYSCFG_EXTICR
		uint8_t temp1 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber / 4;
		uint8_t temp2 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber % 4;
		uint8_t portcode = gpio_base_addr_to_code(pGPIOHandle->GPIOx);
		SYSCFG_PCLK_EN();
		SYSCFG->EXTICR[temp1] = (portcode << (temp2 * 4));

		//3. Enable the EXTI interrupt delivery using IMR(interrupt mask register)
		EXTI->IMR |= (1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber);

	}

	temp = 0;

	//2. configure the speed
	temp = ( pGPIOHandle->GPIO_PinConfig.GPIO_PinSpeed << ( 2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber ) );
	pGPIOHandle->GPIOx->OSPEED &= ~(0x3 << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber) );
	pGPIOHandle->GPIOx->OSPEED |= temp;

	temp = 0;


	//3. configure the pupd settings
	temp = ( pGPIOHandle->GPIO_PinConfig.GPIO_PinPuPdCtrl << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber ) );
	pGPIOHandle->GPIOx->PUPDR &= ~(0x3 << (2 * pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber) );
	pGPIOHandle->GPIOx->PUPDR |= temp;

	temp = 0;

	//4. configure the optype
	temp = ( pGPIOHandle->GPIO_PinConfig.GPIO_PinOPType << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber );
	pGPIOHandle->GPIOx->OTYPER &= ~(1 << pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber );
	pGPIOHandle->GPIOx->OTYPER |= temp;

	temp = 0;

	//5. configure the alt functionality
	if(pGPIOHandle->GPIO_PinConfig.GPIO_PinMode == GPIO_MODE_ALTFN)
	{
		uint8_t temp1, temp2;

		temp1 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber / 8;
		temp2 = pGPIOHandle->GPIO_PinConfig.GPIO_PinNumber % 8;

		if(temp1)
		{
			pGPIOHandle->GPIOx->AFRH &= ~( 0xF << (4 * temp2 ) );
			pGPIOHandle->GPIOx->AFRH |= ( pGPIOHandle->GPIO_PinConfig.GPIO_PinAltFunMode << (4 * temp2) );

		} else
		{
			pGPIOHandle->GPIOx->AFRL &= ~( 0xF << (4 * temp2 ) );
			pGPIOHandle->GPIOx->AFRL |= ( pGPIOHandle->GPIO_PinConfig.GPIO_PinAltFunMode << (4 * temp2) );

		}
	}
}

/*********************************************************************************
 * @fn				- GPIO_DeInit
 *
 * @brief			- This function de-initializes given GPIO port
 *
 * @param[in]		- base address of gpio peripheral
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- none
 *
 * @note			- none
 *
 */
void GPIO_DeInit(GPIO_RegDef_t *pGPIOx)
{
	if(pGPIOx == GPIOA)
	{
		GPIOA_REG_RESET();

	} else if(pGPIOx == GPIOB)
	{
		GPIOB_REG_RESET();

	} else if(pGPIOx == GPIOC)
	{
		GPIOC_REG_RESET();

	} else if(pGPIOx == GPIOD)
	{
		GPIOD_REG_RESET();

	} else if(pGPIOx == GPIOE)
	{
		GPIOE_REG_RESET();

	} else if(pGPIOx == GPIOF)
	{
		GPIOF_REG_RESET();

	} else if(pGPIOx == GPIOG)
	{
		GPIOG_REG_RESET();

	} else if(pGPIOx == GPIOH)
	{
		GPIOH_REG_RESET();

	} else if(pGPIOx == GPIOI)
	{
		GPIOI_REG_RESET();
	}
}



/*Data read & write*/

/*********************************************************************************
 * @fn				- GPIO_ReadInputPin
 *
 * @brief			- This function reads the data from given GPIO pin
 *
 * @param[in]		- base address of gpio peripheral
 * @param[in]		- number of GPIO pin
 * @param[in]		-
 *
 * @return			- uint8_t (1 or 0)
 *
 * @note			- none
 *
 */
uint8_t GPIO_ReadInputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber)
{
	uint8_t value;

	value = (uint8_t)( (pGPIOx->IDR >> PinNumber) & 0x00000001);

	return value;
}

/*********************************************************************************
 * @fn				- GPIO_ReadInputPort
 *
 * @brief			- This function reads the data from given GPIO port
 *
 * @param[in]		- base address of gpio peripheral
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- uint16_t
 *
 * @note			- none
 *
 */
uint16_t GPIO_ReadInputPort(GPIO_RegDef_t *pGPIOx)
{
	uint16_t value;

	value = (uint16_t)( pGPIOx->IDR);

	return value;
}

/*********************************************************************************
 * @fn				- GPIO_WriteOutputPin
 *
 * @brief			- This function writes the data to given GPIO pin
 *
 * @param[in]		- base address of gpio peripheral
 * @param[in]		- number of GPIO pin
 * @param[in]		- SET or RESET macros
 *
 * @return			- none
 *
 * @note			- none
 *
 */
void GPIO_WriteOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber, uint8_t value)
{
	if(value) {
		//write 1
		pGPIOx->ODR |= (1 << PinNumber);
	} else {
		//write 0
		pGPIOx->ODR &= ~(1 << PinNumber);
	}
}

/*********************************************************************************
 * @fn				- GPIO_WriteOutputPort
 *
 * @brief			- This function writes the data to given GPIO port
 *
 * @param[in]		- base address of gpio peripheral
 * @param[in]		- 16 bit value
 * @param[in]		-
 *
 * @return			- none
 *
 * @note			- none
 *
 */
void GPIO_WriteOutputPort(GPIO_RegDef_t *pGPIOx, uint16_t value)
{
	pGPIOx->ODR = value;
}

/*********************************************************************************
 * @fn				- GPIO_ToggleOutputPin
 *
 * @brief			- This function changes previous value of GPIO pin
 *
 * @param[in]		- base address of gpio peripheral
 * @param[in]		- number of pin
 * @param[in]		-
 *
 * @return			- none
 *
 * @note			- none
 *
 */
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber)
{
	pGPIOx->ODR ^= (1 << PinNumber);
}



/*IRQ configuration and ISR handling*/

/*********************************************************************************
 * @fn				- GPIO_IRQInterruptConfig
 *
 * @brief			- This function configures the IRQ
 *
 * @param[in]		- number of IRQ
 * @param[in]		- ENABLE or DISABLE macros
 * @param[in]		-
 *
 * @return			- none
 *
 * @note			- none
 *
 */
void GPIO_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnDi)
{
	if(EnDi == ENABLE) {

		if(IRQNumber <= 31 ) {

			//program ISER0 reg
			*NVIC_ISER0 |= (1 << IRQNumber);

		} else if( (IRQNumber > 31) && (IRQNumber < 64) ) {

			//program ISER1 reg
			*NVIC_ISER1 |= (1 << (IRQNumber % 32));

		} else if( (IRQNumber >= 64) && (IRQNumber < 96) ) {

			//program ISER2 reg
			*NVIC_ISER2 |= (1 << (IRQNumber % 64));

		}


	} else {

		if(IRQNumber <= 31 ) {

			//program ICER0 reg
			*NVIC_ICER0 |= (1 << IRQNumber);

		} else if( (IRQNumber > 31) && (IRQNumber < 64) ) {

			//program ICER1 reg
			*NVIC_ICER1 |= (1 << (IRQNumber % 32));

		} else if( (IRQNumber >= 64) && (IRQNumber < 96) ) {

			//program ICER2 reg
			*NVIC_ICER2 |= (1 << (IRQNumber % 64));

		}


	}

}


/*********************************************************************************
 * @fn				- GPIO_IRQPriorityConfig
 *
 * @brief			- This function configures the IRQ priority
 *
 * @param[in]		- number of IRQ
 * @param[in]		- priority of IRQ
 * @param[in]		-
 *
 * @return			- none
 *
 * @note			- none
 *
 */
void GPIO_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
	//1. find out ipr reg
	uint8_t iprx = IRQNumber / 4;
	uint8_t iprx_sec = IRQNumber % 4;

	uint8_t shift_value = (iprx_sec * 8) + ( 8 - NO_PR_BITS_IMPLEMENTED );

	*(NVIC_PR_BASE_ADDR + (iprx * 4)) |= ( IRQPriority << shift_value );
}



/*********************************************************************************
 * @fn				- GPIO_IRQHandling
 *
 * @brief			- This function handles the IRQ
 *
 * @param[in]		- number of pin
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- none
 *
 * @note			- user configurable
 *
 */
void GPIO_IRQHandling(uint8_t PinNumber)
{
	//clear the exti pr reg corresponding to the pin
	if(EXTI->PR & (1 << PinNumber)) {
		//clear
		EXTI->PR |= ( 1 << PinNumber);
		/*
		 * In EXTI pending registers bits are cleared by setting them
		 */
	}
}

