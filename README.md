# P10 LED Matrix Clock with DS3231 RTC

Real-time clock display on P10 LED panels using Arduino, FreeRTOS, and DS3231 RTC module. Built with PlatformIO.

## Hardware Requirements

- Arduino Uno
- P10 LED matrix panel(s)
- DS3231 RTC module
- Power supply (5V, adequate amperage for panels)

## Pin Connections

### P10 Panel to Arduino
```
P10 Pin  →  Arduino Pin
nOE      →  D9  (Output Enable, active low)
A        →  D6  (Row select A)
B        →  D7  (Row select B)
CLK      →  D13 (SPI Clock)
SCLK     →  D8  (Shift clock)
R_DATA   →  D11 (SPI MOSI)
```

### DS3231 RTC to Arduino (I2C)
```
VCC  →  5V
GND  →  GND
SDA  →  A4 (SDA)
SCL  →  A5 (SCL)
```

### Power
- Connect P10 panel power to appropriate 5V supply (check panel specs, typically 2-4A per panel)
- Ensure common ground between Arduino and P10 panel
- Arduino can be powered via USB during development, external 7-12V supply for production

**Note:** If using other SPI devices (e.g., Ethernet W5100), connect their CS to D10 to avoid conflicts.

## Project Structure

```
├── src/
│   └── main.cpp          # Main application code
├── include/
├── lib/                  # DMD library (custom/local)
├── platformio.ini        # PlatformIO configuration
└── README.md
```

## Dependencies

Managed automatically via `platformio.ini`:
- `FreeRTOS` (10.4.6-10) - RTOS kernel
- `RTClib` (2.1.4) - Adafruit RTC library
- `TimerOne` (1.1.1) - Hardware timer for display scanning
- `DMD` - Freetronics DMD library (local in `lib/` folder)

**Note:** DMD library must be manually placed in `lib/` folder as it's not in PlatformIO registry. Download from [Freetronics DMD GitHub](https://github.com/freetronics/DMD) or keep existing local copy.

## Build & Upload

```bash
# Build project
pio run

# Upload to Arduino Uno
pio run -t upload

# Build and upload
pio run -t upload

# Monitor serial output (115200 baud)
pio device monitor -b 115200
```

## Configuration

### Panel Count
Edit in `src/main.cpp`:
```cpp
static constexpr uint8_t DISPLAYS_ACROSS = 1;  // Panels horizontally
static constexpr uint8_t DISPLAYS_DOWN    = 1;  // Panels vertically
```

### Change Board Target
Edit `platformio.ini` to use different Arduino board:
```ini
[env:mega]
platform = atmelavr
board = megaatmega2560
framework = arduino
lib_deps = ...
```

## Features

- **Scrolling text** displaying time, date, and DS3231 temperature
- **FreeRTOS multitasking**: Separate tasks for RTC reading and display updates
- **Queue-based IPC**: Thread-safe data transfer between tasks
- **Auto RTC sync**: Sets RTC to compile time on first upload
- **Multiple fonts**: Arial_Black_16, SystemFont5x7, Arial14 (selectable in code)

## How It Works

1. **rtc_read_task** (Priority 2): Reads DS3231 every 100ms, sends data via queue
2. **p10_display_task** (Priority 1): Receives data, formats text, renders scrolling marquee
3. **Timer1 ISR** (1kHz): Continuously scans display rows for multiplexing
4. **FreeRTOS Scheduler**: Manages task execution and context switching

## Customization

### Change Scrolling Text
Edit format string in `src/main.cpp`, `p10_display_task()`:
```cpp
sprintf(p10_display_text, "Your custom text here - Time: %02d:%02d:%02d", 
        data.hour, data.minute, data.second);
```

### Adjust Scroll Speed
Modify `interval` in marquee loop (default 100ms):
```cpp
uint8_t interval = 50;  // Faster scrolling
```

### Select Font
Uncomment desired font in `p10_display_task()`:
```cpp
dmd.selectFont(Arial_Black_16_ISO_8859_1);  // Current (best for Latin chars)
// dmd.selectFont(Arial_Black_16);           // Standard ASCII
// dmd.selectFont(SystemFont5x7);            // Smaller, more text fits
```

## Troubleshooting

| Issue | Solution |
|-------|----------|
| Build fails: DMD not found | Place DMD library in `lib/` folder, ensure DMD.h and DMD.cpp present |
| No display output | Check 5V power supply amperage (min 2A), verify pin connections |
| RTC not found error | Verify I2C wiring (SDA→A4, SCL→A5), check DS3231 has power |
| Garbled display | Confirm common ground, check for SPI conflicts (D10 device CS high) |
| Incorrect time | Re-upload firmware to sync RTC to compile time |
| Upload fails | Check USB connection, verify correct board in platformio.ini |
| Out of memory | Reduce `p10_display_text` buffer size or shorten display text |

## Memory Usage

Tuned for Arduino Uno (2KB RAM):
- **rtc_read_task**: 128 words stack
- **p10_display_task**: 384 words stack  
- **Text buffer**: 150 bytes
- **DMD framebuffer**: 64 bytes per panel

For longer text or more panels, use Arduino Mega with more RAM.

## Pin Modification

To change P10 pins, edit `lib/DMD/DMD.h`:
```cpp
#define PIN_DMD_nOE       9
#define PIN_DMD_A         6
#define PIN_DMD_B         7
// ... etc
```

Rebuild after changes: `pio run -t clean && pio run`

## License

Code uses Freetronics DMD library (GPL v3). Project code follows same license.

## Additional Resources

- [PlatformIO Docs](https://docs.platformio.org)
- [FreeRTOS Arduino](https://github.com/feilipu/Arduino_FreeRTOS_Library)
- [DMD Library](https://github.com/freetronics/DMD)
- [DS3231 Datasheet](https://www.analog.com/media/en/technical-documentation/data-sheets/DS3231.pdf)

## Link to video
https://youtu.be/WG1ywbPXsYI?si=4tYHl-6cnaJc91U0