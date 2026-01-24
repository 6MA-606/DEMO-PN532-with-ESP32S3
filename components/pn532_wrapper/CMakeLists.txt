idf_component_register(
    SRCS "pn532_wrapper.c"
    INCLUDE_DIRS "include"
    REQUIRES "garag__esp-idf-pn532"
    PRIV_REQUIRES "esp_timer"
)
