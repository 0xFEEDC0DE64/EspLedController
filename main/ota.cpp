#include "ota.h"

// esp-idf includes
#include <esp_log.h>

// 3rdparty lib includes
#include <delayedconstruction.h>
#include <espasyncota.h>
#include <espwifistack.h>

// local includes
#include "wifi.h"

namespace {
constexpr const char * const TAG = "OTA";

cpputils::DelayedConstruction<EspAsyncOta> _asyncOta;
} // namespace

EspAsyncOta &asyncOta{_asyncOta.getUnsafe()};

void ota_setup()
{
    ESP_LOGI(TAG, "called");

    _asyncOta.construct();

    if (const auto result = _asyncOta->startTask(); !result)
    {
        ESP_LOGE(TAG, "starting OTA task failed: %.*s", result.error().size(), result.error().data());
        return;
    }
}

void ota_update()
{
    _asyncOta->update();
}

tl::expected<void, std::string> triggerOta(std::string_view url)
{
    ESP_LOGI(TAG, "%.*s", url.size(), url.data());

    if (const auto result = _asyncOta->trigger(url, {}, {}, {}); !result)
        return tl::make_unexpected(std::move(result).error());

    wifi_stack::delete_scan_result();

    return {};
}
