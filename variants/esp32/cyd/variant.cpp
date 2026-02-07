#include "configuration.h"

void lateInitVariant()
{
    // LED1 & LED2
    pinMode(PIN_LED1, OUTPUT);
    ledOff(PIN_LED1);

    pinMode(PIN_LED2, OUTPUT);
    ledOff(PIN_LED2);

    pinMode(PIN_LED3, OUTPUT);
    ledOff(PIN_LED3);
}
