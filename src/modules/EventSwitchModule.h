#pragma once

#include "configuration.h"

#ifdef EVENT_SWITCH_PIN

#include "concurrency/OSThread.h"

/**
 * Custom feature: read a mode switch ONCE at boot.
 *  - switch LOW (to GND) -> event mode: broadcast a fixed message on a named channel once,
 *    drive an output pin HIGH, then suppress the node's own outbound traffic ("go quiet").
 *  - switch HIGH (pull-up) -> standard mode: this thread disables itself, zero impact.
 * Mode is latched at startup; changing it requires flipping the switch and rebooting.
 */
class EventSwitchModule : public concurrency::OSThread
{
  public:
    EventSwitchModule();

  protected:
    int32_t runOnce() override;

  private:
    bool eventActive = false;
    bool done = false;
};

#endif // EVENT_SWITCH_PIN
