// Battery monitoring via voltage divider on GPIO 0 (ADC1 CH0)
#define BATTERY_PIN 0
#define ADC_MULTIPLIER 3.27 // calibrated: 7.5V bat / 2.3V on pin (1M+470k divider)
#define NUM_CELLS 2

#define ROTARY_A 20
#define ROTARY_B 23
#define ROTARY_PRESS 1

#define BUTTON_PIN 9

#define HAS_NEOPIXEL                         // Enable the use of neopixels
#define NEOPIXEL_COUNT 1                     // How many neopixels are connected
#define NEOPIXEL_DATA 8                      // gpio pin used to send data to the neopixels
#define NEOPIXEL_TYPE (NEO_RGB + NEO_KHZ800) // type of neopixels in use
#define ENABLE_AMBIENTLIGHTING               // Turn on Ambient Lighting

// Default SPI2 will be mapped to the display
#define ST7789_SDA 6
#define ST7789_SCK 7
#define ST7789_CS 14
#define ST7789_RS 15
#define ST7789_BL 22
#define USE_TFTDISPLAY 1

#define BRIGHTNESS_DEFAULT 100        // Medium Low Brightness

#define ST7789_RESET 21
//#define ST7789_MISO -1
#define ST7789_MISO 5 // Make SPI happy
#define ST7789_BUSY -1
#define ST7789_SPI_HOST SPI2_HOST
#define TFT_BL ST7789_BL
#define SPI_FREQUENCY 40000000
#define SPI_READ_FREQUENCY 16000000
#define TFT_HEIGHT 320
#define TFT_WIDTH 172
#define TFT_OFFSET_X 34
#define TFT_OFFSET_Y 0
#define TFT_OFFSET_ROTATION 0
#define SCREEN_ROTATE
#define SCREEN_TRANSITION_FRAMERATE 5

#define USE_SX1262
#define LORA_SCK 7
#define LORA_MISO 5
#define LORA_MOSI 6
#define LORA_CS 4
#define LORA_RESET 3
#define SX126X_CS LORA_CS
#define SX126X_DIO1 18
#define SX126X_BUSY 19
#define SX126X_RESET LORA_RESET
#define SX126X_RXEN 2
#define SX126X_DIO2_AS_RF_SWITCH
#define SX126X_DIO3_TCXO_VOLTAGE 1.8

#define SERIAL_PRINT_PORT 1
