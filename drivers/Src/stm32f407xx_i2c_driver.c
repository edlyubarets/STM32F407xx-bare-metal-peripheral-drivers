/*
 * stm32f407xx_i2c_driver.c
 *
 *  Created on: Jul 30, 2026
 *      Author: dell
 */

#include "stm32f407xx_usart_driver.h"


/* Private helper functions prototypes */
static void I2C_ExecuteAddressPhase(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr, uint8_t WRorRE);
static void I2C_ClearADDRFlag(I2C_Handle_t *pI2CHandle);
static void I2C_AckManagement(I2C_RegDef_t *pI2Cx, uint8_t EnDi);
static void I2C_MasterHandleRXNEInterrupt(I2C_Handle_t *pI2CHandle);
static void I2C_MasterHandleTXEInterrupt(I2C_Handle_t *pI2CHandle);


#define I2C_WRITE		0
#define I2C_READ		1
#define ACK_ENABLE 		1
#define ACK_DISABLE 	0





/*Peripheral clock setup*/

/*********************************************************************************
 * @fn				- I2C_PClkCtrl
 *
 * @brief			- This function enables or disables peripheral clock for I2C peripheral
 *
 * @param[in]		- base address of i2c peripheral
 * @param[in]		- ENABLE or DISABLE macros
 * @param[in]		-
 *
 * @return			- none
 *
 * @note			- none
 *
 */
void I2C_PClkCtrl(I2C_RegDef_t *pI2Cx, uint8_t EnDi)
{
	if(EnDi == ENABLE) {

		if(pI2Cx == I2C1) {
			I2C1_PCLK_EN();
		} else if(pI2Cx == I2C2) {
			I2C2_PCLK_EN();
		} else if(pI2Cx == I2C3) {
			I2C3_PCLK_EN();
		}

	} else {

		if(pI2Cx == I2C1) {
			I2C1_PCLK_DI();
		} else if(pI2Cx == I2C2) {
			I2C2_PCLK_DI();
		} else if(pI2Cx == I2C3) {
			I2C3_PCLK_DI();
		}
	}
}



/*initialization & de-initialization*/

/*********************************************************************************
 * @fn				- I2C_Init
 *
 * @brief			- This function initiates the I2C peripheral
 *
 * @param[in]		- base address of i2c handle structure
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- none
 *
 * @note			- none
 *
 */
void I2C_Init(I2C_Handle_t *pI2CHandle)
{
	uint32_t temp = 0;

	//enable the clock
	I2C_PClkCtrl(pI2CHandle->pI2Cx, ENABLE);

	//ack control bit
	temp |= (pI2CHandle->I2C_Config.ACKControl << I2C_CR1_ACK);
	pI2CHandle->pI2Cx->CR1 = temp;

	//configure the frequency field of CR2
	temp = 0;
	temp |= RCC_GetPCLK1Value() / 1000000U ;
	pI2CHandle->pI2Cx->CR2 = temp & 0x3F;

	//configure the device own address
	temp |= ( pI2CHandle->I2C_Config.I2C_DeviceAddress << 1 );
	temp |= (1 << 14);	//14th bit should always be kept at 1 by software according to reference manual
	pI2CHandle->pI2Cx->OAR1 = temp;

	//CCR calculations
	uint16_t ccr_value = 0;
	temp = 0;
	if(pI2CHandle->I2C_Config.I2C_SCLSpeed <= I2C_SCL_SPEED_STANDARD) {
		//speed mode is standard mode
		ccr_value = ( RCC_GetPCLK1Value() / (2 * pI2CHandle->I2C_Config.I2C_SCLSpeed) );
		temp |= (ccr_value & 0xFFF);
	} else {
		//speed mode is fast mode
		temp |= (1 << I2C_CCR_F_S);
		temp |= (pI2CHandle->I2C_Config.I2C_FMDutyCycle << I2C_CCR_DUTY);
		if(pI2CHandle->I2C_Config.I2C_FMDutyCycle == I2C_FM_DUTY_2) {

			ccr_value = ( RCC_GetPCLK1Value() / (3 * pI2CHandle->I2C_Config.I2C_SCLSpeed) );

		} else if(pI2CHandle->I2C_Config.I2C_FMDutyCycle == I2C_FM_DUTY_16_9) {

			ccr_value = ( RCC_GetPCLK1Value() / (25 * pI2CHandle->I2C_Config.I2C_SCLSpeed) );
		}

		temp |= (ccr_value & 0xFFF);
	}

	pI2CHandle->pI2Cx->CCR = temp;

	//TRISE configuration
	if(pI2CHandle->I2C_Config.I2C_SCLSpeed <= I2C_SCL_SPEED_STANDARD) {

		//speed mode is standard mode
		temp = (RCC_GetPCLK1Value() / 1000000U) + 1;

	} else {

		//speed mode is fast mode
		temp = (RCC_GetPCLK1Value() * 300 / 1000000000U) + 1;
	}

	pI2CHandle->pI2Cx->TRISE = temp & 0x3F;
}

/*********************************************************************************
 * @fn				- I2C_PClkCtrl
 *
 * @brief			- This function de-initiates the I2C peripheral
 *
 * @param[in]		- base address of i2c peripheral
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- none
 *
 * @note			- none
 *
 */
void I2C_DeInit(I2C_RegDef_t *pI2Cx)
{
	if(pI2Cx == I2C1) {

		I2C1_REG_RESET();

	} else if(pI2Cx == I2C2) {

		I2C2_REG_RESET();

	} else if(pI2Cx == I2C3) {

		I2C3_REG_RESET();

	}
}




/*Data Send and Receive*/
/*********************************************************************************
 * @fn				- I2C_MasterSendData
 *
 * @brief			- This function sends the data in Master mode
 *
 * @param[in]		- base address of i2c handle structure
 * @param[in]		- base address of TX buffer
 * @param[in]		- Length of TX buffer
 * @param[in]		- Address of slave to receive the data
 *
 * @return			- none
 *
 * @note			- this is a blocking call
 *
 */
void I2C_MasterSendData(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint32_t Len, uint8_t SlaveAddr, uint8_t Sr)
{
	//1. Generate start condition
	I2C_GenStartCon(pI2CHandle->pI2Cx);

	//2. Confirm that start generation is completed by checking the SB flag in SR1 reg
	//	Note: until SB is cleared SCL will be stretched(pulled to low)
	while(! ( I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_SB) ) );

	//3. Send the address of the slave with r/nw bit set to w(0) (total 8 bits)
	I2C_ExecuteAddressPhase(pI2CHandle->pI2Cx, SlaveAddr, I2C_WRITE);

	//4. Confirm that address phase is completed by checking the ADDR in SR1
	while( ! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_ADDR));

	//5. Clear the ADDR flag according to its software sequence
	// Note: Until ADDR is cleared SCL will be stretched(pulled to low)
	I2C_ClearADDRFlag(pI2CHandle);

	//6. Send the data until Len becomes 0
	while(Len > 0)
	{
		while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_TXE)); //wait till TXE is set
		pI2CHandle->pI2Cx->DR = *pTxBuffer;
		pTxBuffer++;
		Len--;
	}

	//7. When Len becomes 0 wait for TXE=1 and BTF=1 before generating STOP condition
	// Note: TXE=1 and BTF=1 means that both SR and DR regs are empty and next transmission should begin
	// Note: when BTF=1 SCL will be stretched(pulled to LOW)
	while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_TXE));
	while(! I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_BTF));

	//8. Generate STOP condition and master need not to wait for the compltion of stop condition.
	// Note: generating STOP automatically clears the BTF
	if(Sr == I2C_SR_DI) {
		I2C_GenStopCon(pI2CHandle->pI2Cx);
	}
}


/*********************************************************************************
 * @fn				- I2C_MasterReceiveData
 *
 * @brief			- This function receives the data in Master mode
 *
 * @param[in]		- base address of i2c handle structure
 * @param[in]		- base address of TX buffer
 * @param[in]		- Length of TX buffer
 * @param[in]		- Address of slave to receive the data
 *
 * @return			- none
 *
 * @note			- this is a blocking call
 *
 */
void I2C_MasterReceiveData(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer, uint32_t Len, uint8_t SlaveAddr, uint8_t Sr)
{
	//1. Generate the START Condition
	I2C_GenStartCon(pI2CHandle->pI2Cx);

	//2. Confirm that start generation is completed by checking the SB flag in the SR1
	// Note: Until SB is cleared SCL will be stretched(pulled to low)
	while( ! (I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_SB)));

	//3. Send the address of the slave with r/nw bit set to R(1) (total 8 bits)
	I2C_ExecuteAddressPhase(pI2CHandle->pI2Cx, SlaveAddr, I2C_READ);

	//4. wait until address is completed by checking the ADDR flag in SR1
	while( ! (I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_ADDR)));

	//procedure to read only 1 byte from slave
	if(Len == 1)
	{
		//Disable Acking
		I2C_AckManagement(pI2CHandle->pI2Cx, I2C_ACK_DISABLE);

		//clear the ADDR flag
		I2C_ClearADDRFlag(pI2CHandle);

		//wait until RXNE becomes 1
		while( ! (I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_RXNE)));

		//generate STOP condition
		if(Sr == I2C_SR_DI) {
			I2C_GenStopCon(pI2CHandle->pI2Cx);
		}

		//read data into buffer
		*pRxBuffer = pI2CHandle->pI2Cx->DR;

	}

	//procedure to read data from slave when Len > 1
	if(Len > 1)
	{
		//clear the ADDR flag
		I2C_ClearADDRFlag(pI2CHandle);

		//read the data until Len becomes zero
		for(uint32_t i = Len ; i > 0 ; i--)
		{
			//wait until RXNE becomes 1
			while( ! (I2C_GetFlagStatus(pI2CHandle->pI2Cx, I2C_FLAG_RXNE)));

			if(i == 2) //if last 2 bytes are remaining
			{
				//disable acking
				I2C_AckManagement(pI2CHandle->pI2Cx, I2C_ACK_DISABLE);

				//generate STOP condition
				if(Sr == I2C_SR_DI) {
					I2C_GenStopCon(pI2CHandle->pI2Cx);
				}
			}

			//read the data from data register into buffer
			*pRxBuffer = pI2CHandle->pI2Cx->DR;

			//increment buffer address
			pRxBuffer++;
		}
	}

	//re-enable acking
	I2C_AckManagement(pI2CHandle->pI2Cx, I2C_ACK_ENABLE);

}


/*********************************************************************************
 * @fn				- I2C_SlaveSendData
 *
 * @brief			- This function sends the data in Slave mode
 *
 * @param[in]		- base address of i2c handle structure
 * @param[in]		- byte of data
 * @param[in]		-
 *
 * @return			- none
 *
 * @note			- none
 *
 */
void I2C_SlaveSendData(I2C_RegDef_t *pI2Cx, uint8_t data)
{
	pI2Cx->DR = data;
}


/*********************************************************************************
 * @fn				- I2C_SlaveReceiveData
 *
 * @brief			- This function receives the data in Slave mode
 *
 * @param[in]		- base address of i2c handle structure
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- none
 *
 * @note			- none
 *
 */
uint8_t I2C_SlaveReceiveData(I2C_RegDef_t *pI2Cx)
{
	return (uint8_t)pI2Cx->DR;
}


/*Interrupt data send and receive*/

/*********************************************************************************
 * @fn				- I2C_MasterSendDataIT
 *
 * @brief			- This function sends the data in Master mode using interrupt
 *
 * @param[in]		- base address of i2c handle structure
 * @param[in]		- base address of TX buffer
 * @param[in]		- Length of TX buffer
 * @param[in]		- Address of slave to receive the data
 *
 * @return			- none
 *
 * @note			- none
 *
 */
uint8_t I2C_MasterSendDataIT(I2C_Handle_t *pI2CHandle, uint8_t *pTxBuffer, uint32_t Len, uint8_t SlaveAddr, uint8_t Sr)
{
	uint8_t busystate = pI2CHandle->TxRxState;

	if( (busystate != I2C_BUSY_IN_TX) && (busystate != I2C_BUSY_IN_RX))
	{
		pI2CHandle->pTxBuffer = pTxBuffer;
		pI2CHandle->TxLen = Len;
		pI2CHandle->TxRxState = I2C_BUSY_IN_TX;
		pI2CHandle->DevAddr = SlaveAddr;
		pI2CHandle->Sr = Sr;

		//Generate START Condition
		I2C_GenStartCon(pI2CHandle->pI2Cx);

		//Enable ITBUFEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITBUFEN);

		//Enable ITEVFEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITEVTEN);

		//Enable ITERREN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITERREN);

	}

	return busystate;
}

/*********************************************************************************
 * @fn				- I2C_MasterReceiveDataIT
 *
 * @brief			- This function receives the data in Master mode using interrupt
 *
 * @param[in]		- base address of i2c handle structure
 * @param[in]		- base address of TX buffer
 * @param[in]		- Length of TX buffer
 * @param[in]		- Address of slave to receive the data
 *
 * @return			- none
 *
 * @note			- none
 *
 */
uint8_t I2C_MasterReceiveDataIT(I2C_Handle_t *pI2CHandle, uint8_t *pRxBuffer, uint32_t Len, uint8_t SlaveAddr, uint8_t Sr)
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

		//Generate START Condition
		I2C_GenStartCon(pI2CHandle->pI2Cx);

		//Enable ITBUFEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITBUFEN);

		//Enable ITEVFEN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITEVTEN);

		//Enable ITERREN Control Bit
		pI2CHandle->pI2Cx->CR2 |= ( 1 << I2C_CR2_ITERREN);

	}

	return busystate;
}


/*IRQ configuration and ISR handling*/

/*********************************************************************************
 * @fn				- I2C_IRQInterruptConfig
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
void I2C_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnDi)
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
 * @fn				- I2C_IRQPriorityConfig
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
void I2C_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
	//1. find out ipr reg
	uint8_t iprx = IRQNumber / 4;
	uint8_t iprx_sec = IRQNumber % 4;

	uint8_t shift_value = (iprx_sec * 8) + ( 8 - NO_PR_BITS_IMPLEMENTED );

	*(NVIC_PR_BASE_ADDR + (iprx * 4)) |= ( IRQPriority << shift_value );
}

/*********************************************************************************
 * @fn				- I2C_EV_IRQHandling
 *
 * @brief			- This function handles the I2C Events interrupts
 *
 * @param[in]		- I2C Handle structure
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- none
 *
 * @note			- none
 *
 */
void I2C_EV_IRQHandling(I2C_Handle_t *pI2CHandle)
{
	//Interrupt handling for both master and slave  mode of the device

	uint32_t temp1, temp2, temp3;

	temp1 = pI2CHandle->pI2Cx->CR2 & (1 << I2C_CR2_ITERREN);
	temp2 = pI2CHandle->pI2Cx->CR2 & (1 << I2C_CR2_ITBUFEN);

	temp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_SB);
	//1. Handle for the interrupt generated by SB event
	// Note : SB flag is only applicable in Master mode
	if(temp1 && temp3)
	{
		//the interrupt is generated because of SB event
		//Note : this block will not be executed in slave mode
		//       because for slave SB is always zero.

		//in this block lets execute address phase
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX)
		{
			I2C_ExecuteAddressPhase(pI2CHandle->pI2Cx, pI2CHandle->DevAddr, I2C_WRITE);

		} else if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
		{
			I2C_ExecuteAddressPhase(pI2CHandle->pI2Cx, pI2CHandle->DevAddr, I2C_READ);
		}
	}


	temp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_ADDR);
	//2. Handle for the interrupt  generated by ADDR event
	// Note : When Master mode : Address is sent
	//        when Slave mode : Address matched with own address
	if(temp1 && temp3)
	{
		//ADDR flag is set
		I2C_ClearADDRFlag(pI2CHandle);
	}

	temp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_BTF);
	//3. Handle for interrupt generated by BTF(Byte Transfer Finished) event
	if(temp1 && temp3)
	{
		//BTF flag is set

		if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX)
		{
			//make sure that TXE is also set
			if(pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_TXE))
			{
				//BTF, TXE = 1

				if(pI2CHandle->TxLen == 0)
				{
					//1. Generate STOP condition
					if(pI2CHandle->Sr == I2C_SR_DI) {
						I2C_GenStopCon(pI2CHandle->pI2Cx);
					}

					//2. reset all the member elements of handle structure
					I2C_CloseSendData(pI2CHandle);

					//3. notify the application about transmission complete
					I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_TX_CMPLT);
				}
			}
		} else if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
		{
			;
		}
	}

	temp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_STOPF);
	//4. Handle for interrupt generated by STOPF event
	// Note : Stop detection flag is only applicable in slave mode. For master this flag will never be set.
	if(temp1 && temp3)
	{
		//STOPF flag is set
		//clear the STOP ( i.e 1)Read SR1 2)Write to CR1 )

		pI2CHandle->pI2Cx->CR1 |= 0x0000;

		//notify the application that STOP is detected
		I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_STOP);
	}

	temp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_TXE);
	//5. Handle for interrupt generated by TXE event
	if(temp1 && temp2 && temp3)
	{
		//TXE flag is set
		//data transmission
		if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_MSL))		//check for device mode
		{
			//the device is master
			if(pI2CHandle->TxRxState == I2C_BUSY_IN_TX)
			{
				I2C_MasterHandleTXEInterrupt(pI2CHandle);
			}

		} else
		{
			//the device is slave
			//make sure that slave is in TX mode
			if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_TRA))
			{
				I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_DATA_REQ);
			}
		}
	}

	temp3 = pI2CHandle->pI2Cx->SR1 & (1 << I2C_SR1_RXNE);
	//6. Handle for interrupt generated by RXNE event
	if(temp1 && temp2 && temp3)
	{
		//the device is Master

		//RXNE flag is set
		if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_MSL))		//check for device mode
		{
			//The device is Master

			if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
			{
				I2C_MasterHandleRXNEInterrupt(pI2CHandle);
			}
		} else
		{
			//the device is slave
			//make sure that the slave is in RX mode
			if( ! (pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_TRA)))
			{
				I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_DATA_RCV);
			}
		}
	}

}

/*********************************************************************************
 * @fn				- I2C_ER_IRQHandling
 *
 * @brief			- This function handles the I2C Errors interrupts
 *
 * @param[in]		- I2C Handle structure
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- none
 *
 * @note			- none
 *
 */
void I2C_ER_IRQHandling(I2C_Handle_t *pI2CHandle)
{

	uint32_t temp1,temp2;

    //Know the status of  ITERREN control bit in the CR2
	temp2 = (pI2CHandle->pI2Cx->CR2) & ( 1 << I2C_CR2_ITERREN);


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
		pI2CHandle->pI2Cx->SR1 &= ~(1 << I2C_SR1_ARLO);

		//Implement the code to notify the application about the error
	   I2C_ApplicationEventCallback(pI2CHandle, I2C_ERROR_ARLO);
	}

/***********************Check for ACK failure  error************************************/

	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_AF);
	if(temp1  && temp2)
	{
		//This is ACK failure error

	    //Implement the code to clear the ACK failure error flag
		pI2CHandle->pI2Cx->SR1 &= ~(1 << I2C_SR1_AF);

		//Implement the code to notify the application about the error
		I2C_ApplicationEventCallback(pI2CHandle, I2C_ERROR_AF);
	}

/***********************Check for Overrun/underrun error************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_OVR);
	if(temp1  && temp2)
	{
		//This is Overrun/underrun

	    //Implement the code to clear the Overrun/underrun error flag
		pI2CHandle->pI2Cx->SR1 &= ~(1 << I2C_SR1_OVR);

		//Implement the code to notify the application about the error
		I2C_ApplicationEventCallback(pI2CHandle, I2C_ERROR_OVR);
	}

/***********************Check for Time out error************************************/
	temp1 = (pI2CHandle->pI2Cx->SR1) & ( 1 << I2C_SR1_TIMEOUT);
	if(temp1  && temp2)
	{
		//This is Time out error

	    //Implement the code to clear the Time out error flag
		pI2CHandle->pI2Cx->SR1 &= ~(1 << I2C_SR1_TIMEOUT);

		//Implement the code to notify the application about the error
		I2C_ApplicationEventCallback(pI2CHandle, I2C_ERROR_TIMEOUT);
	}
}



/*********************************************************************************
 * @fn				- I2C_CloseSendData
 *
 * @brief			- This function closes data transmission
 *
 * @param[in]		- I2C Handle structure
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- none
 *
 * @note			- none
 *
 */
void I2C_CloseSendData(I2C_Handle_t *pI2CHandle)
{
	//disable ITBUFEN control bit
	pI2CHandle->pI2Cx->CR2 &= ~(1 << I2C_CR2_ITBUFEN);

	//disable ITEVTEN control bit
	pI2CHandle->pI2Cx->CR2 &= ~(1 << I2C_CR2_ITEVTEN);

	pI2CHandle->TxRxState = I2C_READY;
	pI2CHandle->pRxBuffer = NULL;
	pI2CHandle->RxLen = 0;
	pI2CHandle->RxSize = 0;

	if(pI2CHandle->I2C_Config.ACKControl == I2C_ACK_ENABLE)
	{
		I2C_AckManagement(pI2CHandle->pI2Cx, ENABLE);
	}
}


/*********************************************************************************
 * @fn				- CloseReceiveData
 *
 * @brief			- This function closes data reception
 *
 * @param[in]		- I2C Handle structure
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- none
 *
 * @note			- none
 *
 */
void I2C_CloseReceiveData(I2C_Handle_t *pI2CHandle)
{
	//disable ITBUFEN control bit
	pI2CHandle->pI2Cx->CR2 &= ~(1 << I2C_CR2_ITBUFEN);

	//disable ITEVTEN control bit
	pI2CHandle->pI2Cx->CR2 &= ~(1 << I2C_CR2_ITEVTEN);

	pI2CHandle->TxRxState = I2C_READY;
	pI2CHandle->pTxBuffer = NULL;
	pI2CHandle->TxLen = 0;

	if(pI2CHandle->I2C_Config.ACKControl == I2C_ACK_ENABLE)
	{
		I2C_AckManagement(pI2CHandle->pI2Cx, ENABLE);
	}
}



/*Application CallBack*/

/*********************************************************************************
 * @fn				- I2C_ApplicationEventCallback
 *
 * @brief			- This function does application callback
 *
 * @param[in]		- Base address of i2c handle structure
 * @param[in]		- type of event macros
 * @param[in]		-
 *
 * @return			- none
 *
 * @note			- weak implementation
 *
 */
__attribute__((weak)) void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle, uint8_t AppEv)
{

}



/*Other peripheral Control API's*/

/*********************************************************************************
 * @fn				- I2C_PeripheralCtrl
 *
 * @brief			- This function enables or disables I2Cx communication
 *
 * @param[in]		- Base address of I2Cx peripheral
 * @param[in]		- ENABLE or DISABLE macros
 * @param[in]		-
 *
 * @return			- none
 *
 * @note			- this function must be used only after configuration of I2Cx parameters
 *
 */
void I2C_PeripheralCtrl(I2C_RegDef_t *pI2Cx, uint8_t EnDi)
{
	if(EnDi == ENABLE) {

		pI2Cx->CR1 |= (1 << I2C_CR1_PE);

		//enable acking
		I2C_AckManagement(pI2Cx, I2C_ACK_ENABLE);

	} else if(EnDi == DISABLE) {

		pI2Cx->CR1 &= ~(1 << I2C_CR1_PE);

	}
}


/*********************************************************************************
 * @fn				- I2C_GetFlagStatus
 *
 * @brief			- This function returns value of different I2C flags
 *
 * @param[in]		- Base address of configuration structure for i2c peripheral
 * @param[in]		- name of flag
 * @param[in]		-
 *
 * @return			- 1 or 0(uint8_t)
 *
 * @note			- none
 *
 */
uint8_t I2C_GetFlagStatus(I2C_RegDef_t *pI2Cx, uint32_t FlagName)
{
	if(pI2Cx->SR1 & FlagName) {
		return FLAG_SET;
	}
	return FLAG_RESET;
}


/*********************************************************************************
 * @fn				- I2C_GenStartCon
 *
 * @brief			- This function generates I2C START condition
 *
 * @param[in]		- Base address of I2C peripheral
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- none
 *
 * @note			- none
 *
 */
void I2C_GenStartCon(I2C_RegDef_t *pI2Cx)
{
	pI2Cx->CR1 |= (1 << I2C_CR1_START);
}


/*********************************************************************************
 * @fn				- I2C_GenStopCon
 *
 * @brief			- This function generates I2C STOP condition
 *
 * @param[in]		- Base address of I2C peripheral
 * @param[in]		-
 * @param[in]		-
 *
 * @return			- none
 *
 * @note			- none
 *
 */
void I2C_GenStopCon(I2C_RegDef_t *pI2Cx)
{
	pI2Cx->CR1 |= (1 << I2C_CR1_STOP);
}


/*********************************************************************************
 * @fn				- I2C_SlaveEnableDisableCallbackEvents
 *
 * @brief			-
 *
 * @param[in]		- Base address of I2C peripheral
 * @param[in]		- ENABLE or DISABLE macros
 * @param[in]		-
 *
 * @return			- none
 *
 * @note			- none
 *
 */
void I2C_SlaveEnableDisableCallbackEvents(I2C_RegDef_t *pI2Cx, uint8_t EnDi)
{
	if(EnDi == ENABLE)
	{
		pI2Cx->CR2 |= ( 1 << I2C_CR2_ITEVTEN);
		pI2Cx->CR2 |= ( 1 << I2C_CR2_ITBUFEN);
		pI2Cx->CR2 |= ( 1 << I2C_CR2_ITERREN);

	} else
	{
		pI2Cx->CR2 &= ~( 1 << I2C_CR2_ITEVTEN);
		pI2Cx->CR2 &= ~( 1 << I2C_CR2_ITBUFEN);
		pI2Cx->CR2 &= ~( 1 << I2C_CR2_ITERREN);
	}
}



/* Private helper functions definitions*/
static void I2C_ExecuteAddressPhase(I2C_RegDef_t *pI2Cx, uint8_t SlaveAddr, uint8_t WRorRE)
{
	SlaveAddr = SlaveAddr << 1;
	if(WRorRE == I2C_WRITE) {

		SlaveAddr &= ~(1 << 0);

	} else if(WRorRE == I2C_READ) {

		SlaveAddr |= (1 << 0);

	}
	pI2Cx->DR = SlaveAddr;
}


static void I2C_ClearADDRFlag(I2C_Handle_t *pI2CHandle)
{
	uint32_t dummy_read;

	//check for device mode
	if(pI2CHandle->pI2Cx->SR2 & (1 << I2C_SR2_MSL))
	{
		//device is in master mode
		if(pI2CHandle->TxRxState == I2C_BUSY_IN_RX)
		{
			if(pI2CHandle->RxSize == 1)
			{
				//1. disable the ack
				I2C_AckManagement(pI2CHandle->pI2Cx, DISABLE);

				//2. clear the ADDR flag(read SR1, read SR2)
				dummy_read = pI2CHandle->pI2Cx->SR1;
				dummy_read = pI2CHandle->pI2Cx->SR2;
				(void)dummy_read;
			}
		} else {
			//clear the ADDR flag(read SR1, read SR2)
			dummy_read = pI2CHandle->pI2Cx->SR1;
			dummy_read = pI2CHandle->pI2Cx->SR2;
			(void)dummy_read;
		}

	} else {

		//device is in slave mode
		//clear the ADDR flag(read SR1, read SR2)
		dummy_read = pI2CHandle->pI2Cx->SR1;
		dummy_read = pI2CHandle->pI2Cx->SR2;
		(void)dummy_read;
	}

}


static void I2C_AckManagement(I2C_RegDef_t *pI2Cx, uint8_t EnDi)
{
	if(EnDi == ACK_ENABLE) {
		//enable acking
		pI2Cx->CR1 |= (1 << I2C_CR1_ACK);

	} else if(EnDi == ACK_DISABLE) {
		//disable acking
		pI2Cx->CR1 &= ~(1 << I2C_CR1_ACK);
	}
}


static void I2C_MasterHandleRXNEInterrupt(I2C_Handle_t *pI2CHandle)
{
	//do the data reception
	if(pI2CHandle->RxSize == 1)
	{
		*pI2CHandle->pRxBuffer = pI2CHandle->pI2Cx->DR;
		pI2CHandle->RxLen--;
	}

	if(pI2CHandle->RxSize > 1)
	{
		if(pI2CHandle->RxLen == 2)
		{
			//clear the ack bit
			I2C_AckManagement(pI2CHandle->pI2Cx, DISABLE);
		}

		//read DR
		*pI2CHandle->pRxBuffer = pI2CHandle->pI2Cx->DR;
		pI2CHandle->pRxBuffer++;
		pI2CHandle->RxLen--;
	}

	if(pI2CHandle->RxLen == 0)
	{
		//close the I2C data reception and notify the application

		//1. generate stop condition
		if(pI2CHandle->Sr == I2C_SR_DI) {
			I2C_GenStopCon(pI2CHandle->pI2Cx);
		}

		//2. Close the I2C RX
		I2C_CloseReceiveData(pI2CHandle);

		//3. Notify the App
		I2C_ApplicationEventCallback(pI2CHandle, I2C_EV_RX_CMPLT);
	}

}


static void I2C_MasterHandleTXEInterrupt(I2C_Handle_t *pI2CHandle)
{
	if(pI2CHandle->TxLen > 0)
	{
		//1. load the data into DR
		pI2CHandle->pI2Cx->DR = *(pI2CHandle->pTxBuffer);

		//2. decrement the TxLen
		pI2CHandle->TxLen--;

		//3. Increment the buffer address
		pI2CHandle->pTxBuffer++;
	}
}





