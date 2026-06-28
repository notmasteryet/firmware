#pragma once

#include "mesh/MeshTypes.h"

// Global state for the "event-switch" custom feature (see variant EVENT_SWITCH_PIN).
// When the node boots into event mode it sends one message and then "goes quiet":
// all locally-originated transmissions are dropped at the MeshService chokepoint so the
// node stops emitting its normal periodic traffic (position/telemetry/nodeinfo/etc).
namespace EventMode
{
// True once the node has latched event mode at boot. While set, local-origin TX is suppressed.
extern bool active;
// Transient flag set only around the single event message so it bypasses the suppression gate.
extern bool bypass;

// Returns true if this locally-originated packet should be dropped while in event mode.
inline bool suppressLocalSend(RxSource src)
{
    return active && !bypass && src == RX_SRC_LOCAL;
}
} // namespace EventMode
