/*
 * stm32f407xx_spi_driver.c
 *
 *  Created on: Jul 20, 2026
 *      Author: dell
 */

#include "stm32f407xx.h"
#include <stdlib.h>


/*Private helper functions prototypes*/
static void spi_txe_interrupt_handle(SPI_Handle_t *pSPIHandle);
static void spi_rxne_interrupt_handle(SPI_Handle_t *pSPIHandle);
static void spi_ovr_interrupt_handle(SPI_Handle_t *pSPIHandle);


/*Peripheral clock setup*/

/*********************************************************************************
 * @fn				- SPI_PClkCtrl
 *
 * @brief			- This function enables or disables peripheral clock for SPI peripheral
 *
 * @param[in]		- base address of spi peripheral
 * @param[in]		- ENABLE or DISABLE macros
 * @param[in]		-
 *
 * @return			- none
 *
 * @note			- none
 *
 */
void SPI_PClkCtrl(SPI_RegDef_t *pSPIx, uint8_t EnDi)
{
	if(EnDi == ENABLE) {

		if(pSPIx == SPI1) {
			SPI1_PCLK_EN();
		} else if(pSPIx == SPI2) {
			SPI2_PCLK_EN();
		} else if(pSPIx == SPI3) {
			SPI3_PCLK_EN();
		} else if(pSPIx == SPI4) {
			SPI4_PCLK_EN();
		}

	} else {

		if(pSPIx == SPI1) {
			SPI1_PCLK_DI();
		} else if(pSPIx == SPI2) {
			SPI2_PCLK_DI();
		} else if(pSPIx == SPI3) {
			SPI3_PCLK_DI();
		} else if(pSPIx == SPI4) {
			SPI4_PCLK_DI();
		}
	}
}



/*initialization & de-initialization*/

/*********************************************************************************
 * @fn				- SPI_Init
 *
 * @brief			- This function initializes spi peripheral
 *
 * @param[in]		- Base address of configuration structure for spi peripheral
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- none
 *
 * @note			- none
 *
 */
void SPI_Init(SPI_Handle_t *pSPIHandle)
{
	//enable the peripheral clock
	SPI_PClkCtrl(pSPIHandle->pSPIx, ENABLE);


	//Configure the SPI_CR1 reg

	uint32_t temp = 0;


	//1. configure the device mode
	temp |= (pSPIHandle->SPIConfig.SPI_DeviceMode << 2);

	//2. Configure the bus config
	if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_FULL_DUPLEX) {
		//bidi mode should be cleared
		temp &= ~(1 << 15);
	} else if (pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_HALF_DUPLEX) {
		//bidi mode should be set
		temp |= (1 << 15);
	} else if (pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_RX_SIMPLEX) {
		//bidi mode should be cleared
		temp &= ~(1 << 15);
		//rx only bit must be set
		temp |= (1 << 10);
	}

	//3. Configure the spi serial clock speed(baud rate)
	temp |= (pSPIHandle->SPIConfig.SPI_SclkSpeed << SPI_CR1_BR);

	//4. Configure the DFF
	temp |= (pSPIHandle->SPIConfig.SPI_DFF << SPI_CR1_DFF);

	//5. Configure the the CPOL
	temp |= (pSPIHandle->SPIConfig.SPI_CPOL << SPI_CR1_CPOL);

	//6. Configure the CPHA
	temp |= (pSPIHandle->SPIConfig.SPI_CPHA << SPI_CR1_CPHA);

	//7. Configure the SSM
	temp |= (pSPIHandle->SPIConfig.SPI_SSM << SPI_CR1_SSM);

	pSPIHandle->pSPIx->CR1 = temp;
}

/*********************************************************************************
 * @fn				- SPI_DeInit
 *
 * @brief			- This function de-initializes spi peripheral
 *
 * @param[in]		- Base address of configuration structure for spi peripheral
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- none
 *
 * @note			- none
 *
 */
void SPI_DeInit(SPI_RegDef_t *pSPIx)
{
	if(pSPIx == SPI1) {

		SPI1_REG_RESET();

	} else if(pSPIx == SPI2) {

		SPI2_REG_RESET();

	} else if(pSPIx == SPI3) {

		SPI3_REG_RESET();

	} else if(pSPIx == SPI4) {

		SPI4_REG_RESET();

	}
}



/*********************************************************************************
 * @fn				- SPI_GetFlagStatus
 *
 * @brief			- This function returns value of different SPI flags
 *
 * @param[in]		- Base address of configuration structure for spi peripheral
 * @param[in]		- name of flag
 * @param[in]		-
 *
 * @return			- 1 or 0(uint8_t)
 *
 * @note			- none
 *
 */
uint8_t SPI_GetFlagStatus(SPI_RegDef_t *pSPIx, uint32_t FlagName)
{
	if(pSPIx->SR & FlagName) {
		return FLAG_SET;
	}
	return FLAG_RESET;
}


/*Data Send and Receive*/

/*********************************************************************************
 * @fn				- SPI_SendData
 *
 * @brief			- This function sends the data via spi peripheral
 *
 * @param[in]		- Base address of spi peripheral
 * @param[in]		- pointer to TX buffer
 * @param[in]		- length(size) of data
 *
 * @return			- none
 *
 * @note			- this is a blocking call
 *
 */
void SPI_SendData(SPI_RegDef_t *pSPIx, uint8_t *pTxBuffer, uint32_t Len)
{
	while(Len > 0)
	{
		//1. Wait until TXE is set
		while(SPI_GetFlagStatus(pSPIx, SPI_TXE_FLAG) == RESET);

		//2. check the DFF bit in CR1
		if(pSPIx->CR1 & (1 << SPI_CR1_DFF) ) {
			//16 bit DFF

			//1. load the data into the DR
			pSPIx->DR = *((uint16_t*)pTxBuffer);

			//2.Decrement Length 2 times
			Len--;
			Len--;

			pTxBuffer += 2;
		} else {
			//8 bit DFF

			//1. load the data into the DR
			pSPIx->DR = *pTxBuffer;

			//2. Decrement Length
			Len--;

			pTxBuffer++;
		}
	}
}

/*********************************************************************************
 * @fn				- SPI_SendData
 *
 * @brief			- This function receives the data via spi peripheral
 *
 * @param[in]		- Base address of spi peripheral
 * @param[in]		- pointer to RX buffer
 * @param[in]		- length(size) of data
 *
 * @return			- none
 *
 * @note			- none
 *
 */
void SPI_ReceiveData(SPI_RegDef_t *pSPIx, uint8_t *pRxBuffer, uint32_t Len)
{
	while(Len > 0)
	{
		//1. wait until RX buffer is non-empty
		while(SPI_GetFlagStatus(SPI2, SPI_RXNE_FLAG) == RESET);

		//2. check the DFF bit
		if(pSPIx->CR1 & (1 << SPI_CR1_DFF)) {

			//16 bit
			//1.load the data from DR to RX buffer address
			*((uint16_t*)pRxBuffer) = pSPIx->DR;

			Len--;
			Len--;

			pRxBuffer += 2;

		} else {

			//8bit
			//1.load the data from DR to RX buffer address
			*pRxBuffer = pSPIx->DR;

			Len--;

			pRxBuffer++;

		}
	}
}



/*IRQ configuration and ISR handling*/

/*********************************************************************************
 * @fn				- SPI_IRQInterruptConfig
 *
 * @brief			- This function configures the IRQ
 *
 * @param[in]		- IRQ number
 * @param[in]		- ENABLE or DISABLE macro
 * @param[in]		-
 *
 * @return			- none
 *
 * @note			- none
 *
 */
void SPI_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnDi)
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
 * @fn				- SPI_IRQPriorityConfig
 *
 * @brief			- This function configures the priority of IRQ
 *
 * @param[in]		- IRQ number
 * @param[in]		- IRQ priority
 * @param[in]		-
 *
 * @return			- none
 *
 * @note			- none
 *
 */
void SPI_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
	//1. find out ipr reg
	uint8_t iprx = IRQNumber / 4;
	uint8_t iprx_sec = IRQNumber % 4;

	uint8_t shift_value = (iprx_sec * 8) + ( 8 - NO_PR_BITS_IMPLEMENTED );

	*(NVIC_PR_BASE_ADDR + (iprx * 4)) |= ( IRQPriority << shift_value );
}


/*********************************************************************************
 * @fn				- SPI_PeripheralCtrl
 *
 * @brief			- This function enables or disables SPIx communication
 *
 * @param[in]		- Base address of SPIx peripheral
 * @param[in]		- ENABLE or DISABLE macros
 * @param[in]		-
 *
 * @return			- none
 *
 * @note			- this function must be used only after configuration of SPIx parameters
 *
 */
void SPI_PeripheralCtrl(SPI_RegDef_t *pSPIx, uint8_t EnDi)
{
	if(EnDi == ENABLE) {

		pSPIx->CR1 |= (1 << SPI_CR1_SPE);

	} else if(EnDi == DISABLE) {

		pSPIx->CR1 &= ~(1 << SPI_CR1_SPE);

	}
}



/*********************************************************************************
 * @fn				- SPI_SSIConfig
 *
 * @brief			- This function configures SSI bit
 *
 * @param[in]		- Base address of SPIx peripheral
 * @param[in]		- ENABLE or DISABLE macros
 * @param[in]		-
 *
 * @return			- none
 *
 * @note			- none
 *
 */
void SPI_SSIConfig(SPI_RegDef_t *pSPIx, uint8_t EnDi)
{
	if(EnDi == ENABLE) {

		pSPIx->CR1 |= (1 << SPI_CR1_SSI);

	} else if(EnDi == DISABLE) {

		pSPIx->CR1 &= ~(1 << SPI_CR1_SSI);

	}
}


/*********************************************************************************
 * @fn				- SPI_SSOEConfig
 *
 * @brief			- This function configures SSOE bit
 *
 * @param[in]		- Base address of SPIx peripheral
 * @param[in]		- ENABLE or DISABLE macros
 * @param[in]		-
 *
 * @return			- none
 *
 * @note			- none
 *
 */
void SPI_SSOEConfig(SPI_RegDef_t *pSPIx, uint8_t EnDi)
{
	if(EnDi == ENABLE) {

		pSPIx->CR2 |= (1 << SPI_CR2_SSOE);

	} else if(EnDi == DISABLE) {

		pSPIx->CR2 &= ~(1 << SPI_CR2_SSOE);

	}
}


/*********************************************************************************
 * @fn				- SPI_SendDataIT
 *
 * @brief			- This function sends the data via spi peripheral using interrupt
 *
 * @param[in]		- Base address of spi handle structure
 * @param[in]		- pointer to TX buffer
 * @param[in]		- length(size) of data
 *
 * @return			- uint8_t(state)
 *
 * @note			- non-blocking call
 *
 */
uint8_t SPI_SendDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pTxBuffer, uint32_t Len)
{
	uint8_t state = pSPIHandle->TxState;

	if(state != SPI_BUSY_IN_TX)
	{
		//1. Save the TX buffer address and Len information in some global variables
		pSPIHandle->pTxBuffer = pTxBuffer;
		pSPIHandle->TxLen = Len;

		//2. Mark the SPI state as busy in transmission so that no other code can take
		//   over same SPI peripheral until transmission is over
		pSPIHandle->TxState = SPI_BUSY_IN_TX;

		//3. Enable the TXEIE control bit to get interrupt whenever TXE flag is set in SR
		pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_TXEIE);

		//4. Data Transmission will be handled by the ISR code
	}

	return state;
}


/*********************************************************************************
 * @fn				- SPI_ReceiveDataIT
 *
 * @brief			- This function receives the data via spi peripheral using interrupt
 *
 * @param[in]		- Base address of spi handle structure
 * @param[in]		- pointer to TX buffer
 * @param[in]		- length(size) of data
 *
 * @return			- none
 *
 * @note			- none
 *
 */
uint8_t SPI_ReceiveDataIT(SPI_Handle_t *pSPIHandle, uint8_t *pRxBuffer, uint32_t Len)
{
	uint8_t state = pSPIHandle->RxState;

	if(state != SPI_BUSY_IN_RX)
	{
		//1. Save the TX buffer address and Len information in some global variables
		pSPIHandle->pRxBuffer = pRxBuffer;
		pSPIHandle->RxLen = Len;

		//2. Mark the SPI state as busy in transmission so that no other code can take
		//   over same SPI peripheral until transmission is over
		pSPIHandle->RxState = SPI_BUSY_IN_RX;

		//3. Enable the TXEIE control bit to get interrupt whenever TXE flag is set in SR
		pSPIHandle->pSPIx->CR2 |= (1 << SPI_CR2_RXNEIE);

		//4. Data Transmission will be handled by the ISR code
	}

	return state;

}



/*********************************************************************************
 * @fn				- SPI_IRQHandling
 *
 * @brief			- This function configures the IRQ handling
 *
 * @param[in]		- Base address of SPI peripheral
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- none
 *
 * @note			- none
 *
 */
void SPI_IRQHandling(SPI_Handle_t *pSPIHandle)
{
	uint32_t temp1, temp2;

	//check for TXE
	temp1 = pSPIHandle->pSPIx->SR & (1 << SPI_SR_TXE);
	temp2 = pSPIHandle->pSPIx->CR2 & (1 << SPI_CR2_TXEIE);

	if(temp1 && temp2)
	{
		//handle TXE
		spi_txe_interrupt_handle(pSPIHandle);
	}

	//check for RXNE
	temp1 = pSPIHandle->pSPIx->SR & (1 << SPI_SR_RXNE);
	temp2 = pSPIHandle->pSPIx->CR2 & (1 << SPI_CR2_RXNEIE);

	if(temp1 && temp2)
	{
		//handle RXNE
		spi_rxne_interrupt_handle(pSPIHandle);
	}

	//check for ovr flag
	temp1 = pSPIHandle->pSPIx->SR & (1 << SPI_SR_OVR);
	temp2 = pSPIHandle->pSPIx->CR2 & (1 << SPI_CR2_ERRIE);

	if(temp1 && temp2)
	{
		//handle ovr error
		spi_ovr_interrupt_handle(pSPIHandle);
	}
}


/*********************************************************************************
 * @fn				- SPI_ClearOVRFlag
 *
 * @brief			- This function clears the Overrun flag
 *
 * @param[in]		- Base address of SPI peripheral
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- none
 *
 * @note			- none
 *
 */
void SPI_ClearOVRFlag(SPI_RegDef_t *pSPIx)
{
	uint8_t temp;

	temp = pSPIx->DR;
	temp = pSPIx->SR;

	(void)temp;
}


/*********************************************************************************
 * @fn				- SPI_CloseTransmission
 *
 * @brief			- This function closes SPI transmission
 *
 * @param[in]		- Base address of spi handle structure
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- none
 *
 * @note			- none
 *
 */
void SPI_CloseTransmission(SPI_Handle_t *pSPIHandle)
{
	//this prevents interrupts from setting up of TXE flag
	pSPIHandle->pSPIx->CR2 &= ~(1 << SPI_CR2_TXEIE);

	//reset TX buffer
	pSPIHandle->pTxBuffer = NULL;
	//reset length
	pSPIHandle->TxLen = 0;
	//reset state
	pSPIHandle->TxState = SPI_READY;
}


/*********************************************************************************
 * @fn				- SPI_CloseReceprion
 *
 * @brief			- This function closes SPI reception
 *
 * @param[in]		- Base address of spi handle structure
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- none
 *
 * @note			- none
 *
 */
void SPI_CloseReception(SPI_Handle_t *pSPIHandle)
{
	//this prevents interrupts from setting up of TXE flag
	pSPIHandle->pSPIx->CR2 &= ~(1 << SPI_CR2_RXNEIE);

	//reset TX buffer
	pSPIHandle->pRxBuffer = NULL;
	//reset length
	pSPIHandle->RxLen = 0;
	//reset state
	pSPIHandle->RxState = SPI_READY;
}


/*********************************************************************************
 * @fn				- SPI_ApplicationEventCallback
 *
 * @brief			- This function does application callback
 *
 * @param[in]		- Base address of spi handle structure
 * @param[in]		- type of event macros
 * @param[in]		-
 *
 * @return			- none
 *
 * @note			- weak implementation
 *
 */
__attribute__((weak)) void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle, uint8_t AppEv)
{
	//this is weak implementation | user application may overwrite this function
}

/*
 * Helper functions implementations
 */
static void spi_txe_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
	// check the DFF bit in CR1
	if(pSPIHandle->pSPIx->CR1 & (1 << SPI_CR1_DFF) ) {
		//16 bit DFF

		//1. load the data into the DR
		pSPIHandle->pSPIx->DR = *((uint16_t*)pSPIHandle->pTxBuffer);

		//2.Decrement Length 2 times
		pSPIHandle->TxLen -= 2;

		pSPIHandle->pTxBuffer += 2;
	} else {
		//8 bit DFF

		//1. load the data into the DR
		pSPIHandle->pSPIx->DR = *(pSPIHandle->pTxBuffer);

		//2.Decrement Length
		pSPIHandle->TxLen --;

		pSPIHandle->pTxBuffer ++;
	}

	if(! pSPIHandle->TxLen)
	{
		//TxLen is zero, so close the spi transmission ond inform application that TX is over
		SPI_CloseTransmission(pSPIHandle);

		SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_TX_CMPLT);
	}
}


static void spi_rxne_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
	// check the DFF bit
	if(pSPIHandle->pSPIx->CR1 & (1 << SPI_CR1_DFF)) {

		//16 bit
		//1.load the data from DR to RX buffer address
		*((uint16_t*)pSPIHandle->pRxBuffer) = (uint16_t)pSPIHandle->pSPIx->DR;

		pSPIHandle->RxLen -=2;

		pSPIHandle->pRxBuffer += 2;

	} else {

		//8bit
		//1.load the data from DR to RX buffer address
		*(pSPIHandle->pRxBuffer) = pSPIHandle->pSPIx->DR;

		pSPIHandle->RxLen --;

		pSPIHandle->pRxBuffer ++;

	}

	if(! pSPIHandle->RxLen)
	{
		//RxLen is zero, so close the spi transmission and inform the application that RX is over

		SPI_CloseReception(pSPIHandle);

		SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_RX_CMPLT);
	}
}


static void spi_ovr_interrupt_handle(SPI_Handle_t *pSPIHandle)
{
	uint8_t temp;

	//1. Clear the OVR flag
	if(pSPIHandle->TxState != SPI_BUSY_IN_TX)
	{
		temp = pSPIHandle->pSPIx->DR;
		temp = pSPIHandle->pSPIx->SR;
	}
	(void)temp;

	//2. Inform the application
	SPI_ApplicationEventCallback(pSPIHandle, SPI_EVENT_OVR_ERR);
}

