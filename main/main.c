#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "pn532.h"
#include "pn532_driver_i2c.h"

#define TAG "PN532"

/* ===== I2C CONFIG ===== */
#define I2C_SDA    8
#define I2C_SCL    9
#define RESET_PIN  -1
#define IRQ_PIN    -1
#define I2C_PORT   I2C_NUM_0

static pn532_io_t pn532_io;

/* ===== MAIN ===== */
void app_main(void)
{
    ESP_LOGI(TAG, "Initializing PN532 I2C...");
    
    ESP_ERROR_CHECK(pn532_new_driver_i2c(I2C_SDA, I2C_SCL, RESET_PIN, IRQ_PIN, I2C_PORT, &pn532_io));
    
    ESP_ERROR_CHECK(pn532_init(&pn532_io));

    uint32_t version;
    ESP_ERROR_CHECK(pn532_get_firmware_version(&pn532_io, &version));
    
    ESP_LOGI(TAG, "PN532 Firmware ver. %d.%d", 
             (int)(version >> 16) & 0xFF, 
             (int)(version >> 8) & 0xFF);

    ESP_LOGI(TAG, "Waiting for cards...");

    uint8_t uid[10];
    uint8_t uid_len;

    while (1) {
        esp_err_t err = pn532_read_passive_target_id(
            &pn532_io,
            PN532_BRTY_ISO14443A_106KBPS,
            uid,
            &uid_len,
            1000
        );

        if (err == ESP_OK) {
            ESP_LOGI(TAG, "Card detected, UID length: %d", uid_len);
            printf("UID: ");
            for (int i = 0; i < uid_len; i++) {
                printf("%02X ", uid[i]);
            }
            printf("\n");

            vTaskDelay(pdMS_TO_TICKS(1000));
        }

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
