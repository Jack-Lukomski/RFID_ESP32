    # idf_component_register(SRCS "esp_32_test.cpp"
    #                     INCLUDE_DIRS ".")
    idf_component_register(SRCS "esp_32_test.cpp" "../drivers/MFRC522.cpp"
INCLUDE_DIRS "." "../drivers")

