# Wiring Guide - ESP32 MP3 Player

## Overview

This guide shows how to connect all components for the ESP32 MP3 Player.

## Pinout Diagram

```
                    ESP32-WROOM-32 Dev Board
                    ╔════════════════════════╗
                    ║  ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄  ║
                    ║  █  ESP32-WROOM  █  ║
                    ║  ▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀▀  ║
                    ║                      ║
        3.3V  ──────║─● VCC                ║
        GND  ───────║─● GND                ║
                    ║                      ║
        GPIO21  ────║─● SDA (OLED)         ║
        GPIO22  ────║─● SCL (OLED)         ║
                    ║                      ║
        GPIO5   ────║─● CS   (SD Card)     ║
        GPIO18  ────║─● SCK  (SD Card)     ║
        GPIO19  ────║─● MISO (SD Card)     ║
        GPIO23  ────║─● MOSI (SD Card)     ║
                    ║                      ║
        GPIO34  ────║─● POT  (Volume)      ║  (ADC1_CH6, input only)
        GPIO35  ────║─● BAT  (Battery)     ║  (ADC1_CH7, input only)
                    ║                      ║
        GPIO25  ────║─● BTN  LEFT          ║
        GPIO26  ────║─● BTN  RIGHT         ║
        GPIO32  ────║─● BTN  UP            ║
        GPIO33  ────║─● BTN  DOWN          ║
                    ║                      ║
        GPIO2   ────║─● LED  (Status)      ║
                    ║                      ║
                    ╚════════════════════════╝
```

## Component Connections

### 1. OLED Display (SSD1306 I2C)

```
OLED Module          ESP32
─────────          ─────
VCC       ────────── 3.3V
GND       ────────── GND
SDA       ────────── GPIO21
SCL       ────────── GPIO22
```

**Notes:**
- Use 4.7kΩ pull-up resistors on SDA/SCL if not on module
- Address: 0x3C (some modules use 0x3D)
- No reset pin needed for I2C

### 2. MicroSD Card Module (SPI)

```
SD Module           ESP32
─────────          ─────
VCC       ────────── 3.3V (or 5V if module has regulator)
GND       ────────── GND
CS        ────────── GPIO5
SCK       ────────── GPIO18
MISO      ────────── GPIO19
MOSI      ────────── GPIO23
```

**Notes:**
- Must use 3.3V logic level (ESP32 is not 5V tolerant)
- If module has 5V->3.3V regulator, connect VCC to 5V (USB)
- Short traces for reliable SPI communication

### 3. Potentiometer (Volume Control)

```
Potentiometer (10kΩ)    ESP32
─────────────────      ─────
Left pin (CCW)    ────── 3.3V
Center pin (Wiper) ────── GPIO34 (ADC1_CH6)
Right pin (CW)    ────── GND
```

**Notes:**
- GPIO34 is INPUT ONLY (no internal pull-up/down)
- Use linear taper potentiometer (B10K)
- Add 0.1µF capacitor between wiper and GND for noise reduction

### 4. Buttons (4x Tactile)

All buttons use internal pull-up resistors (INPUT_PULLUP).

```
Button LEFT:       GPIO25 ────── Button ────── GND
Button RIGHT:      GPIO26 ────── Button ────── GND
Button UP:         GPIO32 ────── Button ────── GND
Button DOWN:       GPIO33 ────── Button ────── GND
```

**Notes:**
- Buttons connect pin to GND when pressed
- Internal pull-ups enabled in software
- Debounce handled in software (50ms)

### 5. Battery & Charging

```
                    TP4056 Module
                    ╔═════════════╗
    LiPo Battery ───║ B+    B-    ║─── LiPo Battery
    (JST-PH)        ║             ║
                    ║ OUT+  OUT-  ║───┬──────► 3.3V LDO (AMS1117) ───► ESP32 3.3V
                    ║             ║   │
                    ║ MicroUSB    ║   └──────► Battery Monitor (via divider)
                    ╚═════════════╝         │
                                           │
                              Voltage Divider (2x 100kΩ)
                              ┌─────────────┴─────────────┐
                              ▼                           ▼
                           GPIO35 (ADC)                GND
                              │
                           100nF to GND (noise filter)
```

**Battery Voltage Divider:**
```
LiPo+ ───[100kΩ]───┬───[100kΩ]─── GND
                   │
                   ├── 100nF ── GND
                   │
                   └──── GPIO35 (ADC1_CH7)
```

**Notes:**
- TP4056 provides overcharge/overdischarge protection
- Voltage divider scales 4.2V max to 2.1V (within ADC range)
- 100nF capacitor filters noise
- AMS1117-3.3 provides stable 3.3V to ESP32

### 6. Status LED

```
GPIO2 ──────[220Ω]────── LED Anode
LED Cathode ──────────── GND
```

## Assembly Instructions

### Step 1: Prepare Perfboard
- Cut perfboard to ~70x50mm
- Plan layout before soldering

### Step 2: Solder Power Circuit
1. Solder TP4056 module
2. Solder AMS1117-3.3 with input/output capacitors (10µF + 0.1µF)
3. Add JST connectors for battery
4. Test: USB in → 3.3V out, Battery → 3.3V out

### Step 3: Solder ESP32
- Use female headers for ESP32 (removable)
- Solder headers to perfboard
- Verify all pins accessible

### Step 4: Solder Peripherals
1. OLED (I2C) - 4 wires
2. SD Card (SPI) - 6 wires
3. Potentiometer - 3 wires
4. Buttons - 8 wires (4 signal + 4 ground)
5. LED - 2 wires

### Step 5: Battery Monitor
- Solder 2x 100kΩ resistors as voltage divider
- Add 100nF capacitor
- Connect to GPIO35

### Step 6: Verify Connections
```
□ Continuity check all connections
□ No shorts between adjacent pins
□ 3.3V rail measures 3.3V
□ GND common throughout
□ Battery divider: 4.2V in → 2.1V at GPIO35
```

### Step 7: Flash Firmware
1. Connect ESP32 via USB
2. `pio run -t upload`
3. Open serial monitor: `pio device monitor`

### Step 8: Test Hardware
1. OLED shows boot screen
2. SD card detected (insert card)
3. Buttons respond (check serial)
4. Potentiometer changes volume
5. Bluetooth pairs with phone/earbuds

## Troubleshooting

| Symptom | Check |
|---------|-------|
| OLED blank | I2C wiring, 0x3C vs 0x3D address, pull-ups |
| SD init fails | SPI wiring, CS pin, 3.3V power, card format |
| Buttons don't work | Pin numbers, pull-up enabled, GND connection |
| Pot not smooth | Capacitor on wiper, correct pin (GPIO34), ADC config |
| No Bluetooth | Device name, antenna not blocked, earbuds in pairing mode |
| Battery reads wrong | Divider resistors, ADC attenuation (11dB), capacitor |

## Enclosure Design

Recommended internal dimensions: 100 x 60 x 25mm

Cutouts needed:
- OLED: 28 x 28mm (for 0.96")
- SD Card slot: Side access
- Potentiometer: 7mm hole + knob
- Buttons: 4x 6mm holes (D-pad layout)
- USB: MicroUSB cutout for charging
- LED: 3mm hole

## Schematic

See `schematics/` folder for KiCad files (to be created).

## Bill of Materials Cost

| Category | Cost |
|----------|------|
| Electronics | ~$35 |
| Mechanical | ~$15 |
| **Total** | **~$50** |

*Prices from Adafruit/Amazon (2024), may vary*