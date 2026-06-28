#include "EventSwitchModule.h"

#ifdef EVENT_SWITCH_PIN

#include "Channels.h"
#include "EventMode.h"
#include "MeshService.h"
#include "NodeDB.h"
#include "Router.h"
#include "configuration.h"
#include "target_specific.h" // setBluetoothEnable()
#include <Arduino.h>
#include <string.h>

#ifndef EVENT_OUTPUT_PIN
#error "EVENT_OUTPUT_PIN must be defined in the variant alongside EVENT_SWITCH_PIN"
#endif
#ifndef EVENT_CHANNEL_NAME
#define EVENT_CHANNEL_NAME "event"
#endif
#ifndef EVENT_MESSAGE_TEXT
#define EVENT_MESSAGE_TEXT "EVENT"
#endif
// Delay before the event message is sent, to give the LoRa interface time to come up.
#ifndef EVENT_SEND_DELAY_MS
#define EVENT_SEND_DELAY_MS 5000
#endif

// Definitions of the global event-mode flags declared in EventMode.h
namespace EventMode
{
bool active = false;
bool bypass = false;
} // namespace EventMode

EventSwitchModule::EventSwitchModule() : concurrency::OSThread("EventSwitch")
{
    // Output starts LOW (no event sent yet).
    pinMode(EVENT_OUTPUT_PIN, OUTPUT);
    digitalWrite(EVENT_OUTPUT_PIN, LOW);

    // Switch is read once, here at boot. Internal pull-up: closed-to-GND (LOW) means event mode.
    pinMode(EVENT_SWITCH_PIN, INPUT_PULLUP);
    eventActive = (digitalRead(EVENT_SWITCH_PIN) == LOW);

    if (eventActive) {
        LOG_INFO("EventSwitch: EVENT mode latched at boot (pin %d LOW)", EVENT_SWITCH_PIN);
        // Go quiet immediately; the single event message below bypasses this gate.
        EventMode::active = true;

        // Dormant node: no radios besides the one outgoing message, no phone link, just logs.
        // These are RAM-only edits — the switch is re-read at the next boot and config saves are
        // blocked while EventMode::active (see NodeDB::saveToDisk), so nothing here persists to flash.
        config.bluetooth.enabled = false; // no BLE (prevents advertising from boot; see setBluetoothEnable)
#if HAS_WIFI
        config.network.wifi_enabled = false; // no WiFi
#endif
        config.device.rebroadcast_mode =
            meshtastic_Config_DeviceConfig_RebroadcastMode_NONE; // don't relay mesh traffic either

        setIntervalFromNow(EVENT_SEND_DELAY_MS);
    } else {
        LOG_INFO("EventSwitch: STANDARD mode (pin %d HIGH)", EVENT_SWITCH_PIN);
        disable();
    }
}

int32_t EventSwitchModule::runOnce()
{
    if (!eventActive || done)
        return disable();

    // Resolve the target channel by name; fall back to the primary channel if not found.
    int chIndex = -1;
    for (ChannelIndex i = 0; i < channels.getNumChannels(); i++) {
        if (strcasecmp(channels.getName(i), EVENT_CHANNEL_NAME) == 0) {
            chIndex = i;
            break;
        }
    }
    if (chIndex < 0) {
        chIndex = channels.getPrimaryIndex();
        LOG_WARN("EventSwitch: channel '%s' not found, using primary (%d)", EVENT_CHANNEL_NAME, chIndex);
    }

    const char *msg = EVENT_MESSAGE_TEXT;
    meshtastic_MeshPacket *p = router->allocForSending();
    p->to = NODENUM_BROADCAST;
    p->channel = (uint8_t)chIndex;
    p->want_ack = false;
    p->decoded.portnum = meshtastic_PortNum_TEXT_MESSAGE_APP;
    p->decoded.payload.size = strlen(msg);
    memcpy(p->decoded.payload.bytes, msg, p->decoded.payload.size);

    // Let this one packet through the "go quiet" suppression gate.
    EventMode::bypass = true;
    service->sendToMesh(p, RX_SRC_LOCAL, true);
    EventMode::bypass = false;

    // Make sure BLE is actually down now (config gate above keeps it from re-advertising).
    setBluetoothEnable(false);

    // Signal that the event message was sent.
    digitalWrite(EVENT_OUTPUT_PIN, HIGH);
    LOG_INFO("EventSwitch: sent '%s' on ch %d; output pin %d HIGH; node dormant (no BLE/WiFi/relay)", msg, chIndex,
             EVENT_OUTPUT_PIN);

    done = true;
    return disable();
}

#endif // EVENT_SWITCH_PIN
