# P10 LED Matrix Clock with DS3231 RTC

Real-time clock display on P10 LED panels using Arduino, FreeRTOS, and DS3231 RTC module.

## Hardware Requirements

- Arduino board (Uno/Mega recommended)
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
- Connect P10 panel power to appropriate 5V supply
- Ensure common ground between Arduino and P10 panel
- Arduino can be powered via USB or external supply

**Note:** If using other SPI devices (e.g., Ethernet W5100), connect their CS to D10 to avoid conflicts.

## Dependencies

Install via Arduino Library Manager or PlatformIO's Package Manager or manually:

- `Arduino_FreeRTOS`
- `RTClib` (Adafruit)
- `DMD` (Freetronics DMD library)
- `TimerOne`

## Configuration

Adjust panel count in `main.cpp`:
```cpp
static constexpr uint8_t DISPLAYS_ACROSS = 1;  // Panels horizontally
static constexpr uint8_t DISPLAYS_DOWN    = 1;  // Panels vertically
```

## Features

- **Scrolling text** with time, date, and temperature
- **FreeRTOS tasks**: Separate tasks for RTC reading and display updating
- **Queue-based communication**: Safe data transfer between tasks
- **RTC synchronization**: Auto-sets to compile time on upload
- **Multiple font support**: Uncomment desired font in code

## How It Works

1. **RTC Task** (100ms interval): Reads time/date/temp from DS3231, pushes to queue
2. **Display Task**: Pulls from queue, formats text, renders scrolling marquee
3. **Timer Interrupt**: Scans display at 1kHz for multiplexing

## Build & Upload

1. Open `main.cpp` in Arduino IDE or PlatformIO
2. Select board and port
3. Upload (RTC will sync to compile time)
4. Display starts scrolling immediately

## Customization

**Change scrolling text**: Edit `sprintf()` format string in `p10_display_task()`

**Adjust scroll speed**: Modify `interval` variable (default: 100ms per step)

**Select font**: Uncomment desired font in `p10_display_task()`:
```cpp
dmd.selectFont(Arial_Black_16_ISO_8859_1);  // Current
// dmd.selectFont(SystemFont5x7);           // Alternative
```

## Memory Constraints

Stack sizes tuned for Arduino Uno:
- `rtc_read_task`: 128 words
- `p10_display_task`: 384 words

Adjust if using longer text or experiencing crashes.

## License

Code uses DMD library (GPL v3). Modify pins in `DMD.h` as needed for your setup.