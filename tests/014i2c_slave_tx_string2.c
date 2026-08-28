/*
 * 011i2c_master_rx_testing.c
 *
 *  Created on: Aug 4, 2026
 *      Author: dell
 */

#include "stm32f407xx.h"
#include <string.h>
#include <stdio.h>

#define SLAVE_ADDR 		0x68
#define MY_ADDR			SLAVE_ADDR
#define CMD_CODE_LEN	0x51
#define CMD_CODE_READ	0x52


/*
 * PB6 -> SCL
 * PB7 -> SDA
 */

I2C_Handle_t I2C1Handle;

uint32_t data_len = 0;

uint8_t cmd_code = 0;

//large message
uint8_t Tx_Buff[] = "HiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHiHi...123";

//receive buffer
uint8_t rcv_buf[32];


void I2C1_GPIOInit(void)
{
	GPIO_Handle_t I2CPins;
	memset(&I2CPins, 0, sizeof(I2CPins));

	I2CPins.GPIOx = GPIOB;
	I2CPins.GPIO_PinConfig.GPIO_PinMode 		= 	GPIO_MODE_ALTFN;
	I2CPins.GPIO_PinConfig.GPIO_PinAltFunMode 	= 	4;
	I2CPins.GPIO_PinConfig.GPIO_PinOPType 		= 	GPIO_OP_TYPE_OD;
	I2CPins.GPIO_PinConfig.GPIO_PinPuPdCtrl	 	=   GPIO_PIN_PU;
	I2CPins.GPIO_PinConfig.GPIO_PinSpeed		=	GPIO_SPEED_FAST;

	//SCL
	I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_6;
	GPIO_Init(&I2CPins);

	//SDA
	I2CPins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_7;
	GPIO_Init(&I2CPins);

}

void I2C1_Init()
{
	memset(&I2C1Handle, 0, sizeof(I2C1Handle));

	I2C1Handle.pI2Cx = I2C1;

	I2C1Handle.I2C_Config.ACKControl = I2C_ACK_ENABLE;
	I2C1Handle.I2C_Config.I2C_DeviceAddress = MY_ADDR;
	I2C1Handle.I2C_Config.I2C_FMDutyCycle = I2C_FM_DUTY_2;
	I2C1Handle.I2C_Config.I2C_SCLSpeed = I2C_SCL_SPEED_STANDARD;

	I2C_Init(&I2C1Handle);
}


void GPIO_ButtonInit()
{
	GPIO_Handle_t GpioButton;
	memset(&GpioButton, 0, sizeof(GpioButton));

	GpioButton.GPIOx = GPIOA;

	GpioButton.GPIO_PinConfig.GPIO_PinNumber 		= 	GPIO_PIN_0;
	GpioButton.GPIO_PinConfig.GPIO_PinMode 			= 	GPIO_MODE_IN;
	GpioButton.GPIO_PinConfig.GPIO_PinSpeed 		= 	GPIO_SPEED_MEDIUM;
	GpioButton.GPIO_PinConfig.GPIO_PinPuPdCtrl 		= 	GPIO_NO_PUPD;

	GPIO_Init(&GpioButton);

}


void delay(void)
{
	for(uint32_t i = 0 ; i < 250000 ; i++);
}


int main(void)
{

	data_len = strlen((char*)Tx_Buff);

	//i2c pins init
	I2C1_GPIOInit();

	//button pin init
	GPIO_ButtonInit();

	//i2c peripheral configuration
	I2C1_Init();

	//I2C IRQ configuration
	I2C_IRQInterruptConfig(IRQ_NO_I2C1_EV, ENABLE);
	I2C_IRQInterruptConfig(IRQ_NO_I2C1_ER, ENABLE);

	I2C_SlaveEnableDisableCallbackEvents(I2C1, ENABLE);

	//enable the i2c peripheral
	I2C_PeripheralCtrl(I2C1, ENABLE);

	while(1);

}


void I2C1_EV_IRQHandler(void)
{
	I2C_EV_IRQHandling(&I2C1Handle);
}


void I2C1_ER_IRQHandler(void)
{
	I2C_ER_IRQHandling(&I2C1Handle);
}


void I2C_ApplicationEventCallback(I2C_Handle_t *pI2CHandle, uint8_t AppEv)
{
	static uint32_t cnt = 0;
	static uint32_t w_ptr = 0;


	if(AppEv == I2C_EV_DATA_REQ)
	{
		//master is requesting for the data -> send data
		if(cmd_code == 0x51)
		{
			//here we are sending 4 bytes of length information
			I2C_SlaveSendData(I2C1, ((data_len >> ((cnt % 4) * 8)) & 0xFF));
			cnt++;
		} else if(cmd_code == 0x52)
		{
			//sending tx buffer contents indexed by w_ptr variable
			I2C_SlaveSendData(I2C1, Tx_Buff[w_ptr++]);
		}

	} else if(AppEv == I2C_EV_DATA_RCV)
	{
		//master sends some data -> slave has to receive it
		cmd_code = I2C_SlaveReceiveData(I2C1);

	} else if(AppEv == I2C_ERROR_AF)
	{
		//this happens only during slave transmission
		//master has sent the NACK
		//slave has to understand that master doesn't need more data

		//if the current active code is 0x52 then don't validate
		if( ! (cmd_code == 0x52))
		{
			cmd_code == 0xFF;
		}

		//reset the counter value because its end of transmission
		cnt = 0;

		//Slave concludes it sent all the bytes when w_ptr reaches data_len
		if(w_ptr >= data_len)
		{
			w_ptr = 0;
			cmd_code == 0xFF;
		}

	} else if(AppEv == I2C_EV_STOP)
	{
		//this happens only during slave reception
		//master has ended i2c communication with a slave
		cnt = 0;

	}
}


