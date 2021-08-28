set(headers
    ble.h
    led.h
    ota.h
    webserver.h
    wifi.h
)

set(sources
    ble.cpp
    led.cpp
    main.cpp
    ota.cpp
    webserver.cpp
    wifi.cpp
)

set(dependencies
    freertos nvs_flash esp_http_server esp_https_ota mdns app_update esp_system esp_websocket_client driver
    arduino-esp32 ArduinoJson FastLED-idf cpputils date espchrono espasyncota espcpputils esp-nimble-cpp
    esphttpdutils espwifistack expected fmt
)

idf_component_register(
    SRCS
        ${headers}
        ${sources}
    INCLUDE_DIRS
        .
    REQUIRES
        ${dependencies}
)

target_compile_options(${COMPONENT_TARGET}
    PRIVATE
        -fstack-reuse=all
        -fstack-protector-all
        -Wno-unused-function
        -Wno-deprecated-declarations
        -Wno-missing-field-initializers
        -Wno-parentheses
)
