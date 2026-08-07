/*
 * This file is part of Betaflight.
 *
 * Betaflight is free software. You can redistribute this software
 * and/or modify this software under the terms of the GNU General
 * Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later
 * version.
 *
 * Betaflight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this software.
 *
 * If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#ifndef TARGET_BOARD_IDENTIFIER
#define TARGET_BOARD_IDENTIFIER "H757"
#endif

#ifndef USBD_PRODUCT_STRING
#define USBD_PRODUCT_STRING     "Betaflight - GD32H757"
#endif

#ifndef GD32H757
#define GD32H757
#endif

#define USE_I2C_DEVICE_0
#define USE_I2C_DEVICE_1
#define USE_I2C_DEVICE_2
#define USE_I2C_DEVICE_3

#define I2C0_CLOCKSPEED 400
#define I2C1_CLOCKSPEED 400
#define I2C2_CLOCKSPEED 400
#define I2C3_CLOCKSPEED 400

#define USE_VCP

#define USE_SOFTSERIAL

#ifdef USE_SOFTSERIAL
#define UNIFIED_SERIAL_PORT_COUNT       3
#else
#define UNIFIED_SERIAL_PORT_COUNT       1
#endif

#define USE_UART0
#define USE_UART1
#define USE_UART2
#define USE_UART3
#define USE_UART4
#define USE_UART5
#define USE_UART6
#define USE_UART7

#ifdef USE_UART0
#define SERIAL_UART_FIRST_INDEX    0
#endif

#define USE_INVERTER

#define USE_SPI_DEVICE_0
#define USE_SPI_DEVICE_1
#define USE_SPI_DEVICE_2
#define USE_SPI_DEVICE_3
#define USE_SPI_DEVICE_4
#define USE_SPI_DEVICE_5

#define TARGET_IO_PORTA 0xffff
#define TARGET_IO_PORTB 0xffff
#define TARGET_IO_PORTC 0xffff
#define TARGET_IO_PORTD 0xffff
#define TARGET_IO_PORTE 0xffff
#define TARGET_IO_PORTF 0xffff

#define LED0_PIN PE5
#define LED1_PIN PE4

#define BEEPER_PIN PE3

#define USB_DETECT_PIN PE6

#define LED_STRIP_PIN PA8

#define UART0_TX_PIN PA9
#define UART0_RX_PIN PA10
#define UART1_TX_PIN PD5
#define UART1_RX_PIN PD6
#define UART2_TX_PIN PD8
#define UART2_RX_PIN PD9
#define UART3_TX_PIN PD1
#define UART3_RX_PIN PD0
#define UART4_TX_PIN PC12
#define UART4_RX_PIN PD2
#define UART5_TX_PIN PC6
#define UART5_RX_PIN PC7
#define UART6_TX_PIN PE8
#define UART6_RX_PIN PE7
#define UART7_TX_PIN PE1
#define UART7_RX_PIN PE2

#define I2C0_SCL_PIN PB6
#define I2C0_SDA_PIN PB7
#define I2C1_SCL_PIN PB10
#define I2C1_SDA_PIN PB11

#define SPI0_SCK_PIN PA5
#define SPI0_SDI_PIN PA6
#define SPI0_SDO_PIN PA7
#define SPI1_SCK_PIN PB13
#define SPI1_SDI_PIN PB14
#define SPI1_SDO_PIN PB15
#define SPI2_SCK_PIN PC10
#define SPI2_SDI_PIN PC11
#define SPI2_SDO_PIN PB2
#define SPI3_SCK_PIN PE12
#define SPI3_SDI_PIN PE13
#define SPI3_SDO_PIN PE14

#define ADC_EXTERNAL1_PIN PC0
#define ADC_VBAT_PIN PC1
#define ADC_RSSI_PIN PC2
#define ADC_CURR_PIN PC3

#define PINIO1_PIN PC5

#define RX_PPM_PIN PB9

#define TIMER_PIN_MAPPING \
    TIMER_PIN_MAP(0, PA0, 1, 0) \
    TIMER_PIN_MAP(1, PA1, 1, 0) \
    TIMER_PIN_MAP(2, PA2, 1, 0) \
    TIMER_PIN_MAP(3, PA3, 1, 0) \
    TIMER_PIN_MAP(4, PB0, 2, 0) \
    TIMER_PIN_MAP(5, PB1, 2, 0) \
    TIMER_PIN_MAP(6, PC8, 7, 0) \
    TIMER_PIN_MAP(7, PC9, 7, 0) \
    TIMER_PIN_MAP(8, PA8, 0, 0) \
    TIMER_PIN_MAP(9, PB9, 3, 0)

#define USE_I2C
#define I2C_FULL_RECONFIGURABILITY

// #define USE_DSHOT_BITBAND

#define USE_BEEPER

#define USE_PINIO

#ifdef USE_SDCARD
#ifndef USE_SDCARD_SDIO
#define USE_SDCARD_SPI
#endif
#if !defined(ENABLE_SDIO_INIT)
#define ENABLE_SDIO_INIT 1
#endif
#if !defined(ENABLE_SDIO_PIN_CONFIG)
#define ENABLE_SDIO_PIN_CONFIG 1
#endif
#endif

#define USE_SPI
#define SPI_FULL_RECONFIGURABILITY
#define USE_SPI_DMA_ENABLE_LATE

#define USE_USB_DETECT

#define USE_ESCSERIAL

#define USE_ADC
#ifndef ADC_INSTANCE
#define ADC_INSTANCE                ADC0
#endif

#define USE_EXTI

// #define USE_TIMER_UP_CONFIG  //TODO

#define FLASH_PAGE_SIZE ((uint32_t)0x1000) // 4K sectors

#if !defined(ADC0_DMA_OPT)
#define ADC0_DMA_OPT (DMA_OPT_UNUSED)
#endif

#if defined(USE_LED_STRIP) && !defined(USE_LED_STRIP_CACHE_MGMT)
#define USE_LED_STRIP_CACHE_MGMT
#endif
