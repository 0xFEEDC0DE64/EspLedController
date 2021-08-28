#include "webserver.h"

// esp-idf includes
#include <esp_log.h>
#include <esp_http_server.h>

// 3rdparty lib includes
#include <esphttpdutils.h>
#include <espcppmacros.h>
#include <fmt/core.h>
#include <espasyncota.h>

// local includes
#include "wifi.h"
#include "ota.h"

using namespace esphttpdutils;

namespace {
constexpr const char * const TAG = "WEBSERVER";

httpd_handle_t httpdHandle;

esp_err_t webserver_root_handler(httpd_req_t *req);
esp_err_t webserver_ota_handler(httpd_req_t *req);
} // namespace

void webserver_setup()
{
    {
        httpd_config_t httpConfig HTTPD_DEFAULT_CONFIG();
        httpConfig.core_id = 1;
        httpConfig.max_uri_handlers = 14;

        const auto result = httpd_start(&httpdHandle, &httpConfig);
        ESP_LOG_LEVEL_LOCAL((result == ESP_OK ? ESP_LOG_INFO : ESP_LOG_ERROR), TAG, "httpd_start(): %s", esp_err_to_name(result));
        if (result != ESP_OK)
            return;
    }

    for (const httpd_uri_t &uri : {
         httpd_uri_t { .uri = "/",    .method = HTTP_GET, .handler = webserver_root_handler, .user_ctx = NULL },
         httpd_uri_t { .uri = "/ota", .method = HTTP_GET, .handler = webserver_ota_handler,  .user_ctx = NULL },
    })
    {
        const auto result = httpd_register_uri_handler(httpdHandle, &uri);
        ESP_LOG_LEVEL_LOCAL((result == ESP_OK ? ESP_LOG_INFO : ESP_LOG_ERROR), TAG, "httpd_register_uri_handler() for %s: %s", uri.uri, esp_err_to_name(result));
        //if (result != ESP_OK)
        //    return result;
    }
}

void webserver_update()
{

}

namespace {
esp_err_t webserver_root_handler(httpd_req_t *req)
{
    std::string body = fmt::format("<h1>{}</h1>", htmlentities(deviceName));

    if (const auto otaStatus = asyncOta.status(); otaStatus == OtaCloudUpdateStatus::Idle)
    {
        body +=
            "<form action=\"/ota\">"
            "<label>ota url: <input type=\"url\" name=\"url\" required /></label>"
            "<button type=\"submit\">Start</button>"
            "</form>";
    }
    else
    {
        const auto progress = asyncOta.progress();
        const char *color;
        switch (otaStatus)
        {
        case OtaCloudUpdateStatus::Updating:  color = "#b8b800"; break;
        case OtaCloudUpdateStatus::Succeeded: color = "#13c200"; break;
        default:                              color = "#b80000";
        }

        body += fmt::format("<p style=\"color: {};\">OTA: status={} progress={}", color, esphttpdutils::htmlentities(toString(otaStatus)), progress);

        if (const auto totalSize = asyncOta.totalSize())
        {
            body += fmt::format(" totalSize={}", *totalSize);
            if (*totalSize)
                body += fmt::format(" percentage={:.1f}%", 100.f * progress / *totalSize);
        }

        if (const auto &message = asyncOta.message(); !message.empty())
            body += fmt::format(" message={}", esphttpdutils::htmlentities(message));

        body += "</p>\n";
    }

    CALL_AND_EXIT(webserver_resp_send_succ, req, "text/html", body)
}

esp_err_t webserver_ota_handler(httpd_req_t *req)
{
    std::string query;

    if (const size_t queryLength = httpd_req_get_url_query_len(req))
    {
        query.resize(queryLength);
        CALL_AND_EXIT_ON_ERROR(httpd_req_get_url_query_str, req, query.data(), query.size() + 1)
    }

    char urlBufEncoded[256];
    if (const auto result = httpd_query_key_value(query.data(), "url", urlBufEncoded, sizeof(urlBufEncoded)); result == ESP_ERR_NOT_FOUND)
    {
        CALL_AND_EXIT(webserver_resp_send_err, req, HTTPD_400_BAD_REQUEST, "text/plain", "url parameter missing")
    }
    else if (result != ESP_OK)
    {
        const auto msg = fmt::format("httpd_query_key_value() {} failed with {}", "url", esp_err_to_name(result));
        ESP_LOGE(TAG, "%.*s", msg.size(), msg.data());
        CALL_AND_EXIT(webserver_resp_send_err, req, HTTPD_400_BAD_REQUEST, "text/plain", msg)
    }

    char urlBuf[257];
    esphttpdutils::urldecode(urlBuf, urlBufEncoded);

    std::string_view url{urlBuf};

    if (const auto result = triggerOta(url); !result)
    {
        ESP_LOGE(TAG, "%.*s", result.error().size(), result.error().data());
        CALL_AND_EXIT(webserver_resp_send_err, req, HTTPD_400_BAD_REQUEST, "text/plain", result.error())
    }

    CALL_AND_EXIT(webserver_resp_send_succ, req, "text/plain", "OTA called...")
}
}
