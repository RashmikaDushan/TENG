#include <Arduino.h>
#include <CyclicBuffer.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// definitions
#define sensorPin 36
#define bufferSize 10

#define SERVICE_UUID "349ecf79-ac9d-484f-9d93-b25e91613f78"
#define CHARACTERISTIC_UUID "c3fd1614-8aec-4c3d-b7b9-b2aafdfbec86"

// global variables
CyclicBuffer buffer(bufferSize);
uint8_t reading = 0;
uint8_t byteArray[bufferSize * sizeof(uint8_t)];

BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

// function declarations
void collectData(void *Parameters);
void sendData(void *Parameters);

class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    deviceConnected = true;
  };

  void onDisconnect(BLEServer *pServer)
  {
    deviceConnected = false;
  }
};

void setup() {
    Serial.begin(115200);
    pinMode(sensorPin, INPUT);
    analogReadResolution(8);

      // Create the BLE Device
  BLEDevice::init("TENG Sensor");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_INDICATE);

    pCharacteristic->addDescriptor(new BLE2902());

    // Start the service
    pService->start();

    // Start advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(0x0); // set value to 0x00 to not advertise this parameter
    BLEDevice::startAdvertising();
    Serial.println("Waiting a client connection to notify...");

    xTaskCreate(collectData, "Collecting Data", 10000, NULL, 3, NULL);

    xTaskCreate(sendData, "Sending Data", 10000, NULL, 3, NULL);
}

void loop() {
}

void collectData(void *Parameters) {
    for(;;) {
        reading = analogRead(sensorPin);
        Serial.print("Reading: ");
        Serial.printf("%02X",reading);
        Serial.println();
        buffer.push(reading);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void sendData(void *Parameters) {
    for(;;) {
        if (deviceConnected)
    {
      buffer.toByteArray(byteArray);
      buffer.printHex(true,true);
      pCharacteristic->setValue(byteArray, bufferSize * sizeof(uint8_t));
      pCharacteristic->indicate();
      vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    // disconnecting
    if (!deviceConnected && oldDeviceConnected)
    {
      delay(500);                  // give the bluetooth stack the chance to get things ready
      pServer->startAdvertising(); // restart advertising
      Serial.println("start advertising");
      oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected)
    {
      // do stuff here on connecting
      oldDeviceConnected = deviceConnected;
    }
    }
}