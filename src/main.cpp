#include <Arduino.h>
#include <SPI.h>
#include <SD_MMC.h>
#include <ETH.h>
#include <FS.h>
#include <SPIFFS.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>

#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>

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

void wait_for_serial_debug();
void flash_init();
void psram_init();
void sdcard_init();
void ethernet_init();
void spi_init();
void display_init();
void webserver_init();
void display_eth_status();

void setup() {
  Serial.begin(115200);

  esp_task_wdt_delete(NULL);  // Disable WDT for main task

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

