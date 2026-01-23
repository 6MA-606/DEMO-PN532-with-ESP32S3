#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "pn532_wrapper.h"

#define TAG "MAIN"
#define NO_CARD_TIMEOUT_SEC 5

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
    
    ESP_LOGI(TAG, "Ready! Waiting for NFC/RFID cards...");
    ESP_LOGI(TAG, "Auto re-init if no card detected for %d seconds", NO_CARD_TIMEOUT_SEC);
    
    int64_t last_card_time = esp_timer_get_time() / 1000000; // Convert to seconds
    int scan_count = 0;
    
    while (1) {
        // Scan for cards
        bool card_detected = pn532_scan_cards(&pn532_io);
        
        if (card_detected) {
            // Reset timer when card is detected
            last_card_time = esp_timer_get_time() / 1000000;
            scan_count = 0;
            
            // Wait for card to be removed
            vTaskDelay(pdMS_TO_TICKS(1000));
        } else {
            // No card detected
            int64_t current_time = esp_timer_get_time() / 1000000;
            int64_t elapsed = current_time - last_card_time;
            
            scan_count++;
            if (scan_count % 10 == 0) {  // Every ~5 seconds
                ESP_LOGI(TAG, "Scanning... (no card for %lld sec)", elapsed);
            }
            
            // Check if timeout reached
            if (elapsed >= NO_CARD_TIMEOUT_SEC) {
                ESP_LOGW(TAG, "No card detected for %d seconds - Re-initializing PN532...", NO_CARD_TIMEOUT_SEC);
                ret = pn532_module_init(&pn532_io);
                if (ret == ESP_OK) {
                    ESP_LOGI(TAG, "PN532 re-initialized successfully");
                    last_card_time = esp_timer_get_time() / 1000000;
                } else {
                    ESP_LOGE(TAG, "PN532 re-initialization failed");
                }
                scan_count = 0;
            }
            
            vTaskDelay(pdMS_TO_TICKS(500));
        }
    }
}
