#include <Arduino.h>
#include "FS.h"
#include "SD_MMC.h"
#include "esp_eth.h"
#include "eth_phy/esp_eth_phy_ip101.h"

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n[Boot] Starting ESP32-P4 Basic Test");

  // PSRAM
  if (psramFound()) {
    Serial.printf("[PSRAM] Detected: %d bytes\n", psramSize());
  } else {
    Serial.println("[PSRAM] Not found.");
  }

  // SD Card
  Serial.print("[SD] Mounting SD_MMC...");
  if (!SD_MMC.begin("/sdcard", true)) {
    Serial.println("Failed!");
  } else {
    Serial.println("OK");
    File root = SD_MMC.open("/");
    while (File file = root.openNextFile()) {
      Serial.printf("  [SD] %s (%d bytes)\n", file.name(), file.size());
      file.close();
    }
  }

  // Ethernet
  Serial.println("[ETH] Initializing Ethernet (IP101GRI)...");
  eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
  eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
  phy_config.phy_addr = 1;
  phy_config.reset_gpio_num = 48;

  eth_esp32_emac_config_t emac_config = {
    .smi_mdc_num = 23,
    .smi_mdio_num = 18,
    .interface = EMAC_IF_RMII,
    .clock_config = {
      .rmii_clock_mode = ETH_CLOCK_GPIO_INPUT,
      .rmii_clock_gpio = 50
    }
  };

  esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&emac_config);
  esp_eth_phy_t *phy = esp_eth_phy_new_ip101(&phy_config);
  esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy);

  esp_eth_handle_t eth_handle = NULL;
  if (esp_eth_driver_install(&config, &eth_handle) == ESP_OK) {
    esp_eth_start(eth_handle);
    Serial.println("[ETH] Driver started.");
  } else {
    Serial.println("[ETH] Failed to install driver.");
  }
}

void loop() {
  delay(10000);
}