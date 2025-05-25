#include <Arduino.h>
#include <ETH.h>
#include "FS.h"
#include "SD_MMC.h"

extern "C" {
  #include "esp_psram.h"
  #include "esp_log.h"
  #include "esp_eth.h"
  #include "esp_eth_phy_ip101.h"
  #include "eth_phy_ip101_relaxed.h"
}

#define ETH_ADDR              0
#define ETH_MDC_PIN           23
#define ETH_MDIO_PIN          18
#define ETH_POWER_PIN         -1
#define ETH_CLK_IN_PIN        50
#define ETH_CLOCK_GPIO_INPUT  EMAC_CLK_EXT_IN

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n[Boot] Starting ESP32-P4 Basic Test");

  if (psramFound()) {
    Serial.printf("[PSRAM] Detected: %d bytes\n", esp_psram_get_size());
  } else {
    Serial.println("[PSRAM] Not found.");
  }

  Serial.print("[SD] Mounting SD_MMC...");
  if (!SD_MMC.begin("/sdcard", true)) {
    Serial.println(" Failed!");
  } else {
    Serial.println(" OK");
    File root = SD_MMC.open("/");
    while (File file = root.openNextFile()) {
      Serial.printf("  [SD] %s (%d bytes)\n", file.name(), file.size());
      file.close();
    }
  }

  Serial.println("[ETH] Starting Ethernet...");
  eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
  phy_config.phy_addr = ETH_ADDR;
  phy_config.reset_gpio_num = ETH_POWER_PIN;

  esp_eth_phy_t* custom_phy = esp_eth_phy_new_ip101_relaxed(&phy_config);

  if (ETH.begin()) {  // or try default overload
//(ETH.begin(custom_phy, ETH_ADDR, ETH_MDC_PIN, ETH_MDIO_PIN, ETH_POWER_PIN, ETH_CLOCK_GPIO_INPUT)) {
    Serial.println("[ETH] Initialization successful.");
  } else {
    Serial.println("[ETH] Initialization failed.");
  }
}

void loop() {
  delay(10000);
}