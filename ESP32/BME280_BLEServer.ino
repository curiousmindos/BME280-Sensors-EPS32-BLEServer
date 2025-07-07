/*
   Based on Oleg Semyonov example for BME280+BLE: https://github.dev/curiousmindos/BME280-Sensors-EPS32-BLEServer/tree/main
   Ported to Arduino ESP32 by Oleg Semyonov
*/

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

// I2C pins for ESP32
#define SDA_PIN 21
#define SCL_PIN 22
#define SEALEVELPRESSURE_HPA (1013.25)

#define ledPin 2

// BLE UUIDs
#define SERVICE_UUID        "12345678-1234-1234-1234-1234567890ab"
#define CHARACTERISTIC_UUID "abcdefab-1234-5678-1234-abcdefabcdef"

// I2C
Adafruit_BME280 bme;

BLECharacteristic *pCharacteristic;
BLEServer *pServer;

bool deviceConnected = false;
bool oldDeviceConnected = false;

class Esp32BLEServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    digitalWrite(ledPin, HIGH);
  };

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    digitalWrite(ledPin, LOW);
  }
};


void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);
  pinMode(ledPin, OUTPUT);

  if (!bme.begin(0x76)) {
    Serial.println("Could not find BME280 sensor!");
    while (1);
  }

  // BLE Setup
  BLEDevice::init("ESP32-BME280-v1");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new Esp32BLEServerCallbacks());
  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
  
  // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
  // Create a BLE Descriptor
  pCharacteristic->addDescriptor(new BLE2902());
  
  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  //pAdvertising->setScanResponse(false);
  //pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  pAdvertising->start();

  Serial.println("BLE BME280 Sensor is advertising...");
}

void loop() {

  if (deviceConnected) {
    float temp = bme.readTemperature();
    float hum = bme.readHumidity();
    float pres = bme.readPressure() / 100.0F; // Convert Pa to hPa
    float alt = bme.readAltitude(SEALEVELPRESSURE_HPA);

    char payload[64];
    snprintf(payload, sizeof(payload), "%.2f#%.2f#%.2f#%.2f", temp, hum, pres, alt);

    Serial.println(payload);
    pCharacteristic->setValue(payload);
    pCharacteristic->notify(); // Optional: send update

    delay(5000); // Update every 5 seconds
  }
  
  // disconnecting
  if (!deviceConnected && oldDeviceConnected) {
    Serial.println("Device disconnected.");
    delay(500); // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
    Serial.println("Start advertising");
    oldDeviceConnected = deviceConnected;
  }

  // connecting
  if (deviceConnected && !oldDeviceConnected) {
    // do stuff here on connecting
    oldDeviceConnected = deviceConnected;
    Serial.println("Device Connected");
  }
}
