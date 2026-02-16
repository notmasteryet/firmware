#ifndef _VARIANT_ESP32C3_SUPER_MINI_
#define _VARIANT_ESP32C3_SUPER_MINI_

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// I2C (Wire) & OLED
#define WIRE_INTERFACES_COUNT (1)
#define I2C_SDA (8)
#define I2C_SCL (9)

#define USE_SSD1306

// GPS
#undef GPS_RX_PIN
#undef GPS_TX_PIN
#define GPS_RX_PIN (20)
#define GPS_TX_PIN (21)

// Button
#define BUTTON_PIN (9) // BOOT button

// LoRa
#define USE_LR1121

#define LORA_DIO0 RADIOLIB_NC
#define LORA_RESET (2)
#define LORA_DIO1 (1)
#define LORA_BUSY (3)
#define LORA_SCK (4)
#define LORA_MISO (5)
#define LORA_MOSI (6)
#define LORA_CS (7)

#define LR1121_IRQ_PIN LORA_DIO1
#define LR1121_NRESET_PIN LORA_RESET
#define LR1121_BUSY_PIN LORA_BUSY
#define LR1121_SPI_NSS_PIN LORA_CS
#define LR1121_SPI_SCK_PIN LORA_SCK
#define LR1121_SPI_MOSI_PIN LORA_MOSI
#define LR1121_SPI_MISO_PIN LORA_MISO
#define LR11X0_DIO3_TCXO_VOLTAGE 1.8
#define LR11X0_DIO_AS_RF_SWITCH

#ifdef __cplusplus
}
#endif

/*----------------------------------------------------------------------------
 *        Arduino objects - C++ only
 *----------------------------------------------------------------------------*/

#endif
