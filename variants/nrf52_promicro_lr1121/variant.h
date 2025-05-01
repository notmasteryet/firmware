#ifndef _VARIANT_PROMICRO_DIY_
#define _VARIANT_PROMICRO_DIY_

/** Master clock frequency */
#define VARIANT_MCK (64000000ul)

// #define USE_LFXO // Board uses 32khz crystal for LF
#define USE_LFRC // Board uses RC for LF

#define PROMICRO_LR1121

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "WVariant.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// Number of pins defined in PinDescription array
#define PINS_COUNT (48)
#define NUM_DIGITAL_PINS (48)
#define NUM_ANALOG_INPUTS (1)
#define NUM_ANALOG_OUTPUTS (0)

// Pin 13 enables 3.3V periphery. If the Lora module is on this pin, then it should stay enabled at all times.
#define PIN_3V3_EN (0 + 13) // P0.13

// Analog pins
#define BATTERY_PIN (0 + 31) // P0.31 Battery ADC
#define ADC_CHANNEL ADC1_GPIO4_CHANNEL
#define ADC_RESOLUTION 14
#define BATTERY_SENSE_RESOLUTION_BITS 12
#define BATTERY_SENSE_RESOLUTION 4096.0
// Definition of milliVolt per LSB => 3.0V ADC range and 12-bit ADC resolution = 3000mV/4096
#define VBAT_MV_PER_LSB (0.73242188F)
// Voltage divider value => 1.5M + 1M voltage divider on VBAT = (1.5M / (1M + 1.5M))
#define VBAT_DIVIDER (0.6F)
// Compensation factor for the VBAT divider
#define VBAT_DIVIDER_COMP (1.73)
// Fixed calculation of milliVolt from compensation value
#define REAL_VBAT_MV_PER_LSB (VBAT_DIVIDER_COMP * VBAT_MV_PER_LSB)
#undef AREF_VOLTAGE
#define AREF_VOLTAGE 3.0
#define VBAT_AR_INTERNAL AR_INTERNAL_3_0
#define ADC_MULTIPLIER VBAT_DIVIDER_COMP // REAL_VBAT_MV_PER_LSB
#define VBAT_RAW_TO_SCALED(x) (REAL_VBAT_MV_PER_LSB * x)

// WIRE IC AND IIC PINS
#define WIRE_INTERFACES_COUNT 1

// #define PIN_WIRE_SDA (32 + 4) // P1.04
// #define PIN_WIRE_SCL (0 + 11) // P0.11
#define PIN_WIRE_SDA (0 + 17)
#define PIN_WIRE_SCL (0 + 20)

// LED
#define PIN_LED1 (0 + 15) // P0.15
#define LED_BUILTIN PIN_LED1
// Actually red
#define LED_BLUE PIN_LED1
#define LED_STATE_ON 1 // State when LED is lit

// Button
#define BUTTON_PIN (32 + 1) // P1.01

#define NO_GPS 1

// UART interfaces
#define PIN_SERIAL1_RX (0 + 6) // P0.06
#define PIN_SERIAL1_TX (0 + 8) // P0.08

// Serial interfaces
#define SPI_INTERFACES_COUNT 1

#define PIN_SPI_MISO (0 + 2)   // P0.02
#define PIN_SPI_MOSI (32 + 15) // P1.15
#define PIN_SPI_SCK (32 + 11)  // P1.11

#define LORA_MISO PIN_SPI_MISO
#define LORA_MOSI PIN_SPI_MOSI
#define LORA_SCK PIN_SPI_SCK
#define LORA_CS (32 + 13) // P1.13

// LORA MODULES
#define USE_LR1121

#define USE_SSD1306

// LR1121
#ifdef USE_LR1121
#define LR1121_IRQ_PIN (0 + 10)      // P0.10 IRQ
#define LR1121_NRESET_PIN (0 + 9) // P0.09 NRST
#define LR1121_BUSY_PIN (0 + 29)     // P0.29 BUSY
#define LR1121_SPI_NSS_PIN LORA_CS   // P1.13
#define LR1121_SPI_SCK_PIN LORA_SCK
#define LR1121_SPI_MOSI_PIN LORA_MOSI
#define LR1121_SPI_MISO_PIN LORA_MISO
#define LR11X0_DIO3_TCXO_VOLTAGE 1.8
#define LR11X0_DIO_AS_RF_SWITCH
#endif

/*
On the SX1262, DIO3 sets the voltage for an external TCXO, if one is present. If one is not present, use TCXO_OPTIONAL to try both
settings.

| Mfr          | Module           | TCXO | RF Switch | Notes                                 |
| ------------ | ---------------- | ---- | --------- | ------------------------------------- |
| Ebyte        | E80-900M2213S    | Yes  | Int       | LR1121 radio                          |

*/

#ifdef __cplusplus
}
#endif

/*----------------------------------------------------------------------------
 *        Arduino objects - C++ only
 *----------------------------------------------------------------------------*/

#endif
