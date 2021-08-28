#include "udpcontrol.h"

// esp-idf includes
#include <esp_log.h>

// 3rdparty lib includes
#include <asyncudplistener.h>

namespace {
constexpr const char * const TAG = "UDP_CONTROL";

AsyncUdpListener udp;
} // namespace

void udpcontrol_setup()
{
    if (!udp.listen(1234))
    {
        ESP_LOGE(TAG, "could not start listening on udp (port=%i)", 1234);
        return;
    }
}

void udpcontrol_update()
{
    const auto &packet = udp.poll();
    if (!packet)
        return;

    ESP_LOGI(TAG, "%s responded \"%.*s\"", wifi_stack::toString(packet->remoteAddr()).c_str(), packet->data().size(), packet->data().data());
}
