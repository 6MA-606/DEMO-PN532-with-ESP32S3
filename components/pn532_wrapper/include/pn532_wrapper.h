#ifndef PN532_WRAPPER_H
#define PN532_WRAPPER_H

#include "esp_err.h"
#include "pn532_driver_i2c.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize PN532 module with I2C communication
 * 
 * This function sets up the I2C driver and initializes the PN532 module.
 * It also retrieves and logs the firmware version.
 * 
 * @param[out] pn532_io Pointer to PN532 IO structure
 * @return 
 *     - ESP_OK: Success
 *     - ESP_ERR_INVALID_ARG: Invalid argument
 *     - ESP_ERR_INVALID_STATE: I2C communication failed
 *     - Other error codes from PN532 driver
 */
esp_err_t pn532_module_init(pn532_io_t *pn532_io);

/**
 * @brief Deinitialize PN532 module and release I2C resources
 * 
 * This function cleans up the I2C driver and releases all resources.
 * Call this before re-initializing after power loss.
 * 
 * @param[in] pn532_io Pointer to PN532 IO structure
 */
void pn532_module_deinit(pn532_io_t *pn532_io);

/**
 * @brief Scan for NFC/RFID cards continuously
 * 
 * This function runs an infinite loop scanning for NFC/RFID cards.
 * When a card is detected, it prints the UID.
 * Handles I2C communication errors with helpful diagnostics.
 * 
 * @param[in] pn532_io Pointer to initialized PN532 IO structure
 * @return 
 *     - true: Card detected
 *     - false: No card detected or error occurred
 */
bool pn532_scan_cards(pn532_io_t *pn532_io);

/**
 * @brief Scan for NFC/RFID cards with auto idle re-initialization
 * 
 * This function scans for cards and automatically handles idle re-initialization
 * based on configuration. It tracks time since last card detection and
 * re-initializes the module if timeout is reached (when enabled).
 * 
 * @param[in] pn532_io Pointer to initialized PN532 IO structure
 * @return 
 *     - true: Card detected
 *     - false: No card detected or error occurred
 */
bool pn532_scan_cards_with_auto_reinit(pn532_io_t *pn532_io);

/**
 * @brief Reset the idle timer for auto re-initialization
 * 
 * Resets the internal timer used for tracking idle time.
 * Useful when you want to manually reset the timeout counter.
 */
void pn532_reset_idle_timer(void);

/**
 * @brief Check if auto idle re-initialization is enabled
 * 
 * This function returns the compile-time configuration setting
 * for auto idle re-initialization feature.
 * 
 * @return 
 *     - true: Auto idle reinit is enabled
 *     - false: Auto idle reinit is disabled
 */
bool pn532_is_auto_idle_reinit_enabled(void);

/**
 * @brief Get the idle re-initialization timeout in seconds
 * 
 * Returns the configured timeout period for auto re-initialization
 * when no card is detected.
 * 
 * @return Timeout in seconds (0 if feature is disabled)
 */
int pn532_get_idle_reinit_timeout_sec(void);

#ifdef __cplusplus
}
#endif

#endif /* PN532_WRAPPER_H */
