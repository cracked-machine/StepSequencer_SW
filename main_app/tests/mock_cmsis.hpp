// MIT License

// Copyright (c) 2022 Chris Sutton

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef __MOCK_CMSIS_HPP__
#define __MOCK_CMSIS_HPP__

#include <stdint.h>
#define __IO volatile

typedef struct
{
  __IO uint32_t CCR;         /*!< DMA channel x configuration register        */
  __IO uint32_t CNDTR;       /*!< DMA channel x number of data register       */
  __IO uint32_t CPAR;        /*!< DMA channel x peripheral address register   */
  __IO uint32_t CMAR;        /*!< DMA channel x memory address register       */
} DMA_Channel_TypeDef;

typedef struct
{
  __IO uint32_t ISR;         /*!< DMA interrupt status register,                 Address offset: 0x00 */
  __IO uint32_t IFCR;        /*!< DMA interrupt flag clear register,             Address offset: 0x04 */
} DMA_TypeDef;

typedef struct
{
  __IO uint32_t RTSR1;          /*!< EXTI Rising Trigger Selection Register 1,        Address offset:   0x00 */
  __IO uint32_t FTSR1;          /*!< EXTI Falling Trigger Selection Register 1,       Address offset:   0x04 */
  __IO uint32_t SWIER1;         /*!< EXTI Software Interrupt event Register 1,        Address offset:   0x08 */
  __IO uint32_t RPR1;           /*!< EXTI Rising Pending Register 1,                  Address offset:   0x0C */
  __IO uint32_t FPR1;           /*!< EXTI Falling Pending Register 1,                 Address offset:   0x10 */
       uint32_t RESERVED1[3];   /*!< Reserved 1,                                                0x14 -- 0x1C */
  __IO uint32_t RTSR2;          /*!< EXTI Rising Trigger Selection Register 2,        Address offset:   0x20 */
  __IO uint32_t FTSR2;          /*!< EXTI Falling Trigger Selection Register 2,       Address offset:   0x24 */
  __IO uint32_t SWIER2;         /*!< EXTI Software Interrupt event Register 2,        Address offset:   0x28 */
  __IO uint32_t RPR2;           /*!< EXTI Rising Pending Register 2,                  Address offset:   0x2C */
  __IO uint32_t FPR2;           /*!< EXTI Falling Pending Register 2,                 Address offset:   0x30 */
       uint32_t RESERVED3[11];  /*!< Reserved 3,                                                0x34 -- 0x5C */
  __IO uint32_t EXTICR[4];      /*!< EXTI External Interrupt Configuration Register,            0x60 -- 0x6C */
       uint32_t RESERVED4[4];   /*!< Reserved 4,                                                0x70 -- 0x7C */
  __IO uint32_t IMR1;           /*!< EXTI Interrupt Mask Register 1,                  Address offset:   0x80 */
  __IO uint32_t EMR1;           /*!< EXTI Event Mask Register 1,                      Address offset:   0x84 */
       uint32_t RESERVED5[2];   /*!< Reserved 5,                                                0x88 -- 0x8C */
  __IO uint32_t IMR2;           /*!< EXTI Interrupt Mask Register 2,                  Address offset:   0x90 */
  __IO uint32_t EMR2;           /*!< EXTI Event Mask Register 2,                      Address offset:   0x94 */
} EXTI_TypeDef;


typedef struct
{
  __IO uint32_t MODER;       /*!< GPIO port mode register,               Address offset: 0x00      */
  __IO uint32_t OTYPER;      /*!< GPIO port output type register,        Address offset: 0x04      */
  __IO uint32_t OSPEEDR;     /*!< GPIO port output speed register,       Address offset: 0x08      */
  __IO uint32_t PUPDR;       /*!< GPIO port pull-up/pull-down register,  Address offset: 0x0C      */
  __IO uint32_t IDR;         /*!< GPIO port input data register,         Address offset: 0x10      */
  __IO uint32_t ODR;         /*!< GPIO port output data register,        Address offset: 0x14      */
  __IO uint32_t BSRR;        /*!< GPIO port bit set/reset  register,     Address offset: 0x18      */
  __IO uint32_t LCKR;        /*!< GPIO port configuration lock register, Address offset: 0x1C      */
  __IO uint32_t AFR[2];      /*!< GPIO alternate function registers,     Address offset: 0x20-0x24 */
  __IO uint32_t BRR;         /*!< GPIO Bit Reset register,               Address offset: 0x28      */
} GPIO_TypeDef;

typedef struct
{
  __IO uint32_t CR1;         /*!< I2C Control register 1,            Address offset: 0x00 */
  __IO uint32_t CR2;         /*!< I2C Control register 2,            Address offset: 0x04 */
  __IO uint32_t OAR1;        /*!< I2C Own address 1 register,        Address offset: 0x08 */
  __IO uint32_t OAR2;        /*!< I2C Own address 2 register,        Address offset: 0x0C */
  __IO uint32_t TIMINGR;     /*!< I2C Timing register,               Address offset: 0x10 */
  __IO uint32_t TIMEOUTR;    /*!< I2C Timeout register,              Address offset: 0x14 */
  __IO uint32_t ISR;         /*!< I2C Interrupt and status register, Address offset: 0x18 */
  __IO uint32_t ICR;         /*!< I2C Interrupt clear register,      Address offset: 0x1C */
  __IO uint32_t PECR;        /*!< I2C PEC register,                  Address offset: 0x20 */
  __IO uint32_t RXDR;        /*!< I2C Receive data register,         Address offset: 0x24 */
  __IO uint32_t TXDR;        /*!< I2C Transmit data register,        Address offset: 0x28 */
} I2C_TypeDef;


typedef struct
{
  __IO uint32_t CR1;      /*!< SPI Control register 1 (not used in I2S mode),       Address offset: 0x00 */
  __IO uint32_t CR2;      /*!< SPI Control register 2,                              Address offset: 0x04 */
  __IO uint32_t SR;       /*!< SPI Status register,                                 Address offset: 0x08 */
  __IO uint32_t DR;       /*!< SPI data register,                                   Address offset: 0x0C */
  __IO uint32_t CRCPR;    /*!< SPI CRC polynomial register (not used in I2S mode),  Address offset: 0x10 */
  __IO uint32_t RXCRCR;   /*!< SPI Rx CRC register (not used in I2S mode),          Address offset: 0x14 */
  __IO uint32_t TXCRCR;   /*!< SPI Tx CRC register (not used in I2S mode),          Address offset: 0x18 */
  __IO uint32_t I2SCFGR;  /*!< SPI_I2S configuration register,                      Address offset: 0x1C */
  __IO uint32_t I2SPR;    /*!< SPI_I2S prescaler register,                          Address offset: 0x20 */
} SPI_TypeDef;

typedef struct
{
  __IO uint32_t CR1;         /*!< TIM control register 1,                   Address offset: 0x00 */
  __IO uint32_t CR2;         /*!< TIM control register 2,                   Address offset: 0x04 */
  __IO uint32_t SMCR;        /*!< TIM slave mode control register,          Address offset: 0x08 */
  __IO uint32_t DIER;        /*!< TIM DMA/interrupt enable register,        Address offset: 0x0C */
  __IO uint32_t SR;          /*!< TIM status register,                      Address offset: 0x10 */
  __IO uint32_t EGR;         /*!< TIM event generation register,            Address offset: 0x14 */
  __IO uint32_t CCMR1;       /*!< TIM capture/compare mode register 1,      Address offset: 0x18 */
  __IO uint32_t CCMR2;       /*!< TIM capture/compare mode register 2,      Address offset: 0x1C */
  __IO uint32_t CCER;        /*!< TIM capture/compare enable register,      Address offset: 0x20 */
  __IO uint32_t CNT;         /*!< TIM counter register,                     Address offset: 0x24 */
  __IO uint32_t PSC;         /*!< TIM prescaler register,                   Address offset: 0x28 */
  __IO uint32_t ARR;         /*!< TIM auto-reload register,                 Address offset: 0x2C */
  __IO uint32_t RCR;         /*!< TIM repetition counter register,          Address offset: 0x30 */
  __IO uint32_t CCR1;        /*!< TIM capture/compare register 1,           Address offset: 0x34 */
  __IO uint32_t CCR2;        /*!< TIM capture/compare register 2,           Address offset: 0x38 */
  __IO uint32_t CCR3;        /*!< TIM capture/compare register 3,           Address offset: 0x3C */
  __IO uint32_t CCR4;        /*!< TIM capture/compare register 4,           Address offset: 0x40 */
  __IO uint32_t BDTR;        /*!< TIM break and dead-time register,         Address offset: 0x44 */
  __IO uint32_t DCR;         /*!< TIM DMA control register,                 Address offset: 0x48 */
  __IO uint32_t DMAR;        /*!< TIM DMA address for full transfer,        Address offset: 0x4C */
  __IO uint32_t OR1;         /*!< TIM option register,                      Address offset: 0x50 */
  __IO uint32_t CCMR3;       /*!< TIM capture/compare mode register 3,      Address offset: 0x54 */
  __IO uint32_t CCR5;        /*!< TIM capture/compare register5,            Address offset: 0x58 */
  __IO uint32_t CCR6;        /*!< TIM capture/compare register6,            Address offset: 0x5C */
  __IO uint32_t AF1;         /*!< TIM alternate function register 1,        Address offset: 0x60 */
  __IO uint32_t AF2;         /*!< TIM alternate function register 2,        Address offset: 0x64 */
  __IO uint32_t TISEL;       /*!< TIM Input Selection register,             Address offset: 0x68 */
} TIM_TypeDef;

typedef struct
{
  __IO uint32_t CR1;         /*!< USART Control register 1,                 Address offset: 0x00  */
  __IO uint32_t CR2;         /*!< USART Control register 2,                 Address offset: 0x04  */
  __IO uint32_t CR3;         /*!< USART Control register 3,                 Address offset: 0x08  */
  __IO uint32_t BRR;         /*!< USART Baud rate register,                 Address offset: 0x0C  */
  __IO uint32_t GTPR;        /*!< USART Guard time and prescaler register,  Address offset: 0x10  */
  __IO uint32_t RTOR;        /*!< USART Receiver Time Out register,         Address offset: 0x14  */
  __IO uint32_t RQR;         /*!< USART Request register,                   Address offset: 0x18  */
  __IO uint32_t ISR;         /*!< USART Interrupt and status register,      Address offset: 0x1C  */
  __IO uint32_t ICR;         /*!< USART Interrupt flag Clear register,      Address offset: 0x20  */
  __IO uint32_t RDR;         /*!< USART Receive Data register,              Address offset: 0x24  */
  __IO uint32_t TDR;         /*!< USART Transmit Data register,             Address offset: 0x28  */
  __IO uint32_t PRESC;       /*!< USART Prescaler register,                 Address offset: 0x2C  */
} USART_TypeDef;

#define FLASH_BASE            (0x08000000UL)  /*!< FLASH base address */
#define SRAM_BASE             (0x20000000UL)  /*!< SRAM base address */
#define PERIPH_BASE           (0x40000000UL)  /*!< Peripheral base address */
#define IOPORT_BASE           (0x50000000UL)  /*!< IOPORT base address */

#define APBPERIPH_BASE        (PERIPH_BASE)

#define TIM2_BASE             (APBPERIPH_BASE + 0UL)
#define TIM3_BASE             (APBPERIPH_BASE + 0x00000400UL)
#define TIM4_BASE             (APBPERIPH_BASE + 0x00000800UL)
#define TIM6_BASE             (APBPERIPH_BASE + 0x00001000UL)
#define TIM7_BASE             (APBPERIPH_BASE + 0x00001400UL)
#define TIM14_BASE            (APBPERIPH_BASE + 0x00002000UL)
#define RTC_BASE              (APBPERIPH_BASE + 0x00002800UL)
#define WWDG_BASE             (APBPERIPH_BASE + 0x00002C00UL)
#define IWDG_BASE             (APBPERIPH_BASE + 0x00003000UL)
#define SPI2_BASE             (APBPERIPH_BASE + 0x00003800UL)
#define SPI3_BASE             (APBPERIPH_BASE + 0x00003C00UL)
#define USART2_BASE           (APBPERIPH_BASE + 0x00004400UL)
#define USART3_BASE           (APBPERIPH_BASE + 0x00004800UL)
#define USART4_BASE           (APBPERIPH_BASE + 0x00004C00UL)
#define USART5_BASE           (APBPERIPH_BASE + 0x00005000UL)
#define I2C1_BASE             (APBPERIPH_BASE + 0x00005400UL)
#define I2C2_BASE             (APBPERIPH_BASE + 0x00005800UL)
#define USB_BASE              (APBPERIPH_BASE + 0x00005C00UL)  /*!< USB_IP Peripheral Registers base address */
#define FDCAN1_BASE           (APBPERIPH_BASE + 0x00006400UL)
#define FDCAN_CONFIG_BASE     (APBPERIPH_BASE + 0x00006500UL)  /*!< FDCAN configuration registers base address */
#define FDCAN2_BASE           (APBPERIPH_BASE + 0x00006800UL)
#define CRS_BASE              (APBPERIPH_BASE + 0x00006C00UL)
#define PWR_BASE              (APBPERIPH_BASE + 0x00007000UL)
#define DAC1_BASE             (APBPERIPH_BASE + 0x00007400UL)
#define DAC_BASE              (APBPERIPH_BASE + 0x00007400UL) /* Kept for legacy purpose */
#define CEC_BASE              (APBPERIPH_BASE + 0x00007800UL)
#define LPTIM1_BASE           (APBPERIPH_BASE + 0x00007C00UL)
#define LPUART1_BASE          (APBPERIPH_BASE + 0x00008000UL)
#define LPUART2_BASE          (APBPERIPH_BASE + 0x00008400UL)
#define I2C3_BASE             (APBPERIPH_BASE + 0x00008800UL)
#define LPTIM2_BASE           (APBPERIPH_BASE + 0x00009400UL)
#define USB_DRD_BASE          (APBPERIPH_BASE + 0x00005C00UL) /*!< USB_DRD_IP Peripheral Registers base address */
#define USB_DRD_PMAADDR       (APBPERIPH_BASE + 0x00009800UL) /*!< USB_DRD_IP Packet Memory Area base address */
#define UCPD1_BASE            (APBPERIPH_BASE + 0x0000A000UL)
#define UCPD2_BASE            (APBPERIPH_BASE + 0x0000A400UL)
#define TAMP_BASE             (APBPERIPH_BASE + 0x0000B000UL)
#define SRAMCAN_BASE          (APBPERIPH_BASE + 0x0000B400UL)
#define SYSCFG_BASE           (APBPERIPH_BASE + 0x00010000UL)
#define VREFBUF_BASE          (APBPERIPH_BASE + 0x00010030UL)
#define COMP1_BASE            (SYSCFG_BASE + 0x0200UL)
#define COMP2_BASE            (SYSCFG_BASE + 0x0204UL)
#define COMP3_BASE            (SYSCFG_BASE + 0x0208UL)
#define ADC1_BASE             (APBPERIPH_BASE + 0x00012400UL)
#define ADC1_COMMON_BASE      (APBPERIPH_BASE + 0x00012708UL)
#define ADC_BASE              (ADC1_COMMON_BASE) /* Kept for legacy purpose */
#define TIM1_BASE             (APBPERIPH_BASE + 0x00012C00UL)
#define SPI1_BASE             (APBPERIPH_BASE + 0x00013000UL)
#define USART1_BASE           (APBPERIPH_BASE + 0x00013800UL)
#define USART6_BASE           (APBPERIPH_BASE + 0x00013C00UL)
#define TIM15_BASE            (APBPERIPH_BASE + 0x00014000UL)
#define TIM16_BASE            (APBPERIPH_BASE + 0x00014400UL)
#define TIM17_BASE            (APBPERIPH_BASE + 0x00014800UL)
#define DBG_BASE              (APBPERIPH_BASE + 0x00015800UL)

#define GPIOA_BASE            (IOPORT_BASE + 0x00000000UL)
#define GPIOB_BASE            (IOPORT_BASE + 0x00000400UL)
#define GPIOC_BASE            (IOPORT_BASE + 0x00000800UL)
#define GPIOD_BASE            (IOPORT_BASE + 0x00000C00UL)
#define GPIOE_BASE            (IOPORT_BASE + 0x00001000UL)
#define GPIOF_BASE            (IOPORT_BASE + 0x00001400UL)

#define TIM2                ((TIM_TypeDef *) TIM2_BASE)
#define TIM3                ((TIM_TypeDef *) TIM3_BASE)
#define TIM4                ((TIM_TypeDef *) TIM4_BASE)
#define TIM6                ((TIM_TypeDef *) TIM6_BASE)
#define TIM7                ((TIM_TypeDef *) TIM7_BASE)
#define TIM14               ((TIM_TypeDef *) TIM14_BASE)
#define CRS                 ((CRS_TypeDef *) CRS_BASE)
#define RTC                 ((RTC_TypeDef *) RTC_BASE)
#define TAMP                ((TAMP_TypeDef *) TAMP_BASE)
#define WWDG                ((WWDG_TypeDef *) WWDG_BASE)
#define IWDG                ((IWDG_TypeDef *) IWDG_BASE)
#define SPI2                ((SPI_TypeDef *) SPI2_BASE)
#define SPI3                ((SPI_TypeDef *) SPI3_BASE)
#define USART2              ((USART_TypeDef *) USART2_BASE)
#define USART3              ((USART_TypeDef *) USART3_BASE)
#define USART4              ((USART_TypeDef *) USART4_BASE)
#define USART5              ((USART_TypeDef *) USART5_BASE)
#define USART6              ((USART_TypeDef *) USART6_BASE)
#define I2C1                ((I2C_TypeDef *) I2C1_BASE)
#define I2C2                ((I2C_TypeDef *) I2C2_BASE)
#define I2C3                ((I2C_TypeDef *) I2C3_BASE)
#define USB_DRD_FS          ((USB_DRD_TypeDef *) USB_DRD_BASE)
#define USB_DRD_PMA_BUFF    ((USB_DRD_PMABuffDescTypeDef*) USB_DRD_PMAADDR)
#define FDCAN1              ((FDCAN_GlobalTypeDef *) FDCAN1_BASE)
#define FDCAN_CONFIG        ((FDCAN_Config_TypeDef *) FDCAN_CONFIG_BASE)
#define FDCAN2              ((FDCAN_GlobalTypeDef *) FDCAN2_BASE)
#define LPTIM1              ((LPTIM_TypeDef *) LPTIM1_BASE)
#define PWR                 ((PWR_TypeDef *) PWR_BASE)
#define RCC                 ((RCC_TypeDef *) RCC_BASE)
#define EXTI                ((EXTI_TypeDef *) EXTI_BASE)
#define DAC1                ((DAC_TypeDef *) DAC1_BASE)
#define DAC                 ((DAC_TypeDef *) DAC_BASE) /* Kept for legacy purpose */
#define LPUART1             ((USART_TypeDef *) LPUART1_BASE)
#define LPTIM2              ((LPTIM_TypeDef *) LPTIM2_BASE)
#define LPUART2             ((USART_TypeDef *) LPUART2_BASE)
#define CEC                 ((CEC_TypeDef *) CEC_BASE)
#define SYSCFG              ((SYSCFG_TypeDef *) SYSCFG_BASE)
#define VREFBUF             ((VREFBUF_TypeDef *) VREFBUF_BASE)
#define COMP1               ((COMP_TypeDef *) COMP1_BASE)
#define COMP2               ((COMP_TypeDef *) COMP2_BASE)
#define COMP12_COMMON       ((COMP_Common_TypeDef *) COMP1_BASE)
#define COMP3               ((COMP_TypeDef *) COMP3_BASE)
#define COMP23_COMMON       ((COMP_Common_TypeDef *) COMP2_BASE)
#define TIM1                ((TIM_TypeDef *) TIM1_BASE)
#define SPI1                ((SPI_TypeDef *) SPI1_BASE)
#define USART1              ((USART_TypeDef *) USART1_BASE)
#define TIM15               ((TIM_TypeDef *) TIM15_BASE)
#define TIM16               ((TIM_TypeDef *) TIM16_BASE)
#define TIM17               ((TIM_TypeDef *) TIM17_BASE)
#define DMA1                ((DMA_TypeDef *) DMA1_BASE)
#define DMA2                ((DMA_TypeDef *) DMA2_BASE)
#define FLASH               ((FLASH_TypeDef *) FLASH_R_BASE)
#define CRC                 ((CRC_TypeDef *) CRC_BASE)
#define GPIOA               ((GPIO_TypeDef *) GPIOA_BASE)
#define GPIOB               ((GPIO_TypeDef *) GPIOB_BASE)
#define GPIOC               ((GPIO_TypeDef *) GPIOC_BASE)
#define GPIOD               ((GPIO_TypeDef *) GPIOD_BASE)
#define GPIOE               ((GPIO_TypeDef *) GPIOE_BASE)
#define GPIOF               ((GPIO_TypeDef *) GPIOF_BASE)
#define ADC1                ((ADC_TypeDef *) ADC1_BASE)
#define ADC1_COMMON         ((ADC_Common_TypeDef *) ADC1_COMMON_BASE)
#define ADC                 (ADC1_COMMON) /* Kept for legacy purpose */

#define DMA1_BASE             (0UL)
#define DMA2_BASE             (0UL)
#define DMAMUX1_BASE          (0UL)
#define RCC_BASE              (0UL)
#define EXTI_BASE             (0UL)
#define FLASH_R_BASE          (0UL)
#define CRC_BASE              (0UL)


#define DMA1_Channel1_BASE    (DMA1_BASE + 0x00000008UL)
#define DMA1_Channel2_BASE    (DMA1_BASE + 0x0000001CUL)
#define DMA1_Channel3_BASE    (DMA1_BASE + 0x00000030UL)
#define DMA1_Channel4_BASE    (DMA1_BASE + 0x00000044UL)
#define DMA1_Channel5_BASE    (DMA1_BASE + 0x00000058UL)
#define DMA1_Channel6_BASE    (DMA1_BASE + 0x0000006CUL)
#define DMA1_Channel7_BASE    (DMA1_BASE + 0x00000080UL)
#define DMA2_Channel1_BASE    (DMA2_BASE + 0x00000008UL)
#define DMA2_Channel2_BASE    (DMA2_BASE + 0x0000001CUL)
#define DMA2_Channel3_BASE    (DMA2_BASE + 0x00000030UL)
#define DMA2_Channel4_BASE    (DMA2_BASE + 0x00000044UL)
#define DMA2_Channel5_BASE    (DMA2_BASE + 0x00000058UL)

#define LL_SYSCFG_I2C_FASTMODEPLUS_PB6      0
#define LL_SYSCFG_I2C_FASTMODEPLUS_PB7      0
#define LL_SYSCFG_I2C_FASTMODEPLUS_PB8      0
#define LL_IOP_GRP1_PERIPH_GPIOB            0
#define LL_APB1_GRP1_PERIPH_SPI2            0

#define LL_TIM_CHANNEL_CH1                  0
#define LL_TIM_CHANNEL_CH1N                 0
#define LL_TIM_CHANNEL_CH2                  0
#define LL_TIM_CHANNEL_CH2N                 0
#define LL_TIM_CHANNEL_CH3                  0
#define LL_TIM_CHANNEL_CH3N                 0
#define LL_TIM_CHANNEL_CH4                  0
#define LL_TIM_CHANNEL_CH5                  0
#define LL_TIM_CHANNEL_CH6                  0

#define TLC5955_SPI2_LAT_Pin (0x1UL << 9U)
#define TLC5955_SPI2_LAT_GPIO_Port GPIOB
#define SPI1_DC_Pin (0x1UL << 0U)
#define SPI1_DC_GPIO_Port GPIOA
#define SPI1_RESET_Pin (1U << 3U)
#define SPI1_RESET_GPIO_Port GPIOA
#define I2C3_INT_Pin (0x1UL << 5U)
#define I2C3_INT_GPIO_Port GPIOA
#define I2C3_INT_EXTI_IRQn 7
#define TLC5955_SPI2_GSCLK_Pin (0x1UL << 6U)
#define TLC5955_SPI2_GSCLK_GPIO_Port GPIOB
#define TLC5955_SPI2_MOSI_Pin (0x1UL << 7U)
#define TLC5955_SPI2_MOSI_GPIO_Port GPIOB
#define TLC5955_SPI2_SCK_Pin (0x1UL << 8U)
#define TLC5955_SPI2_SCK_GPIO_Port GPIOB

#endif // __MOCK_CMSIS_HPP__