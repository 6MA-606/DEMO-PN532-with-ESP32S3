#include "pn532_wrapper.h"
#include "pn532.h"
#include "pn532_driver.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include "sdkconfig.h"

static const char *TAG = "PN532";

/* I2C Configuration - Use Kconfig values if available, otherwise defaults */
#ifdef CONFIG_PN532_I2C_SDA_PIN
#define I2C_SDA_PIN    CONFIG_PN532_I2C_SDA_PIN
#else
#define I2C_SDA_PIN    8
#endif

#ifdef CONFIG_PN532_I2C_SCL_PIN
#define I2C_SCL_PIN    CONFIG_PN532_I2C_SCL_PIN
#else
#define I2C_SCL_PIN    9
#endif

#ifdef CONFIG_PN532_RESET_PIN
#define RESET_PIN      CONFIG_PN532_RESET_PIN
#else
#define RESET_PIN      -1
#endif

#ifdef CONFIG_PN532_IRQ_PIN
#define IRQ_PIN        CONFIG_PN532_IRQ_PIN
#else
#define IRQ_PIN        -1
#endif

#ifdef CONFIG_PN532_I2C_PORT
#define I2C_PORT       CONFIG_PN532_I2C_PORT
#else
#define I2C_PORT       I2C_NUM_0
#endif

void pn532_module_deinit(pn532_io_t *pn532_io)
{
    if (pn532_io == NULL || pn532_io->driver_data == NULL) {
        return;
    }
    
    ESP_LOGI(TAG, "Deinitializing PN532...");
    
    // Release I2C and driver resources
    if (pn532_io->pn532_release_driver != NULL) {
        pn532_io->pn532_release_driver(pn532_io);
    }
    
    ESP_LOGI(TAG, "PN532 deinitialized");
}

esp_err_t pn532_module_init(pn532_io_t *pn532_io)
{
    if (pn532_io == NULL) {
        ESP_LOGE(TAG, "Invalid argument: pn532_io is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    // Clean up any existing resources first
    pn532_module_deinit(pn532_io);
    
    // Small delay to let hardware settle after deinit
    vTaskDelay(pdMS_TO_TICKS(100));

    ESP_LOGI(TAG, "Initializing PN532 with I2C...");
    ESP_LOGI(TAG, "I2C Config: SDA=GPIO%d, SCL=GPIO%d", I2C_SDA_PIN, I2C_SCL_PIN);

    // Initialize I2C driver
    esp_err_t ret = pn532_new_driver_i2c(I2C_SDA_PIN, I2C_SCL_PIN, 
                                         RESET_PIN, IRQ_PIN, 
                                         I2C_PORT, pn532_io);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize I2C driver: %s", esp_err_to_name(ret));
        return ret;
    }

    // Initialize PN532 module
    ret = pn532_init(pn532_io);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize PN532: %s", esp_err_to_name(ret));
        ESP_LOGE(TAG, "Check: 1) DIP switches (I2C mode: SW1=OFF, SW2=ON)");
        ESP_LOGE(TAG, "       2) Wiring (VCC, GND, SDA, SCL)");
        ESP_LOGE(TAG, "       3) Power supply (3.3V or 5V)");
        return ret;
    }

    // Get and display firmware version
    uint32_t version;
    ret = pn532_get_firmware_version(pn532_io, &version);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get firmware version: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG, "PN532 Firmware ver. %d.%d", 
             (int)(version >> 16) & 0xFF, 
             (int)(version >> 8) & 0xFF);
    ESP_LOGI(TAG, "Initialization successful!");

    return ESP_OK;
}

bool pn532_scan_cards(pn532_io_t *pn532_io)
{
    uint8_t uid[10];
    uint8_t uid_len;

    esp_err_t err = pn532_read_passive_target_id(
        pn532_io,
        PN532_BRTY_ISO14443A_106KBPS,
        uid,
        &uid_len,
        500  // 500ms timeout
    );

    if (err == ESP_OK) {
        // Card detected successfully
        ESP_LOGI(TAG, "Card detected! UID length: %d bytes", uid_len);
        printf("UID: ");
        for (int i = 0; i < uid_len; i++) {
            printf("%02X ", uid[i]);
        }
        printf("\n");
        return true;
        
    } else if (err == ESP_ERR_TIMEOUT) {
        // No card detected (normal operation)
        return false;
        
    } else {
        // I2C communication error or other error
        ESP_LOGE(TAG, "I2C Communication Error: %s", esp_err_to_name(err));
        return false;
    }
}
