#include "wifi.h"

// esp-idf includes
#include <esp_log.h>

// 3rdparty lib includes
#include <espwifistack.h>

namespace {
constexpr const char * const TAG = "WIFI";
wifi_stack::config makeWifiConfig();
} // namespace

char deviceName[32] = "ledcontrol";

void wifi_setup()
{
    if (const auto result = wifi_stack::get_default_mac_addr())
        std::sprintf(deviceName, "ledcontrol_%02hhx%02hhx%02hhx", result->at(3), result->at(4), result->at(5));
    else
        ESP_LOGE(TAG, "get_default_mac_addr() failed: %.*s", result.error().size(), result.error().data());

    ESP_LOGI(TAG, "deviceName = %s", deviceName);

    wifi_stack::init(makeWifiConfig());
}

void wifi_update()
{
    wifi_stack::update(makeWifiConfig());
}

namespace {
wifi_stack::config makeWifiConfig()
{
    return wifi_stack::config {
        .wifiEnabled = true,
        .hostname = deviceName,
        .sta = {
            .wifis = std::array<wifi_stack::wifi_entry, 10> {
                wifi_stack::wifi_entry { .ssid = "camp++", .key = {} },
                wifi_stack::wifi_entry { .ssid = {}, .key = {} },
                wifi_stack::wifi_entry { .ssid = {}, .key = {} },
                wifi_stack::wifi_entry { .ssid = {}, .key = {} },
                wifi_stack::wifi_entry { .ssid = {}, .key = {} },
                wifi_stack::wifi_entry { .ssid = {}, .key = {} },
                wifi_stack::wifi_entry { .ssid = {}, .key = {} },
                wifi_stack::wifi_entry { .ssid = {}, .key = {} },
                wifi_stack::wifi_entry { .ssid = {}, .key = {} },
                wifi_stack::wifi_entry { .ssid = {}, .key = {} }
            },
            .min_rssi = -90
        },
        .ap = {
            .ssid = deviceName,
            .key = "CampPlusPlus",
            .static_ip = {
                .ip = {192, 168, 0, 1},
                .subnet = {255, 255, 255, 0},
                .gateway = {192, 168, 0, 1}
            },
            .channel = 1,
            .authmode = WIFI_AUTH_WPA2_PSK,
            .ssid_hidden = false,
            .max_connection = 4,
            .beacon_interval = 100
        }
    };
}
}
