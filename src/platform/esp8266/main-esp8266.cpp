#include "configuration.h"

#ifdef ARCH_ESP8266

#include <Arduino.h>
#include <umm_malloc/umm_heap_select.h>

// ESP8266 toolchain (xtensa) has no libatomic.  NotifiedWorkerThread uses
// std::atomic<uint32_t>::exchange(), which the compiler lowers to
// __atomic_exchange_4.  On this single-core chip a critical section is
// sufficient: disable interrupts for the read-modify and re-enable.
extern "C" uint32_t __atomic_exchange_4(volatile void *ptr, uint32_t val, int)
{
    uint32_t old;
    noInterrupts();
    old = *reinterpret_cast<volatile uint32_t *>(ptr);
    *reinterpret_cast<volatile uint32_t *>(ptr) = val;
    interrupts();
    return old;
}

// libstdc++ new_opa.o (C++17 aligned operator new) references memalign.
// umm_malloc guarantees 8-byte alignment which covers all our types; ignore
// the requested alignment for simplicity.
extern "C" void *memalign(size_t /*alignment*/, size_t size)
{
    return malloc(size);
}

// --wrap=malloc redirects all malloc() calls here (including the one inside
// operator new from the Arduino abi.cpp).  When the DRAM heap is exhausted,
// retry from the IRAM secondary heap (enabled via MMU_IRAM_HEAP).
extern "C" void *__real_malloc(size_t);
extern "C" void *__real_calloc(size_t, size_t);
extern "C" void *__real_realloc(void *, size_t);

extern "C" void *__wrap_malloc(size_t size)
{
    void *p = __real_malloc(size);
    if (!p) {
        HeapSelectIram _iram;
        p = __real_malloc(size);
    }
    return p;
}

extern "C" void *__wrap_calloc(size_t n, size_t size)
{
    void *p = __real_calloc(n, size);
    if (!p) {
        HeapSelectIram _iram;
        p = __real_calloc(n, size);
    }
    return p;
}

extern "C" void *__wrap_realloc(void *ptr, size_t size)
{
    void *p = __real_realloc(ptr, size);
    if (!p && size > 0) {
        // umm_malloc cannot realloc a block across heap boundaries (it follows
        // the block's own context, ignoring HeapSelectIram).  Do the cross-
        // heap move manually: allocate from IRAM, 32-bit-word-copy the old
        // content, then free the old DRAM block.
        //
        // We copy `size` bytes (the new requested size) rather than the unknown
        // old size.  realloc is always called to grow an array, so old_size <
        // size.  The extra bytes read are alignment padding from the old block
        // (umm_malloc rounds up); the caller will immediately overwrite them
        // with the new element — so the garbage copy is harmless.
        //
        // Word-only copies keep us safe from StoreProhibited on IRAM writes.
        // Any caller storing individual bytes to the returned IRAM pointer still
        // needs NON32XFER_HANDLER; here all callers (Fsm transitions) use
        // 4-byte-aligned struct assignment which the compiler emits as word stores.
        HeapSelectIram _iram;
        p = __real_malloc(size);
        if (p && ptr) {
            // 32-bit word copy (IRAM requires word-level writes)
            uint32_t *dst = reinterpret_cast<uint32_t *>(p);
            const uint32_t *src = reinterpret_cast<const uint32_t *>(ptr);
            for (size_t i = 0; i < size / 4; i++)
                dst[i] = src[i];
            free(ptr);
        }
    }
    return p;
}

// No Bluetooth on ESP8266
void setBluetoothEnable(bool enable) {}

// Build a locally-administered 6-byte address from the 32-bit chip ID.
// Avoids pulling in the full ESP8266WiFi library.
void getMacAddr(uint8_t *dmac)
{
    uint32_t id = ESP.getChipId(); // 24-bit chip ID; upper byte is always 0
    dmac[0] = 0x02; // locally administered, unicast
    dmac[1] = 0xAA;
    // Use a static prefix so the nodenum (dmac[2]<<24 | dmac[3..5]) is always
    // 8 hex digits instead of 6 (ESP8266 chip ID is only 24 bits).
    dmac[2] = 0xAA;
    dmac[3] = (id >> 16) & 0xFF;
    dmac[4] = (id >> 8) & 0xFF;
    dmac[5] = id & 0xFF;
}

void cpuDeepSleep(uint32_t msecToWake)
{
    // ESP8266 deep sleep wakes via RST pin connected to GPIO16 (D0)
    ESP.deepSleep((uint64_t)msecToWake * 1000ULL);
}

void updateBatteryLevel(uint8_t level) {}

// Defined in Screen.cpp normally; stub here since there is no display.
bool shouldWakeOnReceivedMessage()
{
    return false;
}

#endif // ARCH_ESP8266
