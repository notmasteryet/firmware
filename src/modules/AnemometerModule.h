#pragma once
#include "SinglePortModule.h"

class AnemometerModule : public SinglePortModule, private concurrency::OSThread
{
  public:
    AnemometerModule() : SinglePortModule("text", meshtastic_PortNum_TEXT_MESSAGE_APP), OSThread("Anemometer")
    {
    }

  protected:
    virtual int32_t runOnce() override;

  private:
    bool firstTime = true;
    bool wasInitialized = false;
    uint32_t lastSentToMesh = 0;
    uint32_t lastValueRead;
    uint16_t lastValue;

    static constexpr size_t NUM_ENTRIES = 8;
    static constexpr size_t NUM_ENTRIES_WITH_RESERVE = 16;
    uint8_t bufPos = 0;
    uint8_t bufEnd = 0;
    float buf[NUM_ENTRIES_WITH_RESERVE];

    void sendCurrentStateMessage();
};

extern AnemometerModule *anemometerModule;