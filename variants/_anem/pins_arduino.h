#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <stdint.h>

static const uint8_t TX = 21;
static const uint8_t RX = 20;

#define BATTERY_PIN 1
#define ADC_CHANNEL ADC1_GPIO0_CHANNEL
#define ADC_MULTIPLIER  3.2

static const uint8_t SDA = 18;
static const uint8_t SCL = 19;
#define I2C_SDA SDA
#define I2C_SCL SCL
#define I2C_SPEED 10000

static const uint8_t SS = 8;
static const uint8_t MOSI = 7;
static const uint8_t MISO = 6;
static const uint8_t SCK = 10;

static const uint8_t A0 = 0;
static const uint8_t A1 = 1;
static const uint8_t A2 = 2;
static const uint8_t A3 = 3;
static const uint8_t A4 = 4;
static const uint8_t A5 = 5;

#endif /* Pins_Arduino_h */