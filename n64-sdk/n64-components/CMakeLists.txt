idf_component_register(SRCS 
                    "n64-pad.c"
                    INCLUDE_DIRS "include"
                    REQUIRES app_update driver
                    esp_partition esp_system
                    BluControl-SDK-ESP32)
