#include "RadioLib.h"

// This is rewritten to match the requirements of the E80-900M2213S
// The E80 does not conform to the reference Semtech switches(!) and therefore needs a custom matrix.
// See footnote #3 in "https://www.cdebyte.com/products/E80-900M2213S/2#Pin"
// RF Switch Matrix SubG RFO_HP_LF / RFO_LP_LF / RFI_[NP]_LF0
// DIO5 -> RFSW0_V1
// DIO6 -> RFSW1_V2
// DIO7 -> not connected on E80 module - note that GNSS and Wifi scanning are not possible.

static const uint32_t rfswitch_dio_pins[] = {RADIOLIB_LR11X0_DIO5, RADIOLIB_LR11X0_DIO6, RADIOLIB_NC, RADIOLIB_NC,
                                             RADIOLIB_NC};

static const Module::RfSwitchMode_t rfswitch_table[] = {
    // mode             DIO5  DIO6
    {LR11x0::MODE_STBY, {LOW, LOW}},  {LR11x0::MODE_RX, {LOW, LOW}},
    {LR11x0::MODE_TX, {LOW, HIGH}},  {LR11x0::MODE_TX_HP, {HIGH, LOW}},
    {LR11x0::MODE_TX_HF, {LOW, LOW}}, {LR11x0::MODE_GNSS, {LOW, LOW}},
    {LR11x0::MODE_WIFI, {LOW, LOW}},  END_OF_MODE_TABLE,
};
