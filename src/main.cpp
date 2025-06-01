#include <Arduino.h>
#include <SPI.h>
#include <SD_MMC.h>
#include <ETH.h>
#include <FS.h>
#include <SPIFFS.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <Arduino.h>
#include <SPIFFS.h>


<<<<<<< Updated upstream
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
=======
extern "C" {
  #include "esp_psram.h"
  #include "esp_spi_flash.h"
  #include "esp_flash.h"          // IDF flash API
  #include "esp_err.h"
}
>>>>>>> Stashed changes

#include "esp_task_wdt.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128

#define SSD1351_BLACK   0x0000
#define SSD1351_WHITE   0xFFFF

// ——— SPI / OLED PINS ———
#define OLED_MOSI  13
#define OLED_CLK   14
#define OLED_CS    15
#define OLED_DC    16
#define OLED_RST   17

// ——— ETH CONFIG ———
#define ETH_PHY_TYPE       ETH_PHY_TLK110
#define ETH_ADDR           1
#define ETH_MDC_PIN        31
#define ETH_MDIO_PIN       52
#define ETH_POWER_PIN      51
#define ETH_CLOCK_GPIO_IN  EMAC_CLK_EXT_IN

// Global state for display summary
float flashSizeMB = 0;
float spiffsUsedMB = 0;
float spiffsTotalMB = 0;
float psramSizeMB = 0;
IPAddress ethIP;
int ethSpeedMbps = 0;
bool ethLinkUp = false;


Adafruit_SSD1351 display = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_CS, OLED_DC, OLED_RST);
AsyncWebServer server(80);

<<<<<<< Updated upstream
void wait_for_serial_debug();
void flash_init();
void psram_init();
void sdcard_init();
void ethernet_init();
void spi_init();
void display_init();
void webserver_init();
void display_eth_status();
=======
// ——— SPI / OLED PINS ———
// PUEXT1
#define OLED_MOSI  13
#define OLED_CLK   14
#define OLED_CS    15
#define OLED_DC    16
#define OLED_RST   17

// ——— Adafruit SSD1351 instance ———
Adafruit_SSD1351 display = Adafruit_SSD1351(
  /* width */ 128,
  /* height*/ 128,
  /* spi */   &SPI,
  /* cs */    OLED_CS,
  /* dc */    OLED_DC,
  /* rst */   OLED_RST
);

>>>>>>> Stashed changes

void setup() {
  Serial.begin(115200);

<<<<<<< Updated upstream
  esp_task_wdt_delete(NULL);  // Disable WDT for main task
=======
  Serial.println("[Boot] Flash test");

  // Grab the default flash chip handle
  esp_flash_t *chip = esp_flash_default_chip;
  if (!chip) {
    Serial.println("ERROR: no default flash chip");
    return;
  }

  // 1) Read JEDEC ID
  uint32_t jedec = 0;
  esp_err_t err = esp_flash_read_id(chip, &jedec);
  if (err == ESP_OK) {
    Serial.printf("JEDEC ID : 0x%06X\n", jedec);
  } else {
    Serial.printf("esp_flash_read_id() failed: %d\n", err);
  }

  // 2) Query total size (note: uint32_t)
  uint32_t flash_sz = 0;
  err = esp_flash_get_size(chip, &flash_sz);
  if (err == ESP_OK) {
    Serial.printf("Flash size: %u bytes (%.2f MB)\n",
                  flash_sz, flash_sz / 1024.0 / 1024.0);
  } else {
    Serial.printf("esp_flash_get_size() failed: %d\n", err);
  }

  // 3) Read the first 16 bytes
  const size_t R = 16;
  uint8_t buf[R];
  err = esp_flash_read(chip, buf, /*address=*/0, /*length=*/R);
  if (err == ESP_OK) {
    Serial.print("First 16 bytes: ");
    for (size_t i = 0; i < R; ++i) {
      Serial.printf("%02X ", buf[i]);
    }
    Serial.println();
  } else {
    Serial.printf("esp_flash_read() failed: %d\n", err);
  }

  Serial.println("\n[FS Test]");

  // 1) Mount SPIFFS, format if unformatted
  if (!SPIFFS.begin(/*formatOnFail=*/true)) {
    Serial.println("⚠️ SPIFFS mount failed!");
    return;
  }
  Serial.println("✅ SPIFFS mounted");

  // 2) Report capacity
  size_t total = SPIFFS.totalBytes();
  size_t used  = SPIFFS.usedBytes();
  Serial.printf(" FS total: %u KB\n", total  / 1024);
  Serial.printf(" FS used : %u KB\n", used   / 1024);

  // 3) Write a tiny file
  const char* path = "/hello.txt";
  File f = SPIFFS.open(path, FILE_WRITE);
  if (!f) {
    Serial.println("❌ open for write failed");
  } else {
    f.printf("Hello from P4 @ %lu\n", millis());
    f.close();
    Serial.println("✍️ Wrote /hello.txt");
  }

  // 4) Read it back
  f = SPIFFS.open(path, FILE_READ);
  if (!f) {
    Serial.println("❌ open for read failed");
  } else {
    Serial.print("📖 Content: ");
    while (f.available()) {
      Serial.write(f.read());
    }
    Serial.println();
    f.close();
  }

  // — Init SPI once for both displays —
  SPI.begin(OLED_CLK, /* MISO */ -1, OLED_MOSI, OLED_CS);
>>>>>>> Stashed changes

  void wait_for_serial_debug();

  Serial.println("[Boot] ESP32-P4 Async Web Server Test");

  flash_init();
  psram_init();
  sdcard_init();
  ethernet_init();
  webserver_init();
  spi_init();
  display_init();
}

void loop() {
  // Your loop code here
}

///////////////////////////////////////////////////////////////////////////////////
void wait_for_serial_debug() {
  delay(5000); // Allow USB CDC to re-enumerate on Windows
}

void flash_init() {
  Serial.println("[Flash] Init");
  flashSizeMB = ESP.getFlashChipSize() / 1048576.0;

  if (SPIFFS.begin(true)) {
    Serial.println("✅ SPIFFS mounted");
    spiffsUsedMB = SPIFFS.usedBytes() / 1048576.0;
    spiffsTotalMB = SPIFFS.totalBytes() / 1048576.0;

    File file = SPIFFS.open("/hello.txt", FILE_WRITE);
    if (file) {
      file.printf("Hello from P4 @ %d\n", millis());
      file.close();
      file = SPIFFS.open("/hello.txt");
      if (file) {
        Serial.print("📖 Content: ");
        while (file.available()) Serial.write(file.read());
        file.close();
        Serial.println();
      }
    } else {
      Serial.println("❌ Failed to write file");
    }
  } else {
    Serial.println("❌ SPIFFS mount failed");
  }
}

void psram_init() {
  psramSizeMB = ESP.getPsramSize() / 1048576.0;
  Serial.printf("[PSRAM] %.2f MB\n", psramSizeMB);
}


void sdcard_init() {
  Serial.print("[SD] begin… ");
  if (!SD_MMC.begin("/sdcard", true)) {
    Serial.println("❌ Failed");
  } else {
    Serial.println("OK");
  }
}

void ethernet_init() {
  Serial.println("[ETH] init…");
  ETH.begin();
  ETH.config(IPAddress(0,0,0,0), IPAddress(0,0,0,0), IPAddress(0,0,0,0));
  ETH.setHostname("esp32p4_test");

  WiFi.onEvent([](WiFiEvent_t event) {
    switch (event) {
      case ARDUINO_EVENT_ETH_GOT_IP:
        ethIP = ETH.localIP();
        ethSpeedMbps = ETH.linkSpeed();
        ethLinkUp = true;
        Serial.printf("[ETH] IP: %s  Gateway: %s\n", ethIP.toString().c_str(), ETH.gatewayIP().toString().c_str());
        Serial.printf("[ETH] Speed  : %d Mbps, Duplex: %s\n", ethSpeedMbps, ETH.fullDuplex() ? "FULL" : "HALF");

        display_eth_status();  // Update display
        break;

      case ARDUINO_EVENT_ETH_DISCONNECTED:
        ethLinkUp = false;
        Serial.println("[ETH] Disconnected");

        display_eth_status();  // Clear or update display
        break;

      default:
        break;
    }
  });

}

void webserver_init() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "Hello from ESP32-P4");
  });
  server.begin();
  Serial.println("[HTTP] AsyncWebServer started");
}

void spi_init() {
  SPI.begin(OLED_CLK, /* MISO */ -1, OLED_MOSI, OLED_CS);
  Serial.println("[SPI] bus initialized");
}

void display_init() {
  display.begin();
  display.fillScreen(SSD1351_BLACK);
  display.setTextColor(SSD1351_WHITE);
  display.setCursor(0, 0);
  display.setTextSize(1);

  display.printf("ESP32-P4 TEST\n\n");

  display.printf("Flash: %.2f MB\n\n", flashSizeMB);
  display.printf("FS: %.1f / %.1f MB\n\n", spiffsUsedMB, spiffsTotalMB);
  display.printf("PSRAM: %.2f MB\n", psramSizeMB);

  Serial.println("[Display] initialized");
}

void display_eth_status() {
  display.setCursor(0, 70); // Position below previous text
  display.setTextColor(SSD1351_WHITE, SSD1351_BLACK); // Overwrite previous text

  if (ethLinkUp) {
    display.print("IP: ");
    display.println(ethIP);
    display.printf("%d Mbps\n", ethSpeedMbps);
  } else {
    display.println("ETH: No Link");
  }
}

