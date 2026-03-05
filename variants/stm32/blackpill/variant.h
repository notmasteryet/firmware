#pragma once

// https://github.com/stm32duino/Arduino_Core_STM32/blob/main/variants/STM32F4xx/F411C(C-E)(U-Y)/variant_BLACKPILL_F411CE.h
#define LED_PIN LED_BUILTIN
// #define LED_STATE_ON 1

#define BUTTON_PIN USER_BTN


// I2C
#define I2C_SDA PIN_WIRE_SDA
#define I2C_SCL PIN_WIRE_SCL

#define SERIAL_PRINT_PORT 1

// SPI
// #define SPI_INTERFACES_COUNT 1
// #define PIN_SPI_MISO 11
// #define PIN_SPI_MOSI 10
// #define PIN_SPI_SCK 9

#define USE_SX1262

// LORA CONFIG
#define SX126X_CS    PA3
#define SX126X_DIO1  PA2     // IRQ
#define SX126X_DIO2_AS_RF_SWITCH // Note for E22 modules: DIO2 is not attached internally to TXEN for automatic TX/RX switching,
                                 // so it needs connecting externally if it is used in this way
#define SX126X_BUSY  PB0
#define SX126X_RESET PA1
#define SX126X_RXEN RADIOLIB_NC
#define SX126X_TXEN RADIOLIB_NC  // Assuming that DIO2 is connected to TXEN pin. If not, TXEN must be connected.
