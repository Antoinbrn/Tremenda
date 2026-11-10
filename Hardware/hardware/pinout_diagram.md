# ESP32 MP3 Player - Pinout Diagram

## Quick Reference

```
┌─────────────────────────────────────────────────────────────────┐
│                      ESP32-WROOM-32                             │
│                         Pinout                                  │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│    ADC1_CH0 (GPIO36)  ●●  GPIO39 (ADC1_CH3)  SENSOR_VP         │
│    ADC1_CH1 (GPIO37)  ●●  GPIO38 (ADC1_CH2)  SENSOR_VN         │
│    ADC1_CH4 (GPIO32)  ●●  GPIO33 (ADC1_CH5)  BTN_UP            │
│    ADC1_CH6 (GPIO34)  ●●  GPIO35 (ADC1_CH7)  BAT_MON           │
│    ADC2_CH0 (GPIO4)   ●●  GPIO0 (BOOT)                         │
│    ADC2_CH1 (GPIO5)   ●●  GPIO2 (LED)          SD_CS           │
│    ADC2_CH2 (GPIO18)  ●●  GPIO19 (MISO)       SD_SCK           │
│    ADC2_CH3 (GPIO23)  ●●  GPIO22 (SCL)        SD_MOSI          │
│    ADC2_CH4 (GPIO21)  ●●  GPIO1 (TX0)         OLED_SDA         │
│    ADC2_CH5 (GPIO17)  ●●  GPIO3 (RX0)         OLED_SCL         │
│    ADC2_CH6 (GPIO16)  ●●  GPIO15 (HSPI_CS)                    │
│    ADC2_CH7 (GPIO14)  ●●  GPIO12 (HSPI_MISO)                  │
│    ADC2_CH8 (GPIO13)  ●●  GPIO27 (PWM)                       │
│    ADC2_CH9 (GPIO25)  ●●  GPIO26 (PWM)        BTN_LEFT         │
│    GND                ●●  GPIO34 (ADC)         BTN_RIGHT       │
│    3.3V               ●●  EN (RESET)                         │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

## Our Connections (Highlighted)

```
┌─────────────────────────────────────────────────────────────────┐
│                     ESP32 MP3 PLAYER PINS                       │
├──────────────┬──────────┬──────────────────────────────────────┤
│ Function     │ GPIO     │ Notes                                │
├──────────────┼──────────┼──────────────────────────────────────┤
│ OLED SDA     │ GPIO21   │ I2C Data (4.7k pull-up)             │
│ OLED SCL     │ GPIO22   │ I2C Clock (4.7k pull-up)            │
│ SD CS        │ GPIO5    │ SPI Chip Select                      │
│ SD SCK       │ GPIO18   │ SPI Clock (VSPI)                     │
│ SD MISO      │ GPIO19   │ SPI MISO (VSPI)                      │
│ SD MOSI      │ GPIO23   │ SPI MOSI (VSPI)                      │
│ Volume Pot   │ GPIO34   │ ADC1_CH6 (INPUT ONLY!)              │
│ Battery Mon  │ GPIO35   │ ADC1_CH7 (INPUT ONLY!)              │
│ Button LEFT  │ GPIO25   │ INPUT_PULLUP                         │
│ Button RIGHT │ GPIO26   │ INPUT_PULLUP                         │
│ Button UP    │ GPIO32   │ INPUT_PULLUP                         │
│ Button DOWN  │ GPIO33   │ INPUT_PULLUP                         │
│ Status LED   │ GPIO2    │ Active HIGH                          │
├──────────────┼──────────┼──────────────────────────────────────┤
│ USB D+       │ GPIO20   │ Not used (internal USB PHY)         │
│ USB D-       │ GPIO19   │ Not used (internal USB PHY)         │
├──────────────┼──────────┼──────────────────────────────────────┤
│ STRAP PINS   │          │                                      │
│ GPIO0        │ BOOT     │ LOW = Download mode                  │
│ GPIO2        │ LED      │ Must be LOW/HIGH at boot             │
│ GPIO12       │ MTDI     │ Must be LOW at boot                  │
│ GPIO15       │ MTDO     │ Must be HIGH at boot                 │
└──────────────┴──────────┴──────────────────────────────────────┘
```

## Visual Layout (Top View)

```
        USB
         │
    ┌────┴────┐
    │  ESP32  │
    │  DEV    │
    │  BOARD  │
    └────┬────┘
         │
    ┌────┼────┐
    ▼    ▼    ▼
 ┌───┐ ┌───┐ ┌───┐
 │BTN│ │BTN│ │BTN│  UP(32)  DOWN(33)  LEFT(25)
 │UP │ │DN │ │LT │
 └───┘ └───┘ └───┘
    ▲         ▲
    │         │
 ┌──┴──┐  ┌───┴───┐
 │POT  │  │ OLED  │
 │GPIO34│  │SDA/SCL│
 └─────┘  └───────┘
    ▲         ▲
    │         │
 ┌──┴──┐  ┌───┴───┐
 │ SD  │  │ BAT   │
 │ SPI │  │ ADC35 │
 └─────┘  └───────┘
    ▲
    │
 ┌──┴──┐
 │ LED │
 │GPIO2│
 └─────┘
```

## SPI Bus (VSPI) - Shared

| Signal | GPIO | SD Card | Notes |
|--------|------|---------|-------|
| SCK    | 18   | CLK     | VSPI_CLK |
| MISO   | 19   | DO      | VSPI_Q |
| MOSI   | 23   | DI      | VSPI_D |
| CS     | 5    | CS      | VSPI_CS0 |

## I2C Bus - Shared

| Signal | GPIO | OLED | Notes |
|--------|------|------|-------|
| SDA    | 21   | SDA  | 4.7kΩ pull-up to 3.3V |
| SCL    | 22   | SCL  | 4.7kΩ pull-up to 3.3V |

## ADC Channels Used

| Channel | GPIO | Function | Range |
|---------|------|----------|-------|
| ADC1_CH6 | 34 | Volume Potentiometer | 0-3.3V |
| ADC1_CH7 | 35 | Battery Monitor (÷2) | 0-2.1V (0-4.2V bat) |

**Important:** GPIO34, 35, 36, 39 are INPUT ONLY - no output, no pull-up/down

## Buttons - All INPUT_PULLUP

| Button | GPIO | Press = | Release = |
|--------|------|---------|-----------|
| UP     | 32   | LOW     | HIGH      |
| DOWN   | 33   | LOW     | HIGH      |
| LEFT   | 25   | LOW     | HIGH      |
| RIGHT  | 26   | LOW     | HIGH      |

## Power Pins

| Pin | Function | Notes |
|-----|----------|-------|
| 3.3V | Main power | From AMS1117-3.3 |
| GND  | Ground | Common ground |
| VIN  | 5V from USB | To TP4056/AMS1117 input |
| EN   | Reset | Active LOW |

## Unused/Reserved Pins

```
GPIO0   - BOOT button (strap)
GPIO1   - TX0 (Serial debug)
GPIO3   - RX0 (Serial debug)
GPIO4   - Free
GPIO12  - MTDI (strap, must be LOW at boot)
GPIO13  - Free
GPIO14  - Free
GPIO15  - MTDO (strap, must be HIGH at boot)
GPIO16  - Free
GPIO17  - Free
GPIO27  - Free
```

## Pin Conflicts to Avoid

| Pin | Conflict | Resolution |
|-----|----------|------------|
| GPIO12 | MTDI strap | Keep LOW at boot (our buttons use 25,26,32,33 - OK) |
| GPIO15 | MTDO strap | Keep HIGH at boot (our SD uses 5,18,19,23 - OK) |
| GPIO2  | LED + Strap | LED is OK, just don't pull LOW at boot |
| GPIO0  | BOOT | Don't connect button to GPIO0 |
| GPIO34/35/36/39 | Input only | Only use for ADC (we do - OK) |

## Alternative Pin Assignments

If needed, these can be changed in `config.h`:

```cpp
// Alternative I2C pins
#define OLED_SDA    18  // If 21/22 conflict
#define OLED_SCL    19

// Alternative SPI pins (HSPI)
#define SD_CS       15
#define SD_MOSI     13
#define SD_MISO     12
#define SD_SCK      14

// Alternative button pins (avoid straps)
#define BTN_UP      13
#define BTN_DOWN    14
#define BTN_LEFT    27
#define BTN_RIGHT   4
```

## Testing Pin Connections

```cpp
// Run this sketch to verify all pins
void setup() {
    Serial.begin(115200);

    // Test buttons
    pinMode(25, INPUT_PULLUP);
    pinMode(26, INPUT_PULLUP);
    pinMode(32, INPUT_PULLUP);
    pinMode(33, INPUT_PULLUP);

    // Test ADC
    analogReadResolution(12);
    analogSetAttenuation(ADC_11db);

    // Test I2C
    Wire.begin(21, 22);

    // Test SPI
    SPI.begin(18, 19, 23, 5);
}

void loop() {
    Serial.printf("BTN: U=%d D=%d L=%d R=%d  ",
                  digitalRead(32), digitalRead(33),
                  digitalRead(25), digitalRead(26));
    Serial.printf("POT=%d  BAT=%.2fV\n",
                  analogRead(34),
                  analogRead(35) * 3.3 / 4095.0 * 2.0);
    delay(100);
}
```