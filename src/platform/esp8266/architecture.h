#pragma once

#define ARCH_ESP8266

//
// Capabilities for ESP8266: LoRa + serial only, no BLE/WiFi/display/sensors
//

#define HAS_BLUETOOTH 0
#define HAS_WIFI 0
#define HAS_SCREEN 0
#define HAS_WIRE 0
#define HAS_GPS 0
#define HAS_BUTTON 0
#define HAS_TELEMETRY 0
#define HAS_SENSOR 0
#define HAS_RADIO 1
#define HAS_CPU_SHUTDOWN 0
// No hardware AES on ESP8266 — leave HAS_CUSTOM_CRYPTO_ENGINE undefined so
// CryptoEngine.cpp's #ifndef guard instantiates the default software engine.

#define HW_VENDOR meshtastic_HardwareModel_PRIVATE_HW

// LoRa SPI defaults for D1 Mini / NodeMCU (override in variant.h as needed)
#ifndef LORA_SCK
#define LORA_SCK 14 // D5
#endif
#ifndef LORA_MISO
#define LORA_MISO 12 // D6
#endif
#ifndef LORA_MOSI
#define LORA_MOSI 13 // D7
#endif
#ifndef LORA_CS
#define LORA_CS 15 // D8
#endif
