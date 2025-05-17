#include "AnemometerModule.h"
#include "Default.h"
#include "MeshService.h"
#include "NodeDB.h"
#include "PowerFSM.h"
#include "configuration.h"
#include "main.h"
#include <Throttle.h>
AnemometerModule *anemometerModule;

#define POLLING_INTERVAL 45000

int32_t AnemometerModule::runOnce()
{
    if (firstTime) {
        firstTime = false;
        LOG_INFO("Anemometer Module: init");
        return setStartDelay();
    }


    Wire.requestFrom(0x50, 2, 1);
    uint16_t value = (int)Wire.read();
    value |= ((int)Wire.read()) << 8;
    LOG_INFO("Anemometer Module: value=%u", value);
    uint32_t read = millis();

    if (!wasInitialized) {
        lastSentToMesh = millis();
        lastValue = value;
        lastValueRead = read;
        wasInitialized = true;
        return POLLING_INTERVAL;
    }

    buf[bufEnd] = ((value - lastValue) & 0xFFFF) * 1000.0f / (read - lastValueRead);
    bufEnd = (bufEnd + 1) % NUM_ENTRIES_WITH_RESERVE;
    lastValue = value;
    lastValueRead = read;

    const int state_broadcast_secs = (POLLING_INTERVAL / 1000) * NUM_ENTRIES;
    // Even if we haven't detected an event, broadcast our current state to the mesh on the scheduled interval as a sort
    if (state_broadcast_secs > 0 &&
        !Throttle::isWithinTimespanMs(lastSentToMesh,
                                      Default::getConfiguredOrDefaultMs(
                                        state_broadcast_secs,  default_telemetry_broadcast_interval_secs))) {

        sendCurrentStateMessage();
    }
    return POLLING_INTERVAL;
}

void AnemometerModule::sendCurrentStateMessage()
{
    char *message = new char[200];
    char *pm = message + sprintf(message, "%s:", "ANEMM");
    while (bufPos != bufEnd) {
        pm += sprintf(pm, " %.3f", buf[bufPos]);
        bufPos = (bufPos + 1) % NUM_ENTRIES_WITH_RESERVE;
    }

    meshtastic_MeshPacket *p = allocDataPacket();
    p->want_ack = false;
    p->decoded.payload.size = strlen(message);
    memcpy(p->decoded.payload.bytes, message, p->decoded.payload.size);
    lastSentToMesh = millis();
    if (!channels.isDefaultChannel(0)) {
        LOG_INFO("Send message id=%d, dest=%x, msg=%.*s", p->id, p->to, p->decoded.payload.size, p->decoded.payload.bytes);
        service->sendToMesh(p);
    } else
        LOG_ERROR("Message not allow on Public channel");
    delete[] message;
}
