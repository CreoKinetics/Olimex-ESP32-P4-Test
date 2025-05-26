#include <Arduino.h>
#include <ETH.h>
#include "FS.h"
#include "SD_MMC.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

extern "C" {
  #include "esp_psram.h"
}

// ——— CONFIG ———
#define ETH_PHY_TYPE       ETH_PHY_TLK110
#define ETH_ADDR           1
#define ETH_MDC_PIN        31
#define ETH_MDIO_PIN       52
#define ETH_POWER_PIN      51
#define ETH_CLOCK_GPIO_IN  EMAC_CLK_EXT_IN

AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n[Boot] ESP32-P4 Async Web Server Test");

  // — Init PSRAM —
  if (psramFound()) {
    Serial.printf("[PSRAM] %u bytes\n", esp_psram_get_size());
  }

  // — Mount SD card —
  Serial.print("[SD] begin… ");
  if (!SD_MMC.begin("/sdcard", true)) {
    Serial.println("FAIL");
    while (1) delay(500);
  }
  Serial.println("OK");

  // — Start Ethernet —
  Serial.println("[ETH] init…");
  if (!ETH.begin(
        ETH_PHY_TYPE, ETH_ADDR,
        ETH_MDC_PIN, ETH_MDIO_PIN,
        ETH_POWER_PIN, ETH_CLOCK_GPIO_IN
      )) {
    Serial.println("[ETH] init FAILED");
    while (1) delay(500);
  }

  // Wait for link up
  Serial.print("[ETH] waiting for link");
  while (!ETH.linkUp()) {
    Serial.print('.');
    delay(200);
  }
  Serial.println(" OK");

  // Wait for DHCP
  Serial.print("[ETH] DHCP");
  while (ETH.localIP() == INADDR_NONE) {
    Serial.print('.');
    delay(200);
  }
  Serial.println();

  // Print network info
  Serial.printf("[ETH] IP: %s  Gateway: %s\n",
    ETH.localIP().toString().c_str(),
    ETH.gatewayIP().toString().c_str()
  );
  Serial.print("[ETH] Speed  : ");
  Serial.print(ETH.linkSpeed());
  Serial.print(" Mbps, Duplex: ");
  Serial.println(ETH.fullDuplex() ? "FULL" : "HALF");

  // — Setup AsyncWebServer to serve /www/ from SD —
  // All URLs under “/” map to files in /sdcard/www/, defaulting to index.html
  server.serveStatic("/", SD_MMC, "/www/").setDefaultFile("index.html");

  server.onNotFound([](AsyncWebServerRequest *req){
    req->send(404, "text/plain", "Not found");
  });

  server.begin();
  Serial.println("[HTTP] AsyncWebServer started");
}

void loop() {
  // nothing needed here!
}
