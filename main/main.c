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
    
    // Start scanning for cards
    pn532_scan_cards(&pn532_io);
}
