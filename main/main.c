#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "pn532_wrapper.h"

#define TAG "MAIN"

static pn532_io_t pn532_io;

void app_main(void)
{
    ESP_LOGI(TAG, "=== PN532 NFC/RFID Reader ===");
    
    // Initialize PN532 module
    esp_err_t ret = pn532_module_init(&pn532_io);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "PN532 initialization failed, halting...");
        return;
    }
    
    // Display auto idle reinit status
    if (pn532_is_auto_idle_reinit_enabled()) {
        ESP_LOGI(TAG, "Auto re-init enabled: %d seconds timeout", 
                 pn532_get_idle_reinit_timeout_sec());
    } else {
        ESP_LOGI(TAG, "Auto re-init disabled");
    }
    
    ESP_LOGI(TAG, "Ready! Waiting for NFC/RFID cards...");
    
    while (1) {
        // Scan for cards with automatic idle re-initialization
        bool card_detected = pn532_scan_cards_with_auto_reinit(&pn532_io);
        
        if (card_detected) {
            // Card detected - wait for card to be removed
            vTaskDelay(pdMS_TO_TICKS(1000));
        } else {
            // No card - wait a bit before next scan
            vTaskDelay(pdMS_TO_TICKS(500));
        }
    }
}
