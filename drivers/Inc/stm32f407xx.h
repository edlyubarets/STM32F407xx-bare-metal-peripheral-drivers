/*
 * stm32f407xx.h
 *
 *  Created on: Jul 11, 2026
 *      Author: dell
 */

#ifndef INC_STM32F407XX_H_
#define INC_STM32F407XX_H_


#include <stdint.h>
#include <stddef.h>


/*
 * ************************START: ARM Processor specific details************************
 */

/* ARM Cortex Mx Processor NVIC ISERx register Addresses */
#define NVIC_ISER0 		( (volatile uint32_t*)0xE000E100 )
#define NVIC_ISER1 		( (volatile uint32_t*)0xE000E104 )
#define NVIC_ISER2 		( (volatile uint32_t*)0xE000E108 )
#define NVIC_ISER3 		( (volatile uint32_t*)0xE000E10C )
#define NVIC_ISER4 		( (volatile uint32_t*)0xE000E110 )
#define NVIC_ISER5 		( (volatile uint32_t*)0xE000E114 )
#define NVIC_ISER6 		( (volatile uint32_t*)0xE000E118 )
#define NVIC_ISER7		( (volatile uint32_t*)0xE000E11C )

/* ARM Cortex Mx Processor NVIC ICERx register Addresses */
#define NVIC_ICER0 		( (volatile uint32_t*)0xE000E180 )
#define NVIC_ICER1 		( (volatile uint32_t*)0xE000E184 )
#define NVIC_ICER2 		( (volatile uint32_t*)0xE000E188 )
#define NVIC_ICER3 		( (volatile uint32_t*)0xE000E18C )
#define NVIC_ICER4 		( (volatile uint32_t*)0xE000E190 )
#define NVIC_ICER5 		( (volatile uint32_t*)0xE000E194 )
#define NVIC_ICER6 		( (volatile uint32_t*)0xE000E198 )
#define NVIC_ICER7 		( (volatile uint32_t*)0xE000E19C )

/* ARM Cortex Mx Processor IRQs priority registers Addresses */
#define NVIC_PR_BASE_ADDR		((volatile uint32_t*)0xE000E400 )


/*
 * *****************************Peripheral addresses macros******************************
 */

/*FLASH and SRAM memories base addresses macros*/
#define FLASH_BASE_ADDR			0x08000000U			/*Base address of FLASH memory*/
#define SRAM1_BASE_ADDR			0x20000000U			/*Base address of SRAM1 memory*/
#define SRAM2_BASE_ADDR			0x2001C000U			/*Base address of SRAM2 memory*/
#define ROM						0x1FFF0000U			/*Base address of System memory*/
#define SRAM 					SRAM1_BASE_ADDR		/*SRAM address as SRAM1 base address*/


/*AHBx & APBx Bus peripheral base addresses macros*/
#define PERIPH_BASE				0x40000000U			/*Base address of peripherals*/
#define APB1_PERIPH_BASE		PERIPH_BASE			/*Base address APB1 bus of peripherals*/
#define APB2_PERIPH_BASE		0x40010000U			/*Base address APB2 bus of peripherals*/
#define AHB1_PERIPH_BASE		0x40020000U			/*Base address AHB1 bus of peripherals*/
#define AHB2_PERIPH_BASE		0x50000000U			/*Base address AHB2 bus of peripherals*/


/*Base address of AHB1 bus peripherals*/
#define GPIOA_BASE_ADDR 		(AHB1_PERIPH_BASE + 0x0000)
#define GPIOB_BASE_ADDR 		(AHB1_PERIPH_BASE + 0x0400)
#define GPIOC_BASE_ADDR			(AHB1_PERIPH_BASE + 0x0800)
#define GPIOD_BASE_ADDR			(AHB1_PERIPH_BASE + 0x0C00)
#define GPIOE_BASE_ADDR			(AHB1_PERIPH_BASE + 0x1000)
#define GPIOF_BASE_ADDR			(AHB1_PERIPH_BASE + 0x1400)
#define GPIOG_BASE_ADDR			(AHB1_PERIPH_BASE + 0x1800)
#define GPIOH_BASE_ADDR			(AHB1_PERIPH_BASE + 0x1C00)
#define GPIOI_BASE_ADDR			(AHB1_PERIPH_BASE + 0x2000)
#define GPIOJ_BASE_ADDR			(AHB1_PERIPH_BASE + 0x2400)
#define GPIOK_BASE_ADDR			(AHB1_PERIPH_BASE + 0x2800)
#define RCC_BASE_ADDR			(AHB1_PERIPH_BASE + 0x3800)


/*Base address of APB1 bus peripherals*/
#define I2C1_BASE_ADDR 			(APB1_PERIPH_BASE + 0x5400)
#define I2C2_BASE_ADDR			(APB1_PERIPH_BASE + 0x5800)
#define I2C3_BASE_ADDR			(APB1_PERIPH_BASE + 0x5C00)
#define SPI2_BASE_ADDR			(APB1_PERIPH_BASE + 0x3800)
#define SPI3_BASE_ADDR			(APB1_PERIPH_BASE + 0x3C00)
#define USART2_BASE_ADDR		(APB1_PERIPH_BASE + 0x4400)
#define USART3_BASE_ADDR		(APB1_PERIPH_BASE + 0x4800)
#define UART4_BASE_ADDR			(APB1_PERIPH_BASE + 0x4C00)
#define UART5_BASE_ADDR			(APB1_PERIPH_BASE + 0x5000)


/*Base address of APB2 bus peripherals*/
#define EXTI_BASE_ADDR			(APB2_PERIPH_BASE + 0x3C00)
#define SPI1_BASE_ADDR			(APB2_PERIPH_BASE + 0x3000)
#define USART1_BASE_ADDR		(APB2_PERIPH_BASE + 0x1000)
#define USART6_BASE_ADDR		(APB2_PERIPH_BASE + 0x1400)
#define SYSCFG_BASE_ADDR		(APB2_PERIPH_BASE + 0x3800)
#define SPI4_BASE_ADDR			(APB2_PERIPH_BASE + 0x3400)




/*
 * **********************Peripheral register definition structures***********************
 */


/*#### RCC registers definition structure ####*/
typedef struct
{
	volatile uint32_t 	CR;				/*RCC clock control register, 				Address offset: 0x00*/
	volatile uint32_t 	PLLCFGR;		/*RCC PLL configuration register, 			Address offset: 0x04*/
	volatile uint32_t 	CFGR;			/*RCC clock configuration register, 		Address offset: 0x08*/
	volatile uint32_t 	CIR;			/*RCC clock interrupt register, 			Address offset: 0x0C*/
	volatile uint32_t 	AHB1RSTR;		/*RCC AHB1 peripheral reset register, 		Address offset: 0x10*/
	volatile uint32_t 	AHB2RSTR;		/*RCC AHB2 peripheral reset register, 		Address offset: 0x14*/
	volatile uint32_t 	AHB3RSTR;		/*RCC AHB3 peripheral reset register, 		Address offset: 0x18*/
	uint32_t 			RESERVED1;		/* reserved 0x1C*/
	volatile uint32_t 	APB1RSTR;		/*RCC APB1 peripheral reset register, 		Address offset: 0x20*/
	volatile uint32_t 	APB2RSTR;		/*RCC APB2 peripheral reset register, 		Address offset: 0x24*/
	uint32_t 			RESERVED2;		/* reserved 0x28*/
	uint32_t			RESERVED3;		/* reserved 0x2C*/
	volatile uint32_t 	AHB1ENR;		/*RCC AHB1 peripheral clock enable register , 		Address offset: 0x30*/
	volatile uint32_t 	AHB2ENR;		/*RCC AHB2 peripheral clock enable register , 		Address offset: 0x34*/
	volatile uint32_t 	AHB3ENR;		/*RCC AHB3 peripheral clock enable register , 		Address offset: 0x38*/
	uint32_t 			RESERVED4;		/* reserved 0x3C*/
	volatile uint32_t 	APB1ENR;		/*RCC APB1 peripheral clock enable register, 		Address offset: 0x40*/
	volatile uint32_t 	APB2ENR;		/*RCC APB2 peripheral clock enable register, 		Address offset: 0x44*/
	uint32_t			RESERVED5;		/* reserved 0x48*/
	uint32_t			RESERVED6;		/* reserved 0x4C*/
	volatile uint32_t 	AHB1LPENR;		/*RCC AHB1 peripheral clock enable in low power mode register, 		Address offset: 0x50*/
	volatile uint32_t 	AHB2LPENR;		/*RCC AHB2 peripheral clock enable in low power mode register, 		Address offset: 0x54*/
	volatile uint32_t 	AHB3LPENR;		/*RCC AHB3 peripheral clock enable in low power mode register, 		Address offset: 0x58*/
	uint32_t			RESERVED7;		/* reserved 0x5C*/
	volatile uint32_t 	APB1LPENR;		/*RCC APB1 peripheral clock enable in low power mode register, 		Address offset: 0x60*/
	volatile uint32_t 	APB2LPENR;		/*RCC APB2 peripheral clock enabled in low power mode register, 	Address offset: 0x64*/
	uint32_t			RESERVED8;		/* reserved 0x68*/
	uint32_t			RESERVED9;		/* reserved 0x6C*/
	volatile uint32_t 	BDCR;			/*RCC Backup domain control register, 		Address offset: 0x70*/
	volatile uint32_t 	CSR;			/*RCC clock control & status register, 		Address offset: 0x74*/
	uint32_t			RESERVED10;		/* reserved 0x78*/
	uint32_t 			RESERVED11;		/* reserved 0x7C*/
	volatile uint32_t 	SSCGR;			/*RCC spread spectrum clock generation register, 		Address offset: 0x80*/
	volatile uint32_t 	PLLI2SCFGR;		/*RCC PLLI2S configuration register, 					Address offset: 0x84*/

}RCC_RegDef_t;


/*#### GPIO registers structure ####*/
typedef struct
{
	volatile uint32_t MODER;	/*GPIO port mode register,					Address offset: 0x00*/
	volatile uint32_t OTYPER;	/*GPIO port output type register,			Address offset: 0x04*/
	volatile uint32_t OSPEED;	/*GPIO port output speed register,			Address offset: 0x08*/
	volatile uint32_t PUPDR;	/*GPIO port pull-up/pull-down register,		Address offset: 0x0C*/
	volatile uint32_t IDR;		/*GPIO port input data register,			Address offset: 0x10*/
	volatile uint32_t ODR;		/*GPIO port output data register ,			Address offset: 0x14*/
	volatile uint32_t BSRR;		/*GPIO port bit set/reset register,			Address offset: 0x18*/
	volatile uint32_t LCKR;		/*GPIO port configuration lock register,	Address offset: 0x1C*/
	volatile uint32_t AFRL;		/*GPIO alternate function low register,		Address offset: 0x20*/
	volatile uint32_t AFRH;		/*GPIO alternate function high register,	Address offset: 0x24*/

}GPIO_RegDef_t;


/*#### EXTI registers definition structure ####*/
typedef struct
{
	volatile uint32_t IMR;		/*Interrupt mask register, 					Address offset: 0x00*/
	volatile uint32_t EMR;		/*Event mask register, 						Address offset: 0x04*/
	volatile uint32_t RTSR;		/*Rising trigger selection register, 		Address offset: 0x08*/
	volatile uint32_t FTSR;		/*Falling trigger selection register, 		Address offset: 0x0C*/
	volatile uint32_t SWIER;	/*Software interrupt event register, 		Address offset: 0x10*/
	volatile uint32_t PR;		/*Pending register, 						Address offset: 0x14*/

}EXTI_RegDef_t;

/*#### SYSCFG registers definition structure ####*/
typedef struct
{
	volatile uint32_t MEMRMP;		/*memory remap register,  								Address offset: 0x00 */
	volatile uint32_t PMC;			/*peripheral mode configuration register,  				Address offset: 0x04 */
	volatile uint32_t EXTICR[4];	/*external interrupt configuration register(1 - 4),  	Address offset: 0x08 - 0x14 */
	uint32_t RESERVED[2];			/*reserved 0x18 - 0x1C*/
	volatile uint32_t CMPCR;		/*peripheral mode configuration register,  				Address offset: 0x20 */

}SYSCFG_RegDef_t;


/*#### SPI registers definition structure ####*/
typedef struct
{
	volatile uint32_t CR1;			/*SPI control register 1, 			Address offset: 0x00*/
	volatile uint32_t CR2;			/*SPI control register 2, 			Address offset: 0x04*/
	volatile uint32_t SR;			/*SPI status register,  			Address offset: 0x08*/
	volatile uint32_t DR;			/*SPI data register,  				Address offset: 0x0C*/
	volatile uint32_t CRCPR;		/*SPI CRC polynomial register, 		Address offset: 0x10*/
	volatile uint32_t RXCRCR;		/*SPI RX CRC register, 				Address offset: 0x14*/
	volatile uint32_t TXCRCR;		/*SPI TX CRC register, 				Address offset: 0x18*/
	volatile uint32_t I2SCFGR;		/*SPI_I2S configuration register,	Address offset: 0x1C*/
	volatile uint32_t I2SPR;		/*SPI_I2S prescaler register, 		Address offset: 0x20*/

}SPI_RegDef_t;


/* I2C registers definition structure */
typedef struct
{
	volatile uint32_t CR1;		/*<I2C Control register 1, 			Address offset: 0x00>*/
	volatile uint32_t CR2;		/*<I2C Control register 2, 			Address offset: 0x04>*/
	volatile uint32_t OAR1;		/*<I2C Own address register 1, 		Address offset: 0x08>*/
	volatile uint32_t OAR2;		/*<I2C Own address register 2, 		Address offset: 0x0C>*/
	volatile uint32_t DR;		/*<I2C Data register, 				Address offset: 0x10>*/
	volatile uint32_t SR1;		/*<I2C Status register 1, 			Address offset: 0x14>*/
	volatile uint32_t SR2;		/*<I2C Status register 2, 			Address offset: 0x18>*/
	volatile uint32_t CCR;		/*<I2C Clock control register, 		Address offset: 0x1C>*/
	volatile uint32_t TRISE;	/*<I2C TRISE register, 				Address offset: 0x20>*/
	volatile uint32_t FLTR;		/*<I2C FLTR register, 				Address offset: 0x24>*/

}I2C_RegDef_t;


/* USART registers definition structure */
typedef struct
{
	volatile uint32_t SR;		/*<USART Status register, 						Address offset: 0x00>*/
	volatile uint32_t DR;		/*<USART Data register, 						Address offset: 0x04>*/
	volatile uint32_t BRR;		/*<USART Baud Rate register, 					Address offset: 0x08>*/
	volatile uint32_t CR1;		/*<USART Control register 1, 					Address offset: 0x0C>*/
	volatile uint32_t CR2;		/*<USART Control register 2, 					Address offset: 0x10>*/
	volatile uint32_t CR3;		/*<USART Control register 3, 					Address offset: 0x14>*/
	volatile uint32_t GPTR;		/*<USART Guard time and prescaler register , 	Address offset: 0x0C>*/

}USART_RegDef_t;






/*
 * ****************************Peripheral definition macros*****************************
 */

/*Peripheral definition(peripheral base address typecasted to xxx_RegDef_t) macros*/
#define GPIOA 		((GPIO_RegDef_t*)GPIOA_BASE_ADDR)
#define GPIOB 		((GPIO_RegDef_t*)GPIOB_BASE_ADDR)
#define GPIOC 		((GPIO_RegDef_t*)GPIOC_BASE_ADDR)
#define GPIOD 		((GPIO_RegDef_t*)GPIOD_BASE_ADDR)
#define GPIOE 		((GPIO_RegDef_t*)GPIOE_BASE_ADDR)
#define GPIOF 		((GPIO_RegDef_t*)GPIOF_BASE_ADDR)
#define GPIOG 		((GPIO_RegDef_t*)GPIOG_BASE_ADDR)
#define GPIOH 		((GPIO_RegDef_t*)GPIOH_BASE_ADDR)
#define GPIOI 		((GPIO_RegDef_t*)GPIOI_BASE_ADDR)

#define RCC			((RCC_RegDef_t*)RCC_BASE_ADDR)
#define EXTI		((EXTI_RegDef_t*)EXTI_BASE_ADDR)
#define SYSCFG		((SYSCFG_RegDef_t*)SYSCFG_BASE_ADDR)

#define SPI1		((SPI_RegDef_t*)SPI1_BASE_ADDR)
#define SPI2		((SPI_RegDef_t*)SPI2_BASE_ADDR)
#define SPI3		((SPI_RegDef_t*)SPI3_BASE_ADDR)
#define SPI4		((SPI_RegDef_t*)SPI4_BASE_ADDR)

#define I2C1		((I2C_RegDef_t*)I2C1_BASE_ADDR)
#define I2C2		((I2C_RegDef_t*)I2C2_BASE_ADDR)
#define I2C3		((I2C_RegDef_t*)I2C3_BASE_ADDR)

#define USART1		((USART_RegDef_t*)USART1_BASE_ADDR)
#define USART2		((USART_RegDef_t*)USART2_BASE_ADDR)
#define USART3		((USART_RegDef_t*)USART3_BASE_ADDR)
#define UART4		((USART_RegDef_t*)UART4_BASE_ADDR)
#define UART5		((USART_RegDef_t*)UART5_BASE_ADDR)
#define USART6		((USART_RegDef_t*)USART6_BASE_ADDR)



/*
 * ***********************Peripheral clock enable/disable macros************************
 */

/*Clock enable macros for system configuration controller*/
#define SYSCFG_PCLK_EN()		( RCC->APB2ENR |= (1 << 14) )

/*Clock enable macros for GPIOx peripherals*/
#define GPIOA_PCLK_EN() 		( RCC->AHB1ENR |= (1 << 0) )
#define GPIOB_PCLK_EN() 		( RCC->AHB1ENR |= (1 << 1) )
#define GPIOC_PCLK_EN() 		( RCC->AHB1ENR |= (1 << 2) )
#define GPIOD_PCLK_EN() 		( RCC->AHB1ENR |= (1 << 3) )
#define GPIOE_PCLK_EN() 		( RCC->AHB1ENR |= (1 << 4) )
#define GPIOF_PCLK_EN() 		( RCC->AHB1ENR |= (1 << 5) )
#define GPIOG_PCLK_EN() 		( RCC->AHB1ENR |= (1 << 6) )
#define GPIOH_PCLK_EN() 		( RCC->AHB1ENR |= (1 << 7) )
#define GPIOI_PCLK_EN() 		( RCC->AHB1ENR |= (1 << 8) )

/*Clock enable macros for I2Cx peripherals*/
#define I2C1_PCLK_EN()			( RCC->APB1ENR |= (1 << 21) )
#define I2C2_PCLK_EN()			( RCC->APB1ENR |= (1 << 22) )
#define I2C3_PCLK_EN()			( RCC->APB1ENR |= (1 << 23) )

/*Clock enable macros for SPIx peripherals*/
#define SPI1_PCLK_EN()			( RCC->APB2ENR |= (1 << 12) )
#define SPI2_PCLK_EN()			( RCC->APB1ENR |= (1 << 14) )
#define SPI3_PCLK_EN()			( RCC->APB1ENR |= (1 << 15) )
#define SPI4_PCLK_EN()			( RCC->APB2ENR |= (1 << 13) )

/*Clock enable macros for USARTx peripherals*/
#define USART1_PCLK_EN()		( RCC->APB2ENR |= (1 << 4) )
#define USART2_PCLK_EN()		( RCC->APB1ENR |= (1 << 17) )
#define USART3_PCLK_EN()		( RCC->APB1ENR |= (1 << 18) )
#define UART4_PCLK_EN()			( RCC->APB1ENR |= (1 << 19) )
#define UART5_PCLK_EN()			( RCC->APB1ENR |= (1 << 20) )
#define USART6_PCLK_EN()		( RCC->APB2ENR |= (1 << 5) )



/*Clock disable macros for system configuration controller*/
#define SYSCFG_PCLK_DI()		( RCC->APB2ENR &= ~(1 << 14) )

/*Clock disable macros for GPIOx peripherals*/
#define GPIOA_PCLK_DI() 		( RCC->AHB1ENR &= ~(1 << 0) )
#define GPIOB_PCLK_DI() 		( RCC->AHB1ENR &= ~(1 << 1) )
#define GPIOC_PCLK_DI() 		( RCC->AHB1ENR &= ~(1 << 2) )
#define GPIOD_PCLK_DI() 		( RCC->AHB1ENR &= ~(1 << 3) )
#define GPIOE_PCLK_DI() 		( RCC->AHB1ENR &= ~(1 << 4) )
#define GPIOF_PCLK_DI() 		( RCC->AHB1ENR &= ~(1 << 5) )
#define GPIOG_PCLK_DI() 		( RCC->AHB1ENR &= ~(1 << 6) )
#define GPIOH_PCLK_DI() 		( RCC->AHB1ENR &= ~(1 << 7) )
#define GPIOI_PCLK_DI() 		( RCC->AHB1ENR &= ~(1 << 8) )

/*Clock disable macros for I2Cx peripherals*/
#define I2C1_PCLK_DI()			( RCC->APB1ENR &= ~(1 << 21) )
#define I2C2_PCLK_DI()			( RCC->APB1ENR &= ~(1 << 22) )
#define I2C3_PCLK_DI()			( RCC->APB1ENR &= ~(1 << 23) )

/*Clock disable macros for SPIx peripherals*/
#define SPI1_PCLK_DI()			( RCC->APB2ENR &= ~(1 << 12) )
#define SPI2_PCLK_DI()			( RCC->APB1ENR &= ~(1 << 14) )
#define SPI3_PCLK_DI()			( RCC->APB1ENR &= ~(1 << 15) )
#define SPI4_PCLK_DI()			( RCC->APB2ENR &= ~(1 << 13) )

/*Clock disable macros for USARTx peripherals*/
#define USART1_PCLK_DI()		( RCC->APB2ENR |= (1 << 4) )
#define USART2_PCLK_DI()		( RCC->APB1ENR |= (1 << 17) )
#define USART3_PCLK_DI()		( RCC->APB1ENR |= (1 << 18) )
#define UART4_PCLK_DI()			( RCC->APB1ENR |= (1 << 19) )
#define UART5_PCLK_DI()			( RCC->APB1ENR |= (1 << 20) )
#define USART6_PCLK_DI()		( RCC->APB2ENR |= (1 << 5) )



/*
 * ******************************Peripheral Reset macros********************************
 */

/*Macros to Reset GPIOx peripherals*/
#define GPIOA_REG_RESET() 			do{ (RCC->AHB1RSTR |= (1 << 0));	(RCC->AHB1RSTR &= ~(1 << 0));}while(0)
#define GPIOB_REG_RESET() 			do{ (RCC->AHB1RSTR |= (1 << 1));	(RCC->AHB1RSTR &= ~(1 << 1));}while(0)
#define GPIOC_REG_RESET() 			do{ (RCC->AHB1RSTR |= (1 << 2));	(RCC->AHB1RSTR &= ~(1 << 2));}while(0)
#define GPIOD_REG_RESET() 			do{ (RCC->AHB1RSTR |= (1 << 3));	(RCC->AHB1RSTR &= ~(1 << 3));}while(0)
#define GPIOE_REG_RESET() 			do{ (RCC->AHB1RSTR |= (1 << 4));	(RCC->AHB1RSTR &= ~(1 << 4));}while(0)
#define GPIOF_REG_RESET() 			do{ (RCC->AHB1RSTR |= (1 << 5));	(RCC->AHB1RSTR &= ~(1 << 5));}while(0)
#define GPIOG_REG_RESET() 			do{ (RCC->AHB1RSTR |= (1 << 6));	(RCC->AHB1RSTR &= ~(1 << 6));}while(0)
#define GPIOH_REG_RESET() 			do{ (RCC->AHB1RSTR |= (1 << 7));	(RCC->AHB1RSTR &= ~(1 << 7));}while(0)
#define GPIOI_REG_RESET() 			do{ (RCC->AHB1RSTR |= (1 << 8));	(RCC->AHB1RSTR &= ~(1 << 8));}while(0)

/*Macros to Reset SPIx peripherals*/
#define SPI1_REG_RESET()			do{ (RCC->APB2RSTR |= (1 << 12));	(RCC->APB2RSTR &= ~(1 << 12));}while(0)
#define SPI2_REG_RESET()			do{ (RCC->APB1RSTR |= (1 << 14));	(RCC->APB1RSTR &= ~(1 << 14));}while(0)
#define SPI3_REG_RESET()			do{ (RCC->APB1RSTR |= (1 << 15));	(RCC->APB1RSTR &= ~(1 << 15));}while(0)
#define SPI4_REG_RESET()			do{ (RCC->APB2RSTR |= (1 << 13));	(RCC->APB1RSTR &= ~(1 << 13));}while(0)

/*Macros to Reset I2Cx peripherals*/
#define I2C1_REG_RESET()			do{ (RCC->APB1RSTR |= (1 << 21));   (RCC->APB1RSTR &= ~(1 << 21));}while(0)
#define I2C2_REG_RESET()			do{ (RCC->APB1RSTR |= (1 << 22));   (RCC->APB1RSTR &= ~(1 << 22));}while(0)
#define I2C3_REG_RESET()			do{ (RCC->APB1RSTR |= (1 << 23));   (RCC->APB1RSTR &= ~(1 << 23));}while(0)

/*Macros to Reset USARTx peripherals*/
#define USART1_REG_RESET()			do{ (RCC->APB2RSTR |= (1 << 4));	(RCC->APB2RSTR &= ~(1 << 4));}while(0)
#define USART2_REG_RESET()			do{ (RCC->APB1RSTR |= (1 << 17));	(RCC->APB1RSTR &= ~(1 << 17));}while(0)
#define USART3_REG_RESET()			do{ (RCC->APB1RSTR |= (1 << 18));	(RCC->APB1RSTR &= ~(1 << 18));}while(0)
#define UART4_REG_RESET()			do{ (RCC->APB1RSTR |= (1 << 19));	(RCC->APB1RSTR &= ~(1 << 19));}while(0)
#define UART5_REG_RESET()			do{ (RCC->APB1RSTR |= (1 << 20));	(RCC->APB1RSTR &= ~(1 << 20));}while(0)
#define USART6_REG_RESET()			do{ (RCC->APB2RSTR |= (1 << 5));	(RCC->APB2RSTR &= ~(1 << 5));}while(0)



/*
 * ************************************IRQ macros***************************************
 */
#define IRQ_NO_EXTI0			6
#define IRQ_NO_EXTI1			7
#define IRQ_NO_EXTI2			8
#define IRQ_NO_EXTI3			9
#define IRQ_NO_EXTI4			10
#define IRQ_NO_EXTI9_5			23
#define IRQ_NO_EXTI15_10		40

#define IRQ_NO_SPI1			35
#define IRQ_NO_SPI2			36
#define IRQ_NO_SPI3			51
#define IRQ_NO_SPI4			84

#define IRQ_NO_I2C1_EV		31
#define IRQ_NO_I2C1_ER		32
#define IRQ_NO_I2C2_EV		33
#define IRQ_NO_I2C2_ER		34
#define IRQ_NO_I2C3_EV		72
#define IRQ_NO_I2C3_ER		73

#define IRQ_NO_USART1		37
#define IRQ_NO_USART2		38
#define IRQ_NO_USART3		39
#define IRQ_NO_UART4		52
#define IRQ_NO_UART5		53
#define IRQ_NO_USART6		71

#define NO_PR_BITS_IMPLEMENTED	4

#define NVIC_IRQ_PR0		0
#define NVIC_IRQ_PR1		1
#define NVIC_IRQ_PR2		2
#define NVIC_IRQ_PR3		3
#define NVIC_IRQ_PR4		4
#define NVIC_IRQ_PR5		5
#define NVIC_IRQ_PR6		6
#define NVIC_IRQ_PR7		7
#define NVIC_IRQ_PR8		8
#define NVIC_IRQ_PR9		9
#define NVIC_IRQ_PR10		10
#define NVIC_IRQ_PR11		11
#define NVIC_IRQ_PR12		12
#define NVIC_IRQ_PR13		13
#define NVIC_IRQ_PR14		14
#define NVIC_IRQ_PR15		15


/*
 * **********************************Generic Macros*************************************
 */
#define ENABLE 				1
#define DISABLE 			0
#define SET 				ENABLE
#define RESET				DISABLE
#define GPIO_PIN_SET		SET
#define GPIO_PIN_RESET		RESET
#define FLAG_SET			SET
#define FLAG_RESET			RESET


/*
 * *******************Bit Position Definitions for SPI peripherals***********************
 */

#define SPI_CR1_CPHA 		0
#define SPI_CR1_CPOL		1
#define SPI_CR1_MSTR		2
#define SPI_CR1_BR			3
#define SPI_CR1_SPE			6
#define SPI_CR1_LSBFIRST	7
#define SPI_CR1_SSI			8
#define SPI_CR1_SSM			9
#define SPI_CR1_RXONLY		10
#define SPI_CR1_DFF			11
#define SPI_CR1_CRCNEXT		12
#define SPI_CR1_CRCEN		13
#define SPI_CR1_BIDIOE		14
#define SPI_BIDI_MODE		15

#define SPI_CR2_RXDMAEN		0
#define SPI_CR2_TXDMAEN		1
#define SPI_CR2_SSOE		2
#define SPI_CR2_FRF			4
#define SPI_CR2_ERRIE		5
#define SPI_CR2_RXNEIE		6
#define SPI_CR2_TXEIE		7

#define SPI_SR_RXNE			0
#define SPI_SR_TXE			1
#define SPI_SR_CHSIDE		2
#define SPI_SR_UDR			3
#define SPI_SR_CRCERR		4
#define SPI_SR_MODF			5
#define SPI_SR_OVR			6
#define SPI_SR_BSY			7
#define SPI_SR_FRE			8



/*
 * *******************Bit Position Definitions for I2C peripherals***********************
 */
#define I2C_CR1_PE				0
#define I2C_CR1_SMBUS			1
#define I2C_CR1_SMBTYPE			3
#define I2C_CR1_ENARP			4
#define I2C_CR1_ENPEC			5
#define I2C_CR1_ENGC			6
#define I2C_CR1_NOSTRECH		7
#define I2C_CR1_START			8
#define I2C_CR1_STOP			9
#define I2C_CR1_ACK				10
#define I2C_CR1_POS				11
#define I2C_CR1_PEC				12
#define I2C_CR1_ALERT			13
#define I2C_CR1_SWRST			15

#define I2C_CR2_FREQ			0
#define I2C_CR2_ITERREN			8
#define I2C_CR2_ITEVTEN			9
#define I2C_CR2_ITBUFEN			10
#define I2C_CR2_DMAEN			11
#define I2C_CR2_LAST			12

#define I2C_SR1_SB				0
#define I2C_SR1_ADDR			1
#define I2C_SR1_BTF				2
#define I2C_SR1_ADD10			3
#define I2C_SR1_STOPF			4
#define I2C_SR1_RXNE			6
#define I2C_SR1_TXE				7
#define I2C_SR1_BERR			8
#define I2C_SR1_ARLO			9
#define I2C_SR1_AF				10
#define I2C_SR1_OVR				11
#define I2C_SR1_PECERR			12
#define I2C_SR1_TIMEOUT			14
#define I2C_SR1_SMBALERT		15

#define I2C_SR2_MSL				0
#define I2C_SR2_BUSY			1
#define I2C_SR2_TRA				2
#define I2C_SR2_GENCALL			4
#define I2C_SR2_SMBDEFAULT		5
#define I2C_SR2_SMBHOST			6
#define I2C_SR2_DUALF			7
#define I2C_SR2_PEC				8

#define I2C_CCR_CCR				0
#define I2C_CCR_DUTY			14
#define I2C_CCR_F_S				15



/*
 * *******************Bit Position Definitions for USART peripherals***********************
 */
#define USART_SR_PE			0
#define USART_SR_FE			1
#define USART_SR_NF			2
#define USART_SR_ORE		3
#define USART_SR_IDLE		4
#define USART_SR_RXNE		5
#define USART_SR_TC			6
#define USART_SR_TXE		7
#define USART_SR_LBD		8
#define USART_SR_CTS		9

#define USART_BRR_DIV_FRACTION		0
#define USART_BRR_DIV_MANTISSA		4

#define USART_CR1_SBK			0
#define USART_CR1_RWU			1
#define USART_CR1_RE			2
#define USART_CR1_TE			3
#define USART_CR1_IDLEIE		4
#define USART_CR1_RXNEIE		5
#define USART_CR1_TCIE			6
#define USART_CR1_TXEIE			7
#define USART_CR1_PEIE			8
#define USART_CR1_PS			9
#define USART_CR1_PCE			10
#define USART_CR1_WAKE			11
#define USART_CR1_M				12
#define USART_CR1_UE			13
#define USART_CR1_OVER8			15

#define USART_CR2_ADD			0
#define USART_CR2_LBDL			5
#define USART_CR2_LBDIE			6
#define USART_CR2_LBCL			8
#define USART_CR2_CPHA			9
#define USART_CR2_CPOL			10
#define USART_CR2_CLKEN			11
#define USART_CR2_STOP			12
#define USART_CR2_LINEN			14

#define USART_CR3_EIE			0
#define USART_CR3_IREN			1
#define USART_CR3_IRLP			2
#define USART_CR3_HDSEL			3
#define USART_CR3_NACK			4
#define USART_CR3_SCEN			5
#define USART_CR3_DMAR			6
#define USART_CR3_DMAT			7
#define USART_CR3_RTSE			8
#define USART_CR3_CTSE			9
#define USART_CR3_CTSIE			10
#define USART_CR3_ONEBIT		11





/*GPIO codes*/
#define PA_CODE		0
#define PB_CODE		1
#define PC_CODE		2
#define PD_CODE		3
#define PE_CODE		4
#define PF_CODE		5
#define PG_CODE		6
#define PH_CODE		7
#define PI_CODE		8
#define ER_CODE 	0xFF


static inline uint8_t gpio_base_addr_to_code(GPIO_RegDef_t* GPIOx) {
	if(GPIOx == GPIOA) {
		return PA_CODE;
	} else if(GPIOx == GPIOB) {
		return PB_CODE;
	} else if(GPIOx == GPIOC) {
		return PC_CODE;
	} else if(GPIOx == GPIOD) {
		return PD_CODE;
	} else if(GPIOx == GPIOE) {
		return PE_CODE;
	} else if(GPIOx == GPIOF) {
		return PF_CODE;
	} else if(GPIOx == GPIOG) {
		return PG_CODE;
	} else if(GPIOx == GPIOH) {
		return PH_CODE;
	} else if(GPIOx == GPIOI) {
		return PI_CODE;
	}

	//error code return
	return ER_CODE;
}




#include "stm32f407xx_gpio_driver.h"
#include "stm32f407xx_spi_driver.h"
#include "stm32f407xx_i2c_driver.h"
#include "stm32f407xx_rcc_driver.h"
#include "stm32f407xx_usart_driver.h"


#endif /* INC_STM32F407XX_H_ */
