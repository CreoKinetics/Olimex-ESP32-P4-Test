# 🚀 ESP32-P4 System Test Project

This repository contains a modular test and integration suite for the **Olimex ESP32-P4 DevKit**, leveraging the Arduino framework and PlatformIO. It provides a complete initialization and diagnostics environment for testing core hardware peripherals such as Ethernet, PSRAM, SD card (4-bit SDIO), SPI, and display.

---

## 🧩 Features

- ✅ Modular setup structure with per-subsystem init functions
- ✅ SPI bus and OLED display support (Adafruit SSD1351)
- ✅ SPIFFS flash initialization and file write test
- ✅ PSRAM detection and reporting
- ✅ SD_MMC card support in **4-bit SDIO mode**
- ✅ Ethernet with dynamic link detection and IP display
- ✅ Async web server with root handler
- ✅ OLED splash and diagnostics display
- ✅ Serial debugging with structured messages
- 🧪 Support for future expansion: RGB control, sensors, REST endpoints

---

## 🛠️ Hardware Requirements

| Component             | Notes                               |
|----------------------|--------------------------------------|
| Olimex ESP32-P4 DevKit | Tested with 16MB Flash, 32MB PSRAM  |
| SSD1351 128x128 OLED | SPI mode with MOSI/CLK/CS/DC/RST    |
| SD card via SD_MMC   | **4-bit SDIO mode enabled**          |
| Ethernet PHY         | Internal on ESP32-P4                 |

**OLED Pin Mapping**:
```cpp
#define OLED_MOSI  13
#define OLED_CLK   14
#define OLED_CS    15
#define OLED_DC    16
#define OLED_RST   17
```

---

## 📦 Library Dependencies (via `platformio.ini`)

```ini
lib_deps =
  me-no-dev/AsyncTCP
  me-no-dev/ESPAsyncWebServer
  adafruit/Adafruit GFX Library
  adafruit/Adafruit SSD1351 library
```

---

## 🔧 PlatformIO Setup

### 1. Clone the Repository

```bash
git clone https://github.com/YourUsername/Olimex-ESP32-P4-Test.git
cd Olimex-ESP32-P4-Test
```

### 2. Build and Upload (USB or JTAG)

Ensure your PlatformIO environment is set to `olimex-esp32-p4`. You can upload via USB serial or JTAG depending on your config.

```bash
pio run --target upload
```

---

## 🐛 Debugging

Debug via JTAG with:

```bash
pio run --target upload --upload-port custom
```

Logs are available on the serial port (default: 115200 baud). Initial boot messages may require a delay due to USB reconnection.

---

## 📁 File Structure

```
/src
  └── main.cpp               # Full modular init sequence
/openocd-configs
  └── board/esp32-p4.cfg     # Optional custom JTAG configs
/boards
  └── default_16MB.csv       # Custom partition table (optional)
platformio.ini               # PlatformIO config
```

---

## 📈 Roadmap

- [ ] RGB + CCT LED output calibration
- [ ] JSON-based control over UART
- [ ] Web UI with full diagnostics and tuning
- [ ] CRMX/DMX integration for lighting control
- [ ] Auto-calibration with color meter or internal sensor
- [ ] Storage in binary LUT format for performance

---

## ⚖️ License

MIT License © 2025 [CreoKinetics Ltd](https://www.creokinetics.com)

---

## 🤝 Contributing

PRs and issues welcome! Let us know if you’re testing different board variants or expanding the system.
