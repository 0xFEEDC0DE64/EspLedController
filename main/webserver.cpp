#include "webserver.h"

// esp-idf includes
#include <esp_log.h>
#include <esp_http_server.h>

// 3rdparty lib includes
#include <esphttpdutils.h>
#include <espcppmacros.h>

// local includes

using namespace esphttpdutils;

namespace {
constexpr const char * const TAG = "WEBSERVER";

httpd_handle_t httpdHandle;

esp_err_t webserver_root_handler(httpd_req_t *req);
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
         httpd_uri_t { .uri = "/",             .method = HTTP_GET, .handler = webserver_root_handler,             .user_ctx = NULL },
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
    std::string body = "hello world";

    CALL_AND_EXIT(webserver_resp_send_succ, req, "text/html", body)
}
}
