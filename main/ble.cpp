#include "ble.h"

// esp-idf includes
#include <esp_log.h>

// 3rdparty lib includes
#include <NimBLEDevice.h>
#include <ArduinoJson.h>

// local includes
#include "wifi.h"

namespace {
constexpr const char * const TAG = "BLE";

BLEServer *server{};
BLEService *service{};
BLECharacteristic *characteristic{};

class BleCallbacks : public NimBLECharacteristicCallbacks
{
public:
    void onWrite(NimBLECharacteristic* pCharacteristic) override;
};

BleCallbacks bleCallbacks;
}

void ble_setup()
{
    BLEDevice::init(deviceName);

    const auto serviceUuid{"26e743b6-8cf2-43b0-8bd0-f762a03e120e"};

    server = BLEDevice::createServer();

    service = server->createService(serviceUuid);

    characteristic = service->createCharacteristic("87d4bcc1-233f-4317-87e9-02b93f60f27b", NIMBLE_PROPERTY::WRITE);
    characteristic->setCallbacks(&bleCallbacks);

    service->start();

    BLEAdvertising *advertising = BLEDevice::getAdvertising();
    advertising->addServiceUUID(serviceUuid);
    advertising->setScanResponse(true);
    BLEDevice::startAdvertising();
}

void ble_update()
{

}

namespace {
void BleCallbacks::onWrite(NimBLECharacteristic* pCharacteristic)
{
    const auto &val = pCharacteristic->getValue();

    StaticJsonDocument<256> doc;
    if (const auto error = deserializeJson(doc, val))
    {
        ESP_LOGW(TAG, "ignoring cmd with invalid json: %.*s %s", val.size(), val.data(), error.c_str());
        return;
    }

    // TODO
}
}
