#pragma once

// system includes
#include <string>
#include <string_view>

// 3rdparty lib includes
#include <tl/expected.hpp>

// forward declares
class EspAsyncOta;

extern EspAsyncOta &asyncOta;

void ota_setup();
void ota_update();

tl::expected<void, std::string> triggerOta(std::string_view url);
