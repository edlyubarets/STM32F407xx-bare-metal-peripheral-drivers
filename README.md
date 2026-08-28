# STM32F407xx-bare-metal-peripheral-drivers
### Overview 
---

Introducing STM32F407xx Bare Metal Peripheral Drivers

These drivers contain APIs for according peripherals:
`GPIO`, `SPI`, `I2C`, `USART`, `RCC(additionally)`

Usage examples can be found in `/test` directory

### Detailed Description
---
**Drivers components overview:**

`stm32f407xx.h` - contains general MCU & PCU macros, memory addresses & register definition structures

`gpio_driver.h`,`spi_driver.h`, `i2c_driver.h`, `usart_driver.h`, `rcc_driver.h` - contain peripheral config structures, peripheral handle structures,
peripheral related macros & peripheral APIs prototypes

`gpio_driver.c`, `spi_driver.c`, `i2c_driver.c`, `usart_driver.c`, `rcc_driver.c` - contains contain peripheral APIs definitions & some private helper functions

- *RCC driver provides APIs used to get bus clock frequencies, these APIs are used by other drivers itself*

**Peripheral configuration process:**

For configuring peripherals use its **handle structures**. First member of handle structure is **config structure**, use this structure to configure peripheral parameters
with peripheral related macros, you can find them in `.h` driver files. Second member is the **base address of peripheral**, use it to chose the peripheral you want 
to configure. Then use the **Init** API to proceed the peripheral configuration.

**Peripheral functional usage process**

Drivers provide APIs to control peripheral, send data, receive data in either blocking or interrupt mode. For interrupt usage it is necessary to implement IRQ handler
with IRQ_Handling API which is given by driver. Also in interrupt mode `CallBack API` is provided, it has weak implementation and have to be overwritten by user.

### Demonstration of workability
---
