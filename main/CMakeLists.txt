idf_component_register(SRCS "main.c"
                            "pn532_wrapper.c"
                       PRIV_REQUIRES spi_flash
                       INCLUDE_DIRS "")
