#include "esp_eth_phy_ip101.h"
#include "esp_log.h"

static const char *TAG = "ip101_relaxed";

// Static wrapper that disables the chip ID check
static esp_err_t custom_ip101_init(esp_eth_phy_t *phy) {
    esp_err_t ret = esp_eth_phy_new_ip101(NULL)->init(phy);  // Call the original
    if (ret == ESP_FAIL) {
        ESP_LOGW(TAG, "Ignoring chip ID mismatch (patched driver)");
        return ESP_OK;
    }
    return ret;
}

esp_eth_phy_t *esp_eth_phy_new_ip101_relaxed(const eth_phy_config_t *config) {
    esp_eth_phy_t *phy = esp_eth_phy_new_ip101(config);
    if (phy) {
        phy->init = custom_ip101_init;
    }
    return phy;
}
