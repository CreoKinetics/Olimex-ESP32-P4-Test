#pragma once
#include "esp_eth_phy.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_eth_phy_t *esp_eth_phy_new_ip101(const eth_phy_config_t *config);

#ifdef __cplusplus
}
#endif