# Fitting Meshtastic Firmware into a Small-RAM Target

This document captures the techniques used when porting Meshtastic to the ESP8266
(81,920 bytes of static BSS/data RAM). Most of these apply to any constrained
microcontroller; specific numbers are for the ESP8266 + SX1262 minimal variant.

---

## 1. Know your constraints up front

| Resource | ESP8266 limit | Source |
|---|---|---|
| Static RAM (BSS + data) | 81,920 bytes (`dram0_0_seg`, 0x3FFE8000–0x3FFFC000) | linker script |
| Heap (runtime) | remaining after BSS | runtime |
| Flash | 4 MB (D1 Mini) | board |

PlatformIO reports static usage after each build. Watch for:

```
RAM:   [========= ]  88.5% (used 72512 bytes from 81920 bytes)
Flash: [======    ]  61.3% (used 641984 bytes from 1048576 bytes)
```

A BSS overflow produces a linker error like:

```
region `dram0_0_seg' overflowed by 41984 bytes
```

Start measuring early and check after every significant change.

---

## 2. Strip unused source files with `build_src_filter`

PlatformIO compiles everything under `src/` by default. Use `build_src_filter` to
remove whole directories or individual files that aren't needed.

```ini
build_src_filter =
  ${arduino_base.build_src_filter}
  -<platform/esp32/>
  -<platform/nrf52/>
  -<platform/stm32wl/>
  -<platform/rp2xx0/>
  -<platform/portduino/>
  -<platform/extra_variants/>
  -<mesh/http/>
  -<mesh/eth/>
  -<mesh/raspihttp/>
  -<mesh/api/WiFiServerAPI.cpp>
  -<mesh/api/ethServerAPI.cpp>
  -<mesh/wifi/>
  -<modules/esp32/>
  -<modules/audio/>
  -<modules/Telemetry/>
  -<modules/ExternalNotificationModule.cpp>
  -<modules/RangeTestModule.cpp>
  -<modules/CannedMessageModule.cpp>
  -<modules/WaypointModule.cpp>
  -<modules/StoreForwardModule.cpp>
  -<modules/NeighborInfoModule.cpp>
  -<modules/KeyVerificationModule.cpp>
  -<modules/TrafficManagementModule.cpp>
  -<modules/GenericThreadModule.cpp>
  -<modules/OnScreenKeyboardModule.cpp>
  -<detect/ScanI2CTwoWire.cpp>
  -<detect/reClockI2C.cpp>
  -<graphics/>
  -<gps/GPS.cpp>
  -<gps/GPSUpdateScheduling.cpp>
  -<gps/NMEAWPL.cpp>
  -<motion/>
  -<mqtt/>
  -<nimble/>
  -<input/>
  +<platform/esp8266/>
```

**Important caveats:**

- Do **not** exclude `gps/RTC.cpp` or `gps/GeoCoord.cpp` — they provide symbols
  (`readFromRTC`, `GeoCoord`) referenced by non-GPS code paths.
- Do **not** exclude `mesh/api/PhoneAPI.cpp` or `mesh/XModem.cpp` — `PhoneAPI`
  directly references `XModem` and the linker will fail.
- Only exclude the mqtt *directory* when combined with `MESHTASTIC_EXCLUDE_MQTT`;
  the directory contains symbols used elsewhere if MQTT is active.

---

## 3. Use `MESHTASTIC_EXCLUDE_*` build flags

These flags prevent module instances from being created in `Modules.cpp` and guard
feature code throughout the codebase. Always pair a `build_src_filter` exclusion
with the matching flag:

```ini
build_flags =
  -DMESHTASTIC_EXCLUDE_BLUETOOTH=1
  -DMESHTASTIC_EXCLUDE_WIFI=1
  -DMESHTASTIC_EXCLUDE_WEBSERVER=1
  -DMESHTASTIC_EXCLUDE_SCREEN=1
  -DMESHTASTIC_EXCLUDE_GPS=1
  -DMESHTASTIC_EXCLUDE_AUDIO=1
  -DMESHTASTIC_EXCLUDE_ACCELEROMETER=1
  -DMESHTASTIC_EXCLUDE_PAXCOUNTER=1
  -DMESHTASTIC_EXCLUDE_RANGETEST=1
  -DMESHTASTIC_EXCLUDE_ATAK=1
  -DMESHTASTIC_EXCLUDE_ENVIRONMENTAL_SENSOR=1
  -DMESHTASTIC_EXCLUDE_POWER_TELEMETRY=1
  -DMESHTASTIC_EXCLUDE_TRACEROUTE=1
  -DMESHTASTIC_EXCLUDE_I2C=1
  -DMESHTASTIC_EXCLUDE_INPUTBROKER=1
  -DMESHTASTIC_EXCLUDE_EXTERNALNOTIFICATION=1
  -DMESHTASTIC_EXCLUDE_MQTT=1
  -DMESHTASTIC_EXCLUDE_WAYPOINT=1
  -DMESHTASTIC_EXCLUDE_NEIGHBORINFO=1
  -DMESHTASTIC_EXCLUDE_PKI=1
  -DMESHTASTIC_EXCLUDE_CANNEDMESSAGES=1
  -DMESHTASTIC_EXCLUDE_STOREFORWARD=1
  -DMESHTASTIC_EXCLUDE_DETECTIONSENSOR=1
```

If you add a flag but the build still pulls in the module's `.cpp` via `build_src_filter`,
the static objects still exist. Both sides must be consistent.

---

## 4. Shrink packet pool sizes

Each `meshtastic_MeshPacket` is ~358 bytes. The static packet pool size is:

```
MAX_PACKETS_STATIC = MAX_RX_TOPHONE + MAX_RX_FROMRADIO + 2 * MAX_TX_QUEUE + 2
```

Default values produce ~(4 + 4 + 32 + 2) × 358 ≈ 15 KB of static BSS.

The definitions in headers use plain `#define` without `#ifndef` guards.
**Before overriding via build flags you must add `#ifndef` guards** in the source:

| File | Symbol |
|---|---|
| `src/mesh/RadioInterface.h` | `MAX_TX_QUEUE` |
| `src/mesh/Router.cpp` | `MAX_RX_FROMRADIO` |
| `src/mesh/MeshService.cpp` | `MAX_MQTT_PROXY_MESSAGES`, `MAX_CLIENT_NOTIFICATIONS` |

After adding the guards, tune in `platformio.ini`:

```ini
build_flags =
  -DMAX_RX_TOPHONE=4
  -DMAX_TX_QUEUE=2
  -DMAX_RX_FROMRADIO=4
  -DMAX_RX_MQTTPROXY_TOPHONE=1
  -DMAX_MQTT_PROXY_MESSAGES=1
  -DMAX_CLIENT_NOTIFICATIONS=1
```

These values give ~(4 + 4 + 4 + 2) × 358 ≈ 5 KB, saving roughly 10 KB vs defaults.

---

## 5. Limit the NodeDB node count

`MAX_NUM_NODES` controls the `std::vector` pre-allocation in NodeDB. The vector is
heap-allocated, not BSS, but reducing it frees heap for runtime use.

Default is 100 nodes × ~220 bytes ≈ 22 KB of heap. Setting it to 15 nodes costs
~3.3 KB — a saving of ~19 KB of heap headroom:

```ini
build_flags =
  -DMAX_NUM_NODES=15
```

---

## 6. Ignore unused libraries

Libraries in `lib_deps` that don't have source exclusions still get linked.
Use `lib_ignore` to drop them entirely:

```ini
lib_ignore =
  segger_rtt
  NimBLE-Arduino
  ESP32 BLE Arduino
  XPowersLib
  libpax
  Wire
  Ethernet
```

`Wire` is important on ESP8266: if `Wire.h` is reachable (e.g. from
`detect/ScanI2CTwoWire.cpp`) it silently pulls in ~2 KB of BSS for TWI buffers.
Excluding the file via `build_src_filter` **and** ignoring the library is the
safest approach.

---

## 7. Enable size optimisation

Use `-Os` (optimise for size) rather than the default `-O2`:

```ini
build_flags =
  -Os
```

On ESP8266 this typically recovers 5–15% of flash. Its effect on BSS is smaller
but still measurable because some compiler-inserted tables shrink.

---

## 8. Avoid pulling in heavyweight libraries through indirect includes

Two common traps on ESP8266:

**WiFi library via `WiFi.macAddress()`**

`WiFi.macAddress()` links the entire ESP8266WiFi library (~7 KB BSS for internal
socket/lwIP state). If you only need a unique device ID, use:

```cpp
uint32_t id = ESP.getChipId();
```

**`Tone.h` not available on ESP8266**

The Arduino `Tone` API is implemented differently per platform. Guard platform-
specific includes:

```cpp
#if !defined(ARCH_ESP8266)
#include <Tone.h>
#endif
```

---

## 9. Leave `HAS_CUSTOM_CRYPTO_ENGINE` undefined, not defined to 0

`CryptoEngine.cpp` uses:

```cpp
#ifndef HAS_CUSTOM_CRYPTO_ENGINE
// software fallback
#endif
```

If your `architecture.h` writes `#define HAS_CUSTOM_CRYPTO_ENGINE 0` the
preprocessor evaluates the macro as *defined* and skips the software fallback,
causing an undefined reference to `crypto` at link time.

**Correct:** simply omit the define. The `#ifndef` then fires and the software
engine is compiled in.

---

## 10. Provide stubs for symbols from excluded subsystems

When you remove a whole subsystem via `build_src_filter`, functions that other
translation units reference become undefined. Common stubs needed for a headless
(no screen, no BLE, no GPS) build:

```cpp
// in your platform's main-*.cpp
void setBluetoothEnable(bool) {}
void updateBatteryLevel(uint8_t) {}
bool shouldWakeOnReceivedMessage() { return false; }  // normally in Screen.cpp
```

Guard any module-level calls in shared code that aren't already behind
`MESHTASTIC_EXCLUDE_*` flags. Example — `NodeDB.cpp` calls
`neighborInfoModule->resetNeighbors()` unconditionally; it needs a guard:

```cpp
#if !MESHTASTIC_EXCLUDE_NEIGHBORINFO
    neighborInfoModule->resetNeighbors();
#endif
```

---

## 11. Persistent storage: LittleFS

On ESP8266, Meshtastic's NodeDB save/load works through `FSCom` (defined in
`FSCommon.h`). Add an `ARCH_ESP8266` block mapping `FSCom` to `LittleFS`:

```cpp
#if defined(ARCH_ESP8266)
#include <LittleFS.h>
#define FSCom LittleFS
#define FSBegin() FSCom.begin()
#define FILE_O_WRITE "w"
#define FILE_O_READ "r"
#endif
```

Then tell PlatformIO to partition the flash for LittleFS (not the older SPIFFS
default):

```ini
board_build.filesystem = littlefs
```

The D1 Mini linker script `eagle.flash.4m1m.ld` allocates 1 MB for the
filesystem partition — plenty for the protobuf config files that NodeDB writes.

---

## 12. ESP8266 secondary IRAM heap

The ESP8266 has 64 KB of IRAM. By default, 32 KB is used as iCACHE (flash cache)
and 32 KB holds executable code. Switching to the 16 KB / 48 KB cache/IRAM split
frees 16 KB of IRAM as a secondary heap that can absorb runtime allocations when
DRAM runs out.

Enable via the PlatformIO framework flag (not a raw `-D` define):

```ini
build_flags =
  -D PIO_FRAMEWORK_ARDUINO_MMU_CACHE16_IRAM48_SECHEAP_SHARED
```

This tells the espressif8266 platform builder to set
`MMU_IRAM_SIZE=0xC000`, `MMU_ICACHE_SIZE=0x4000`, and `MMU_IRAM_HEAP`, and to
regenerate the linker script with a 48 KB IRAM region. Raw `-DMMU_IRAM_HEAP`
alone does **not** work — `mmu_sec_heap()` is only declared when
`MMU_IRAM_SIZE > 32768`.

### Directing LittleFS caches to IRAM

Wrap `FSBegin()` with the `HeapSelectIram` RAII guard so LittleFS allocates its
internal sector/read caches in IRAM:

```cpp
// FSCommon.h — ARCH_ESP8266 block
#include <umm_malloc/umm_heap_select.h>
inline bool FSBegin() { HeapSelectIram _iram; return FSCom.begin(); }
```

### Directing all malloc/new failures to IRAM

The Arduino `abi.cpp` defines `operator new` as a thin wrapper over `malloc`.
Override at the linker level with `--wrap` so both `malloc` and `operator new`
fall back to IRAM on DRAM exhaustion:

```cpp
// platform/esp8266/main-esp8266.cpp
extern "C" void *__real_malloc(size_t);
extern "C" void *__wrap_malloc(size_t size)
{
    void *p = __real_malloc(size);
    if (!p) { HeapSelectIram _iram; p = __real_malloc(size); }
    return p;
}
// repeat for __wrap_calloc and __wrap_realloc
```

```ini
build_flags =
  -Wl,-wrap=malloc
  -Wl,-wrap=calloc
  -Wl,-wrap=realloc
```

**Trade-off:** 16 KB iCACHE instead of 32 KB means more flash cache misses and
slightly slower code execution (~10–30% slower for flash-resident code). For a
LoRa mesh node that spends most time waiting for packets, this is acceptable.

The secondary heap size is `0x4010C000 - _text_end` (i.e. 48 KB IRAM minus code
size). With `-Os` and a minimal build this is typically 10–20 KB.

---

## Summary: RAM budget after applying all techniques

| Technique | Approximate saving |
|---|---|
| Excluding unused platform dirs | ~8 KB BSS |
| `MESHTASTIC_EXCLUDE_*` flags | ~5 KB BSS |
| Packet pool downsizing (to 2/1/2) | ~11 KB BSS |
| `lib_ignore` (Wire, BLE, WiFi) | ~4 KB BSS |
| `-Os` optimisation | ~3 KB flash / marginal BSS |
| `MAX_NUM_NODES=6` | ~15 KB heap (not BSS) |
| `PACKETHISTORY_MAX=20` | ~1.6 KB heap |
| IRAM secondary heap (16KB iCACHE split) | +10–20 KB heap overflow capacity |

Starting from ~97% BSS utilisation (overflowed), the combination above brings the
ESP8266 to ~86% BSS (70,348 / 81,920 bytes), leaving ~11 KB of static headroom.
Runtime heap is extended by 10–20 KB via the IRAM secondary heap.
