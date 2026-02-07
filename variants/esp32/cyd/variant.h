
#define I2C_SDA 21
#define I2C_SCL 22

#define BUTTON_PIN 0

#define PIN_LED1 17
#define PIN_LED2 4
#define PIN_LED3 16

#define RGBLED_GREEN PIN_LED1
#define RGBLED_RED PIN_LED2
#define RGBLED_BLUE PIN_LED3
#define RGBLED_CA

#define LED_NOTIFICATION RGBLED_RED
#define LED_POWER RGBLED_GREEN
#define LED_PIN PIN_LED1

#define LED_STATE_ON 0
#define ledOff(pin) digitalWrite(pin, HIGH ^ LED_STATE_ON)

#define ADC_MULTIPLIER 2.48 // 1.48M and 1M

#define BATTERY_PIN 34 // A battery voltage measurement pin, voltage divider connected here to measure battery voltage
#define ADC_CHANNEL ADC1_GPIO34_CHANNEL

#define LORA_DIO0 -1  // a No connect on the SX1262/SX1268 module
#define LORA_RESET 25 // RST for SX1276, and for SX1262/SX1268
#define LORA_DIO1 35  // IRQ for SX1262/SX1268
#define LORA_DIO2 39  // BUSY for SX1262/SX1268
#define LORA_DIO3     // Not connected on PCB, but internally on the TTGO SX1262/SX1268, if DIO3 is high the TXCO is enabled

#undef LORA_SCK
#define LORA_SCK  18
#undef LORA_MISO
#define LORA_MISO 19
#undef LORA_MOSI
#define LORA_MOSI 23
#undef LORA_CS
#define LORA_CS   32

#define USE_LR1121
#define LR1121_IRQ_PIN LORA_DIO1
#define LR1121_NRESET_PIN LORA_RESET
#define LR1121_BUSY_PIN LORA_DIO2
#define LR1121_SPI_NSS_PIN LORA_CS
#define LR1121_SPI_SCK_PIN LORA_SCK
#define LR1121_SPI_MOSI_PIN LORA_MOSI
#define LR1121_SPI_MISO_PIN LORA_MISO
#define LR11X0_DIO3_TCXO_VOLTAGE 3.0
#define LR11X0_DIO_AS_RF_SWITCH

#define TFT_CS 15
#define TFT_BL 27
#define TFT_BUSY -1
#define TOUCH_CS 33

#define ST7789_SPI_HOST HSPI_HOST
#define ST7789_CS TFT_CS
#define ST7789_RS 2  // DC
#define ST7789_SDA 13 // MOSI
#define ST7789_SCK 14
#define ST7789_RESET -1
#define ST7789_MISO 12
#define ST7789_BUSY TFT_BUSY
#define ST7789_BL TFT_BL
#define SPI_FREQUENCY 8000000
#define SPI_READ_FREQUENCY 16000000
#define SPI_TOUCH_FREQUENCY 2500000

#define TFT_WIDTH 240
#define TFT_HEIGHT 320
#define TFT_OFFSET_X 0
#define TFT_OFFSET_Y 0
#define TFT_OFFSET_ROTATION 1

#define TFT_TOUCH_X_MIN 434
#define TFT_TOUCH_X_MAX 3755
#define TFT_TOUCH_Y_MIN 390
#define TFT_TOUCH_Y_MAX 3818
#define TFT_TOUCH_OFFSET_ROTATION 3

#define TFT_DUMMY_READ_PIXELS 8
#define TFT_INVERSION_OFF
#define TFT_RGB_ORDER TFT_BGR
// #define TFT_RGB_ORDER TFT_RGB

#define HAS_TOUCHSCREEN 1

#define USE_TFTDISPLAY 1

#define CANNED_MESSAGE_MODULE_ENABLE 1
#define USE_VIRTUAL_KEYBOARD 1
