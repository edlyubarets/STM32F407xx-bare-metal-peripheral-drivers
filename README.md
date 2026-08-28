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
with peripheral related macros, you can find them in `.h` driver files. Second member is the **base address of peripheral**, use it to choose the peripheral you want 
to configure. Then use the **Init** API to proceed the peripheral configuration. Also peripheral handle structure should be filled with zeros with memset function to avoid potential errors.

**Peripheral functional usage process**

Drivers provide APIs to control peripheral, send data, receive data in either blocking or interrupt mode. For interrupt usage it is necessary to implement IRQ handler
with IRQ_Handling API which is given by driver. Also in interrupt mode `CallBack API` is provided, it has weak implementation and have to be overwritten by user.

### Demonstration of workability
---

**GPIO led toggle**
-
<img width="636" height="358" alt="led_toggle_vid" src="https://github.com/user-attachments/assets/c8ee0423-8fd2-43d9-97ed-0a0cca005c0a" />


**SPI communication**
-
<img width="945" height="411" alt="spi_communication" src="https://github.com/user-attachments/assets/0c8559ae-13ed-43b0-8955-8e1699b103f9" />

**I2C communication**
-
<img width="794" height="358" alt="i2c_communication_1" src="https://github.com/user-attachments/assets/9fff67fa-2fb5-49f4-b68b-6fb62e0dce59" />
<img width="959" height="348" alt="i2c_communication_2" src="https://github.com/user-attachments/assets/98084950-14ae-4c63-8a8c-da9ac5ff91df" />

**UART communication**
-
<img width="944" height="427" alt="uart_communication" src="https://github.com/user-attachments/assets/7d5b4ee7-4077-44e0-a076-3189b5f87971" />




