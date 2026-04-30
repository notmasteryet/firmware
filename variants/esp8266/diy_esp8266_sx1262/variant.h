#pragma once

// SX1262 SPI interface
#define LORA_SCK 14  // D5
#define LORA_MISO 12 // D6
#define LORA_MOSI 13 // D7
// GPIO15 (D8) is the ESP8266 hardware SPI SS pin. SPI.begin() configures it as
// a hardware-driven SS output, so it must be the CS. The D1 Mini has a 10 kΩ
// pull-DOWN on GPIO15, keeping it LOW at boot (normal flash mode) even if the
// RA-01sh NSS idles HIGH.
#define LORA_CS 15   // D8 — hardware SPI SS

// SX1262 control pins
#define LORA_DIO1 16
#define LORA_RESET 4
#define LORA_BUSY 5

// Map LORA_* to the SX126X_* names expected by RadioInterface.cpp
#define USE_SX1262
#define SX126X_CS    LORA_CS
#define SX126X_DIO1  LORA_DIO1
#define SX126X_RESET LORA_RESET
#define SX126X_BUSY  LORA_BUSY
// RA-01sh: DIO2 drives the integrated RF switch (TX/RX selection)
#define SX126X_DIO2_AS_RF_SWITCH
// RA-01sh has a TCXO powered from DIO3 at 1.8 V.
#define SX126X_DIO3_TCXO_VOLTAGE 1.8

// Serial control (USB serial on D1 Mini)
#define SerialMesh Serial

// GPIO2 is the built-in LED on D1 Mini, active LOW
#define LED_POWER 2
#define LED_STATE_ON 0

// No display, GPS, I2C, or user button on this variant
#undef HAS_SCREEN
#define HAS_SCREEN 0
#undef HAS_GPS
#define HAS_GPS 0
#undef HAS_WIRE
#define HAS_WIRE 0
#undef HAS_BUTTON
#define HAS_BUTTON 0
