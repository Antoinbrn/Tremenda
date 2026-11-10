# ESP32 MP3 Player

A Spotify-like MP3 player built on ESP32-WROOM-32 with Bluetooth A2DP audio output, OLED display, and SD card storage.

## Features

- **Bluetooth A2DP Sink** - Connect to wireless earbuds/headphones
- **SD Card Support** - MP3, WAV, FLAC, M4A, AAC, OGG playback
- **Spotify-like UI** - Now Playing, Library, Search, Settings on 128x64 OLED
- **4-Button Navigation** - Up, Down, Left, Right with long-press actions
- **Potentiometer Volume** - Smooth analog volume control
- **Folder-based Playlists** - Each folder = one playlist
- **Shuffle Mode** - Fisher-Yates shuffle
- **Real-time Search** - Filter by filename
- **Battery Monitoring** - Voltage display with low-battery warning
- **Deep Sleep** - 10 min pause timeout for 2+ year battery life

## Hardware Requirements

| Component | Specification | Pins |
|-----------|--------------|------|
| ESP32-WROOM-32 | Dev board with USB | All |
| 0.96" OLED | SSD1306 128x64 I2C | GPIO21 (SDA), GPIO22 (SCL) |
| MicroSD Module | SPI interface | GPIO5 (CS), GPIO18 (SCK), GPIO19 (MISO), GPIO23 (MOSI) |
| 10kΩ Potentiometer | Volume control | GPIO34 (ADC1_CH6) |
| 4x Tactile Buttons | 6x6mm 4-pin | GPIO25, 26, 32, 33 |
| LiPo Battery | 3.7V 2000mAh+ | - |
| TP4056 Module | LiPo charger + protection | - |
| 3.3V LDO | AMS1117-3.3 | - |

See [hardware/parts_list.csv](hardware/parts_list.csv) for complete parts list with links.

## Wiring

See [hardware/wiring_guide.md](hardware/wiring_guide.md) for detailed wiring diagram and instructions.

### Pinout Summary

```
ESP32 Pin -> Function
---------   --------
GPIO21      OLED SDA
GPIO22      OLED SCL
GPIO5       SD Card CS
GPIO18      SD Card SCK
GPIO19      SD Card MISO
GPIO23      SD Card MOSI
GPIO34      Potentiometer (Volume)
GPIO35      Battery Monitor (via divider)
GPIO25      Button LEFT
GPIO26      Button RIGHT
GPIO32      Button UP
GPIO33      Button DOWN
GPIO2       Status LED
```

## Software Setup

### Prerequisites

- PlatformIO Core or PlatformIO IDE extension
- ESP32 board support (`pio platform install espressif32`)

### Build and Flash

```bash
cd esp32_mp3_player
pio run -t upload
```

### Monitor Serial Output

```bash
pio device monitor -b 115200
```

### Run Tests

```bash
pio test -e esp32dev_test
```

## SD Card Preparation

1. Format SD card as FAT32
2. Create folder structure:
   ```
   /Music/
     /Rock/
       song1.mp3
       song2.mp3
     /Pop/
       song3.mp3
     /Jazz/
       song4.flac
   /Podcasts/
     episode1.mp3
   ```
3. Insert into SD card module

## First Boot

1. Power on the device
2. Put Bluetooth earbuds in pairing mode
3. Device will auto-connect to "ESP32_MP3_Player"
4. Use buttons to navigate

## Controls

| Button | Now Playing | Library/Playlist | Search | Settings |
|--------|-------------|------------------|--------|----------|
| **Right** | Play/Pause | Enter/Select | Select key | Toggle |
| **Left** | Back to Library | Back | Backspace/Back | Back |
| **Up** | Previous Track | Navigate Up | Keyboard Up | Navigate Up |
| **Down** | Next Track | Navigate Down | Keyboard Down/Results | Navigate Down |
| **Long Left** | Toggle Shuffle | - | - | - |
| **Potentiometer** | Volume | Volume | Volume | Volume |

## Screens

1. **Now Playing** - Album art placeholder, track info, progress bar, volume
2. **Library** - "All Songs" + folder playlists
3. **Playlist** - Track list with playing indicator
4. **Search** - Virtual keyboard + real-time results
5. **Settings** - Shuffle, Auto-play, Brightness, Factory Reset

## Project Structure

```
esp32_mp3_player/
├── platformio.ini          # PlatformIO config
├── custom_partitions.csv   # Partition table (OTA + SPIFFS)
├── src/
│   ├── main.cpp           # Entry point, task management
│   ├── config.h           # Pin definitions, constants
│   ├── audio/             # Audio playback, Bluetooth A2DP, SD card
│   ├── ui/                # OLED UI, screens, components
│   ├── input/             # Buttons, potentiometer
│   ├── storage/           # File indexing, playlist management
│   └── utils/             # String, time, memory utilities
├── test/                  # Unit tests (Unity)
├── hardware/              # Wiring guide, parts list, schematics
├── data/                  # SPIFFS files (fonts, images)
└── docs/                  # API reference, troubleshooting
```

## Configuration

Edit `src/config.h` to customize:
- Pin assignments
- Audio buffer sizes
- UI refresh rates
- Button timing
- Power management thresholds

## Power Management

- **Active**: ~150mA (Bluetooth + OLED + SD)
- **Dimmed**: ~100mA (OLED dimmed after 30s)
- **Deep Sleep**: ~10µA (after 10 min paused)
- **Battery Life**: ~20+ hours continuous, 2+ years with sleep

## Troubleshooting

| Issue | Solution |
|-------|----------|
| No sound | Check Bluetooth connection, volume potentiometer |
| SD card not detected | Check wiring, format FAT32, try different card |
| OLED not showing | Check I2C connections, try 0x3D address |
| Buttons not working | Check pull-up resistors, pin assignments |
| Short battery life | Check for shorts, verify deep sleep works |

See [docs/troubleshooting.md](docs/troubleshooting.md) for more details.

## License

MIT License - Feel free to use and modify.

## Credits

- [ESP32-audioI2S](https://github.com/pschatzmann/ESP32-audioI2S) by pschatzmann
- [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306)
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson)

---

**Built with PlatformIO and ESP32 Arduino Core**